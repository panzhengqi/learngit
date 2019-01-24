#include "GpsAnalyse.h"


using namespace std;
//string str;
int condition =0 ;
typedef struct Obu_algorithm_paras_Control{

    unsigned char  para_type;
    unsigned char  Current_speed;
     unsigned char            WC_brake_feedback;
    unsigned char          Real_throttle_fb;
}Obu_algorithm_paras_Control;


typedef struct Obu_algorithm_paras_Obstance{

    unsigned char  para_type;
    unsigned char  Obstacle_type;
    unsigned char  Obstacle_distance;
    unsigned char  Obstacle_speed;
    unsigned  short          Obstacle_TTC;
}Obu_algorithm_paras_Obstance;

GpsAnalyse::GpsAnalyse( UdpComManager* udpcom,UdpComManager* udp_Tongji,inner_recv_manager* recv_manger,LtevMsgSend*com_ltev)
{  

    //rtkpackcontrol = new RtkPacketControl();
  //  com_ltev  = new LtevMsgSend();
    timer_recv = new QTimer(this);
    time_flash = new QTimer(this);
    rtk_msg_queue      = new QQueue<char *>;
    rtk_msg_queue->clear();
    connect((QObject *)timer_recv, SIGNAL(timeout()), this, SLOT(get_process_msg()));
    connect((QObject *)time_flash, SIGNAL(timeout()), this, SLOT(system_time_flash()));
    connect(this,SIGNAL(SendToLtev(char*,int)),com_ltev, SLOT(Ltev_sendMsg(char*,int)));
    connect(this,SIGNAL(Gps_SendToUdp(char*,int)),udpcom, SLOT(udp_dataSend_slot(char*,int)));
    connect(this,SIGNAL(SendToTongji(char*,int)),udp_Tongji, SLOT(udp_dataSend_slot(char*,int)));
    connect(this,SIGNAL(SendToCan(can_msg*)),recv_manger, SLOT(accept_inner_msg(can_msg*)));
   // connect(udpcom,SIGNAL(SendMsgTongjiMsgPacket(char*,int)),this, SLOT(accept_net_msg(char*,int)));
    memset(buff_temp,0,Rtk_MAX_MSG_LENGTH);
    tongji = new TongjiXieyiPrcase();
  //  time = new QTimer(this);
 //  connect((QObject *)time, SIGNAL(timeout()), this, SLOT(SendCanMessage()));
   // time->start(100);
    time_Control = false;
    timer_recv->start(Timer_Period);
    time_flash->start(1000*3600);

}

GpsAnalyse::~GpsAnalyse()
{
    if(timer_recv){
        if(timer_recv->isActive()){
            timer_recv->stop();
        }
        delete timer_recv;
        timer_recv = NULL;
    }
    if(time_flash){
        if(time_flash->isActive()){
            time_flash->stop();
        }
        delete time_flash;
        time_flash = NULL;
    }
}

void GpsAnalyse::Init()
{

    connect(this,SIGNAL(InsertToRtkSelf(char*,int )),rtkpackcontrol,SLOT(RecvListInsert(char*,int)));
}

void GpsAnalyse::SendCanMessage()
{
        Obu_algorithm_paras_Control  obu_paras ;
        obu_paras.Current_speed = 13.45;
        obu_paras.Real_throttle_fb = 55.45;
        obu_paras.WC_brake_feedback =46.64;
       Obu_algorithm_paras_Obstance obu_obstance;
       //obu_obstance.para_type =3;
       obu_obstance.Obstacle_distance = 123;
       obu_obstance.Obstacle_speed = 124;
       obu_obstance.Obstacle_TTC =57;
       obu_obstance.Obstacle_type = 55;
        char  buff_can[1024]={0};
         char  buff_can1[1024]={0};
        memcpy(buff_can,&obu_paras,sizeof(Obu_algorithm_paras_Control));
       memcpy(buff_can1,&obu_paras,sizeof(Obu_algorithm_paras_Obstance));
        int len1 ,len2;
        char * temp =tongji->InCodePacket(buff_can,&len1,4);
         char * temp1 =tongji->InCodePacket(buff_can1,&len2,3);
        if(temp!=NULL)
       {
            SendToTongji(temp,len1);
          //  printf("Send_To_Tongji%s\n",temp);
        }
        if(temp1!=NULL)
       {
            SendToTongji(temp1,len2);
           // printf("Send_To_Tongji%s\n",temp1);
        }
}

