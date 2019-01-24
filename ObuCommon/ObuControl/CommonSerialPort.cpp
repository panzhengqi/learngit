#include "CommonSerialPort.h"

/****************************
 * CommonSerialPort.cpp
 *
 * Created on:2018-5-18
 *
 * Author:JingHanMing
 *
 ****************************/

using namespace std;
pthread_mutex_t CommonSerialPort::testlock;
pthread_t pid = 0;
pthread_t pid_control = 0;

/************************************
 功能：构造函数

 参数：无

 返回值：无
 ***********************************/
CommonSerialPort::CommonSerialPort()
{
    this->fd = 0;
    this->epid = epoll_create(6);
    pthread_mutex_init(&testlock, NULL);

}

/************************************
 功能：根据端口号打开对应的com口,并创建数据接受线程

 参数：const int Com_port 端口号

 返回值： 0：则失败，否则返回1
 ***********************************/
bool CommonSerialPort::OpenCom(const int Com_port)
{   
    char pathname[20];

    if ((Com_port < 0) || (Com_port > 6)) //判断Com_port是否越界。
    {
      printf("the port is out range");
      return false;
     }

     sprintf(pathname, "/dev/ttyS%d", Com_port);

     if (fd != 0)
     { //判断文件__detachstate是否被占用。
        printf("com is busying!\n");
        return false;
     }

     fd = open(pathname, O_RDWR | O_NOCTTY | O_NDELAY);

     if (fd < 0) //如果文件操作符小于0表示打开文件失败。
     {
        printf("Can‘t open serial port");
        return false;
     }
     else
     {
           printf(" open serial port/dev/ttyS%d\n",Com_port);


     }
     if (isatty(fd) == 0) //判断文件操作符所代表的文件是否为非终端。
     {
       printf("isatty is not a terminal device");
       return false;
     }
     //下面开始创建接受线程。
     pthread_attr_t attr;
     pthread_attr_init(&attr);
     // 设置线程绑定属性
     int res = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
     // 设置线程分离属性
     res += pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
     //创建线程

    int pd = pthread_create(&pid, &attr, ReadThreadFunction, (void *) this);
    if(pd<0)
    {
        return false;
    }
    // DataControl();
//     pd = pthread_create(&pid_control, NULL, ReadDataProc, (void *) this);
//     if(pd<0)
//     {
//         return false;
//     }

     return true;
}

/************************************
 功能：配置com口

 参数：const int Baud_rate 波特率，const int Data_bits 数据位 ， char Parity 校验位,const int Stop_bits 停止位

 返回值： 0：则失败，否则返回1
 ***********************************/
bool CommonSerialPort::SetCom(const int Baud_rate, const int Data_bits, char Parity, const int Stop_bits)
{

    bool result = false;
     struct termios newtio;
     termios oldtio;
     tcgetattr(fd,&oldtio);
     bzero(&newtio, sizeof(newtio));

     newtio.c_cflag |= CLOCAL | CREAD;
     //newtio.c_cflag &= ~OPOST;
     newtio.c_cflag &= ~CSIZE;
    // newtio.c_cflag &= ~CRTSCTS;//  不使用流控制
    // newtio.c_cflag |= CRTSCTS;//   使用硬件刘控制
    // newtio.c_cflag |=  IXON|IXOFF|IXANY;//  使用软件流控制
    // newtio.c_iflag = IGNPAR;
   //  newtio.c_oflag = 0;
    // newtio.c_lflag = 0;


     //设置波特率。
     switch (Baud_rate) {
     case 2400:
      cfsetispeed(&newtio, B2400);
      cfsetospeed(&newtio, B2400);
      break;
     case 4800:
      cfsetispeed(&newtio, B4800);
      cfsetospeed(&newtio, B4800);
      break;
     case 9600:
      cfsetispeed(&newtio, B9600);
      cfsetospeed(&newtio, B9600);
      break;
     case 19200:
      cfsetispeed(&newtio, B19200);
      cfsetospeed(&newtio, B19200);
      break;
     case 38400:
      cfsetispeed(&newtio, B38400);
      cfsetospeed(&newtio, B38400);
      break;
     default:
     case 115200:
      cfsetispeed(&newtio, B115200);
      cfsetospeed(&newtio, B115200);
      break;
     }

     //设置数据位，只支持7，8
     switch (Data_bits) {
     case 7:
      newtio.c_cflag |= CS7;
      break;
     case 8:
      newtio.c_cflag |= CS8;
      break;
     default:
      printf("Unsupported Data_bits\n");
      return result;
     }

     //设置校验位
     switch (Parity)
     {
         default:
         case 'N':
         case 'n':
         {
            newtio.c_cflag &= ~PARENB;
            newtio.c_iflag &= ~INPCK;
         }
         break;
         case 'o':
         case 'O':
         {
            newtio.c_cflag |= (PARODD | PARENB);
            newtio.c_iflag |= INPCK;
         }
        break;
         case 'e':
         case 'E':
         {
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            newtio.c_iflag |= INPCK;
         }
         break;

         case 's':
         case 'S':
          {
            newtio.c_cflag &= ~PARENB;
            newtio.c_cflag &= ~CSTOPB;
         }
        break;
     }
     //设置停止位，值为1 or 2
     switch (Stop_bits) {
     case 1: {
      newtio.c_cflag &= ~CSTOPB;
      break;
     }
     case 2: {
      newtio.c_cflag |= CSTOPB;
      break;
     }
     default:
      printf("Unsupported Stop_bits.\n");
      return result;

     }
      //newtio.c_iflag = IGNPAR | ICRNL;//忽略奇偶校验，映射CR
  //   newtio.c_oflag = 0;     //输出模式为RAW模式
   //  newtio.c_lflag = ICANON;//本地模式，不回显

     //设置最少字符和等待时间，对于接收字符和等待时间没有特别的要求时，可设为0：
     newtio.c_cc[VTIME] = 0;
     newtio.c_cc[VMIN] = SERIALSIZE;

    // newtio.c_cflag &=~OPOST;
     //刷清输入和输出队列
     tcflush(fd, TCIOFLUSH);

     //激活配置，TCSANOW表示更改后立即生效。
     if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)//判断是否激活成功。
     {
        printf("Com set error\n");
        return result;
     }
     result = true;

     QObject::connect(this,SIGNAL(SendToGpsAnlyse(char*,int)),gps,SLOT(gps_analyse(char*,int)));
     return result;
}

