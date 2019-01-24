#include "recvtongjiudp.h"

RecvTongjiUdp::RecvTongjiUdp(QObject *parent) :
    QObject(parent)
{

}
RecvTongjiUdp::~RecvTongjiUdp()
{
    if(udp_send_timer){
        if(udp_send_timer->isActive()){
            udp_send_timer->stop();
        }
        delete udp_send_timer;
        udp_send_timer = NULL;
    }
    if(TongjiToAndriod_msgqueue){
        if(!TongjiToAndriod_msgqueue->isEmpty()){
            TongjiToAndriod_msgqueue->clear();
        }
        delete TongjiToAndriod_msgqueue;
        TongjiToAndriod_msgqueue = NULL;
    }
}

void RecvTongjiUdp::Init()
{
      udpSendProcessThread = new QThread();
      this->moveToThread(udpSendProcessThread);
      udpSendProcessThread->start();
    udp_send_timer      = new QTimer(this);
    timeout_count     = 0;
    //  memset(&obu_can_info_store, 0, sizeof(obu_can_info));
    connect((QObject *)udp_send_timer, SIGNAL(timeout()), this, SLOT(get_process_msg()));
    connect((QObject*)this,SIGNAL(SendToUdpAndriod(char*,int)),udpCom_Andriod,SLOT(udp_dataSend_slot(char*,int)));
    udp_send_timer->start(PERIOD_10MS);
}

void RecvTongjiUdp::process_recv_msg()
{
        RecvMSgPacket * msgTemp = NULL;
        if(!TongjiToAndriod_msgqueue->isEmpty()){
            msgTemp = TongjiToAndriod_msgqueue->dequeue();
        }
        if(msgTemp){
                do_process_recv_msg(msgTemp);

        }
}
void RecvTongjiUdp::do_process_recv_msg(RecvMSgPacket *p_TongjiToAndriod_msg)
{
       if(p_TongjiToAndriod_msg->msgTyp ==1)
       {
           emit SendToUdpAndriod(p_TongjiToAndriod_msg->p_mcu_msg,p_TongjiToAndriod_msg->len);
       }
       if(p_TongjiToAndriod_msg->msgTyp==2)
       {
           p_TongjiToAndriod_msg->p_mcu_msg[0];
           emit SendToCan(p_TongjiToAndriod_msg->p_mcu_msg[0]);
           delete p_TongjiToAndriod_msg;
           p_TongjiToAndriod_msg =NULL;
       }
}

void RecvTongjiUdp::getstimeval()
{
        char buf[28];
        struct timeval tv;
        struct tm      tm;
        size_t         len = 28;
        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, &tm);
        strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tm);
        len = strlen(buf);
        //sprintf(buf + len, ".%06.6d", (int)(tv.tv_usec));
       // printf("%s\n",buf);
        //LOG_INFO("Function_%s,lineNum_%d,%s\n" ,,422,CarSelf_data.c_str());
}

void RecvTongjiUdp::RecvMsg(char *buf,int len )
{
        //tongjiToAndriod_msgqueue->DeCodePacket(p_TongjiToAndriod_msg);
        TongjiToAndriod_msgqueue->enqueue(p_TongjiToAndriod_msg);
}



void RecvTongjiUdp::get_process_msg()
{
        process_recv_msg();
}