void GpsAnalyse::gps_analyse(char *buff,int len_packet)
{
       char *pos_start = NULL;
       char *pos_end  = NULL;
       char*  tempBuff  =NULL;
       if(len_packet >0)
           tempBuff = buff;
      int len=0;
      if(strstr(buff_temp,"$"))
       {
           //LOG_INFO("yuansheng::%s",buff);
           pos_start = strstr(tempBuff,"\r\n");
           if(pos_start==NULL)
           {
               memcpy(buff_temp+strlen(buff_temp),buff,strlen(buff));
               return;
           }
         // len = strlen(buff_temp) +(strlen(tempBuff)-strlen(pos_start));
           char * pa = new char[Rtk_MAX_MSG_LENGTH];
           memset(pa,0,Rtk_MAX_MSG_LENGTH);
           memcpy(pa,buff_temp,strlen(buff_temp));           
           memcpy(pa+strlen(buff_temp),tempBuff,strlen(tempBuff)-strlen(pos_start));
           tempBuff =pos_start;
//           printf("pinjie:%s\n",pa);
           accept_net_msg(pa,strlen(pa));
           memset(buff_temp,0,Rtk_MAX_MSG_LENGTH);
           pos_start =NULL;

       }
       while(NULL!=(pos_start=strstr(tempBuff,"$KSXT")))
       {
          // LOG_INFO("yuansheng_St:%s\n",buff);
           // printf("yuansheng_St:%s\n",buff);
           pos_end=strstr(pos_start,"\r\n");
           if(pos_end ==NULL)
           {
               memset(buff_temp,0,Rtk_MAX_MSG_LENGTH);
               memcpy(buff_temp,pos_start,strlen(pos_start));

               printf("%s\n",pos_start);
                pos_start =NULL;
             // LOG_INFO("pinjie%s",buff_temp);
               break;
           }
           else
           {
               len = pos_end-pos_start;
                tempBuff = pos_start+len;
               char*  ltev_send = new char[1024];
               memset(ltev_send,0,1024);
               memcpy(ltev_send,pos_start,len);
//               printf("wanzheng:%s\n",ltev_send);
               accept_net_msg(ltev_send,len);
               pos_start =NULL;
               pos_end =NULL;
           }
       }
       delete buff;
       buff =NULL;
}

void  GpsAnalyse::ConstructJsonString(char* buf)
{
    Json::Value packet;
    Json::Value packet_data;
    packet["id"]  = Json::valueToString(condition);
    packet["type"] = Json::valueToString(Car_Type);
    packet_data["UUID"] = Json::valueToString(CardID);
    packet_data["isSelf"] = Json::valueToString(Car_IsNoSelf);
    packet_data["extra"] = Json::Value(buf);
    packet["data"] = packet_data;
    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    memset(buf,0,Rtk_MAX_MSG_LENGTH);
    memcpy(buf, str_packet.data(), str_packet.length());;
}

void  GpsAnalyse::ConstructJsonString_toUDP(char* buf)
{
    Json::Value packet;
    Json::Value packet_data;
    if(condition >=PACKID_Condition_MAX_VALUE)
        condition =0;
    packet["id"]  = Json::valueToString(condition);
    packet["type"] = Json::valueToString(Car_Type);
    packet_data["UUID"] = Json::valueToString(CardID);
    packet_data["isSelf"] = Json::valueToString(Car_IsSelf);
    packet_data["extra"] = Json::Value(buf);
    packet["data"] = packet_data;
    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    memset(buf,0,Rtk_MAX_MSG_LENGTH);
    memcpy(buf, str_packet.data(), str_packet.length());
     condition++;
}