/************************************
 功能：数据接收线程的函数

 参数：对象指针

 返回值： 无
 ***********************************/
void * CommonSerialPort::ReadThreadFunction(void *arg)
 {
    printf("------------start ReadThread--------------\n");

    CommonSerialPort *com = (CommonSerialPort*) arg;
    //epoll设置
   // int ft = com->fd;
    com->event.data.fd = com->fd;

    com->event.events = EPOLLET | EPOLLIN;
    if (epoll_ctl(com->epid, EPOLL_CTL_ADD, com->fd, &com->event) != 0) //将读事件添加到epoll的事件队列中
    {
        printf("set epoll error!\n");
        return NULL;
    }
    printf("------------set epoll ok!-----------------\n");

     //下面开始epoll等待
     int i = 0, waiteNum = 0;
     while (true)
     {

         waiteNum = epoll_wait(com->epid, com->events, 6,-1);
       // printf("waiteNUm:%d\n",waiteNum);
        for (i = 0; i < waiteNum; i++)
        {
            // printf("eppol Thread_in\n");
            if (com->events[i].events & EPOLLIN) //判断是否有数据进入
            {         
               com->ComRead(MAXLEN);//接受数据

            }

        }

      }
     return NULL;
}

void ReadDataProc(char*dataBuff,const int BuffLen)
{
//        CommonSerialPort *com_recv = (CommonSerialPort*) arg;
//        int len=0;
//        while(true)
//        {
//             pthread_mutex_lock(&testlock);
//             //printf("Pth_thr_con\n");
//            if((len=com_recv->list_rev.size())>0)
//            {
//              // gpe_as.gps_analyse(com_recv->list_rev.front(),&temp);

//               com_recv->list_rev.pop_front();
//               printf("%d\n",len);
//              /*if(ptmp != NULL)
//                {
//                    ptmp = strcat(ptmp,com_recv->list_rev.front());
//                }
//                else
//                {
//                   ptmp = com_recv->list_rev.front();
//                }
//                ptr = strtok(ptmp,"\r\n");
//                while(ptr!=NULL)
//                {
//                    gpe_as.gps_analyse(ptr,&temp);
//                    com_recv->list_rev.pop_front();
//                    ptmp = ptr;
//                    ptr = strtok(NULL,"\r\n");

//               }*/

//            }
//            pthread_mutex_unlock(&testlock);
//            usleep(30000);
//        }


    //printf("I am ReadDataProc!\n");
    //printf("RecvData is %s\n", DataBuff);
}

/************************************
 功能：Com口读数据

 参数：char * ReadBuff 存储读出的数据，const int ReadLen 读出的长度

 返回值：实际读取长度，-1表示读取失败
 ***********************************/
int CommonSerialPort::ComRead( const int ReadLen)
{
     if (fd < 0) //判断操作符是否打开
     {
         printf("Com error!\n");
         return -1;
     }
     int len = 0;
     int rdlen = 0;
     char* buff_recv = new char[ReadLen];
     memset(buff_recv,0,ReadLen);
     while (true)
     {
         rdlen = read(fd, buff_recv+len, SERIALSIZE);
       //   printf("%s\n",buff_recv);
        //LOG_INFO("Red_LocalFunction1::%s\n,Packet:_len1%d\n",buff_recv,rdlen);
        if(rdlen<0)
        {
          if(len>0)

                 //    printf("send Signaln%s\n",buff_recv);
                emit SendToGpsAnlyse(buff_recv,len);
            return -1;
        }
         len += rdlen;
        if(SERIALSIZE>(ReadLen-len))
         {
           //   printf("Red_LocalFunction::%s\n,Packet:_len%d\n",buff_recv,len);
            //  LOG_INFO("Red_LocalFunction::%s;\nPacket:_len%d\n",buff_recv,len);
            //  printf("send Signaln%s\n",buff_recv);
             emit SendToGpsAnlyse(buff_recv,len);
            return len;
         }
     }
     return len;

}

/************************************
 功能：Com口读数据

 参数：char * WriteBuff 写入的数据，const int ReadLen 写入的长度

 返回值：写入长度,-1表示写入失败
 ***********************************/
int CommonSerialPort::ComWrite(char *WriteBuff, const int WriteLen)
{
     if (fd < 0)//判断操作符是否打开
     {
        printf("Com error!\n");
        return -1;
     }
     int wlen = write(fd, WriteBuff, WriteLen);
     return wlen;
}

/************************************
 功能：Com关闭

 参数：无

 返回值：无
 ***********************************/
void CommonSerialPort::ComClose()
{
     if (this->fd > 0)//判断文件描述符是否存在
    {
      close(this->fd);
    }
}

/************************************
 功能：析构函数

 参数：无

 返回值：无
 ***********************************/
CommonSerialPort::~CommonSerialPort()
{
    ComClose();
    pthread_mutex_destroy(&testlock);
}

