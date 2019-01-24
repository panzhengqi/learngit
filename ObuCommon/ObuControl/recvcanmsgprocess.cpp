#include "recvcanmsgprocess.h"

RecvCanMsgProcess::RecvCanMsgProcess(QObject *parent) :
    QObject(parent)
{
        tongj = new TongjiXieyiPrcase();
        CanToTongji_msgqueue = new QQueue<Obu_algorithm_paras *> ;
        CanToTongji_msgqueue->clear();
        recv_can_timer      = new QTimer(this);
        timeout_count     = 0;
        connect((QObject *)recv_can_timer, SIGNAL(timeout()), this, SLOT(get_process_msg()));
         recv_can_timer->start(PERIOD_10MS);
}

RecvCanMsgProcess::~RecvCanMsgProcess()
{
    if(recv_can_timer){
        if(recv_can_timer->isActive()){
            recv_can_timer->stop();
        }
        delete recv_can_timer;
        recv_can_timer = NULL;
    }
    if(CanToTongji_msgqueue){
        if(!CanToTongji_msgqueue->isEmpty()){
            CanToTongji_msgqueue->clear();
        }
        delete CanToTongji_msgqueue;
        CanToTongji_msgqueue = NULL;
    }
}

void RecvCanMsgProcess::Init()
{

  //  memset(&obu_can_info_store, 0, sizeof(obu_can_info));

    connect((QObject*)this,SIGNAL(SendToUdpTongji(char*,int)),udpCom_Tongji,SLOT(udp_dataSend_slot(char*,int)));

     connect((QObject*)can_recv_man,SIGNAL( send_algorithm_paras(Obu_algorithm_paras *)),this,SLOT(RecvMsg(Obu_algorithm_paras*)));

}

void RecvCanMsgProcess::process_recv_msg()
{
    Obu_algorithm_paras * msgTemp = NULL;
    //printf("process_recv_msg()\n");
    if(!CanToTongji_msgqueue->isEmpty()){
   //      printf("process_recv_msg_dequeue\n");
        msgTemp = CanToTongji_msgqueue->dequeue();
    }
    // printf("msgTemp\n");
    if(msgTemp){
           // printf("msgTemp\n");
            do_process_recv_msg(msgTemp);
    }
}

void RecvCanMsgProcess::do_process_recv_msg(Obu_algorithm_paras *p_TongjiToAndriod_msg)
{
 //  printf("RecvCanMsgProcess::do_process_recv_msg(Obu_algorithm_paras *p_TongjiToAndriod_msg)\n");

    if(p_TongjiToAndriod_msg->type ==OBU_ALGORITHM_PARA_CONTROL)
       {
           datalen = sizeof(Obu_algorithm_para_control);

         //  temp.Current_speed = p_TongjiToAndriod_msg->p_control_para.Current_speed;
           //memcpy(temp)
           char buf[datalen];
           memset(buf,0,datalen);
      //     printf("RecvCanMsgProcess::do_process_recv_msg,Memcpy_in\n");
           memcpy(buf,&p_TongjiToAndriod_msg->p_control_para,datalen);
       //     printf("RecvCanMsgProcess::do_process_recv_msg,Memcpy_Out\n");
//            printf("RecvCanMsgProcess::%s\n,%d\n",buf,datalen);
           char* para_control=tongj->InCodePacket(buf,&datalen,3);
           if(para_control!=NULL)
           {
               emit SendToUdpTongji(para_control,datalen);
           }
       }
       if(p_TongjiToAndriod_msg->type ==OBU_ALGORITHM_PARA_OBSTACLE)
       {
           datalen = sizeof(Obu_algorithm_para_obstacle);
           char buf[datalen];
           memset(buf,0,datalen);
//           printf("RecvCanMsgProcess::do_process_recv_msg,Memcpy_in2\n");
//           printf("RecvCanMsgProcess::%d\n", datalen);

//           buf[0] = p_TongjiToAndriod_msg->p_obstacle_para.Obstacle_type;
//           buf[1] = p_TongjiToAndriod_msg->p_obstacle_para.Obstacle_distance;
//           buf[2] = p_TongjiToAndriod_msg->p_obstacle_para.Obstacle_speed;
//           buf[3] = p_TongjiToAndriod_msg->p_obstacle_para.
           memcpy(buf,&p_TongjiToAndriod_msg->p_obstacle_para,datalen);
//           for(int i = 0; i < datalen; i++){
//               printf("%X\n", buf[i]);
//           }

//            printf("RecvCanMsgProcess::%s\n",buf);
          // memcpy(buf,p_TongjiToAndriod_msg,sizeof(Obu_algorithm_para_obstacle));
//          printf("RecvCanMsgProcess::do_process_recv_msg,Memcpy_in2\n");
           char * para_obstacle=tongj->InCodePacket(buf,&datalen,4);
           if(para_obstacle!=NULL)
           {
                   emit SendToUdpTongji(para_obstacle,datalen);
           }
       }
}

void RecvCanMsgProcess::getstimeval()
{
        char buf[28];
        struct timeval tv;
        struct tm      tm;
        size_t         len = 28;
        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &tm);
        strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tm);
        len = strlen(buf);

}

void RecvCanMsgProcess::RecvMsg(Obu_algorithm_paras *p_TongjiToAndriod_msg )
{
        CanToTongji_msgqueue->enqueue(p_TongjiToAndriod_msg);
}

void RecvCanMsgProcess::get_process_msg()
{
        process_recv_msg();
}