void GpsAnalyse::process_recv_msg()
{
    char * temp = NULL;
    if(!rtk_msg_queue->isEmpty()){

        temp = rtk_msg_queue->dequeue();
    }
    if(temp){
        do_process_recv_msg(temp);
    }
//         char temp[256] ="$KSXT,20180814055947.60,116.33993283,39.98848375,56.6236,250.71,-70.80,329.70,0.238,,3,2,12,12,-859.825,-1278.662,-11.900,-0.120,0.205,0.080,,*26";
//         char*  ltev_send  = new char[1024];
//         memset(ltev_send,0,1024);
//         memcpy(ltev_send,temp,strlen(temp));
//        ConstructJsonString(ltev_send);
//        emit SendToLtev(ltev_send,strlen(ltev_send));

}
void GpsAnalyse::do_process_recv_msg(char *buff)
{
    int len  = strlen(buff);
    if(!time_Control)
    {
        time_str = buff;
        modify_os_time();
    }
     if(len<0)
         return;
    char*  ltev_send  =  new char[1024];
    char*  udp_send  =  new char[1024];
    char*  rtk_recv = new char[1024];
    can_msg*  canmsg =new can_msg;
    memset(ltev_send,0,1024);
    memset(udp_send,0,1024);
    memset(rtk_recv,0,1024);
    memset(canmsg->msg,0,1024);
    memcpy(ltev_send,buff,len);
     //printf("%s\n",ltev_send);
     LOG_INFO("WZ_Packet::%s\n",ltev_send);
    memcpy(udp_send,ltev_send,strlen(ltev_send));
    ConstructJsonString_toUDP(udp_send);
    ConstructJsonString(ltev_send);
    memcpy(rtk_recv,udp_send,strlen(udp_send));
    memcpy(canmsg->msg,udp_send,strlen(udp_send));
    emit SendToLtev(ltev_send,strlen(ltev_send));
    emit InsertToRtkSelf(rtk_recv,len);
    //          qDebug("LTE-V Send To RSU:");
    //           getstimeval();

    emit Gps_SendToUdp(udp_send,strlen(udp_send));
    emit SendToCan(canmsg);
    char *temp = NULL;
    int packlen=0;
    temp =  tongji->InCodePacket(buff,&packlen,Car_IsSelf);
    //       printf("Recv_rtk_packet%s\n",buff);
    if(packlen>0&&temp!=NULL)
    {
    //           printf("Send_toTongji%s\n",temp);
       emit SendToTongji(temp,packlen);
    }
    delete buff;
    buff =NULL;
}

void GpsAnalyse::accept_net_msg(char *buff,int len)
{
         //LOG_INFO("accept_net_msg:%s,accept_net_msg_Pack_len:%d\n",buff,len);
         rtk_msg_queue->enqueue(buff);
}

void GpsAnalyse::system_time_flash()
{    
    if(time_Control)
    {
        modify_os_time();
    }
}

void GpsAnalyse::get_process_msg()
{
     process_recv_msg();
}

bool GpsAnalyse::modify_os_time()
{

    if(time_str.length()>25&&time_str.contains("$KSXT"))
    {
        time_str = time_str.mid(6,14);

    }
   else
    {
        return false;
    }
    QDateTime  datatime;
    datatime= QDateTime::fromString(time_str,"yyyyMMddhhmmss");
    if(datatime.isValid())
  {
        datatime = datatime.addSecs(3600*8);
        QString str_temp = datatime.toString("MMddhhmmyy.ss");
        if(str_temp!=NULL)
         {
            int ret = QProcess::execute("date",QStringList() << str_temp);
            if(ret == -1 || ret == -2)
            {
                return false;
            }
            else
            {

                time_Control =true;
                return true;
            }
        }else
        {
            return false;
        }
        return true;
    }else
    {
        return false;
    }
}

int GpsAnalyse::GetComma(int num,char *str)
{
    int i,j=0;
    int len=strlen(str);
    for(i=0;i<len;i++)
    {
        if(str[i]==',')
        {
             j++;
        }
        if(j==num)
        {
            if(i+1 < len)
            {
                return i+1;
            }else
            {
                return 0;
            }

        }
    }
    return 0;
}

void GpsAnalyse::getstimeval()
{
    char buf[28] = {0};
    struct timeval tv;
    struct tm      tm;
    size_t         len = 28;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tm);
    len = strlen(buf);
    sprintf(buf + len, ".%06.6d", (int)(tv.tv_usec));
    printf("                   %s\n",buf);
    //LOG_INFO("Function_%s,lineNum_%d,%s\n" ,,422,CarSelf_data.c_str());
}
