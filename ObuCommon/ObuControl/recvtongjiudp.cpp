#include "recvtongjiudp.h"

RecvTongjiUdp::RecvTongjiUdp(QObject *parent) :
    QObject(parent)
{
    tongj = new TongjiXieyiPrcase();
    Recv_Tongji_msgqueue = new QQueue<RecvMSgPacket *>;
    Recv_Tongji_msgqueue->clear();
    udp_send_timer      = new QTimer(this);
     connect((QObject *)udp_send_timer, SIGNAL(timeout()), this, SLOT(get_process_msg()));
     udp_send_timer->start(TJ_PERIOD_10MS);
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
    if(Recv_Tongji_msgqueue){
        if(!Recv_Tongji_msgqueue->isEmpty()){
            Recv_Tongji_msgqueue->clear();
        }
        delete Recv_Tongji_msgqueue;
        Recv_Tongji_msgqueue = NULL;
    }
}

void RecvTongjiUdp::Init()
{

  //  timeout_count     = 0;
    //  memset(&obu_can_info_store, 0, sizeof(obu_can_info));

    connect((QObject*)this,SIGNAL(SendToUdpAndriod(char*,int)),udpCom_Andriod,SLOT(udp_dataSend_slot(char*,int)));
    connect((QObject*)udpCom_Tongji,SIGNAL(SendMsgTongjiMsgPacket(char*,int)),this,SLOT(RecvMsg(char*,int)));
     connect((QObject*)this,SIGNAL(SendToCan(can_msg*)),recv_manger,SLOT(accept_inner_msg(can_msg*)));


 }

void  RecvTongjiUdp::ConstructJsonString_ToAndriod(RecvMSgPacket* buf)
{
        int len =0;
        Json::Value packet;
        Json::Value packet_data;
        packet["id"]  = Json::valueToString(condition);
        packet["type"] = Json::valueToString(VoiceGuid_INFO);
        packet_data["UUID"] = Json::valueToString(CardID);
        packet_data["isSelf"] = Json::valueToString(Car_IsSelf);

        packet_data["voiceguid"] = Json::Value(buf->p_mcu_msg[0]);
        if(buf->p_mcu_msg[0]==0x02)
        {

            packet_data["distance"] = Json::Value(buf->p_mcu_msg[1]);
        }
        packet["data"] = packet_data;
        Json::StyledWriter strwriter;
        string str_packet = strwriter.write(packet);
        len = str_packet.length();
        char*  buf_data = new char[str_packet.length()];
        memset(buf_data,0,len);
        memcpy(buf_data, str_packet.data(), str_packet.length());

        char *p = buf->p_mcu_msg;
        delete p;
        p=NULL;
         buf->p_mcu_msg  =NULL;
        buf->p_mcu_msg = buf_data;
        buf->len = str_packet.length();
}

void  RecvTongjiUdp::ConstructJsonString_ToCan(RecvMSgPacket* buf)
{
    int len;
    Json::Value packet;
    Json::Value packet_data;
    if(condition >=10000)
        condition =0;
    condition++;
    packet["id"]  = Json::valueToString(condition);
    packet["type"] = Json::valueToString(Car_ObuControl_Info);
    packet_data["UUID"] = Json::valueToString(CardID);
    packet_data["isSelf"] = Json::valueToString(Car_IsSelf);
   // packet["type"] = Json::Value("0");
   // packet_data["isSelf"] = Json::Value("1");
    packet_data["wc_brake_per"] = Json::valueToString(buf->p_mcu_msg[0]);
    printf("Recv_Tongji shache Value %0x/n",buf->p_mcu_msg[0]);
    packet_data["signal_5g"] =Json::valueToString(0xFF);
    packet["data"] = packet_data;
    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    //return str_packet;
    len = str_packet.length();
    char*  buf_data = new char[str_packet.length()];

    memset(buf_data,0,len);
    memcpy(buf_data, str_packet.data(), str_packet.length());
    char *p = buf->p_mcu_msg;
    delete p;
    p=NULL;
     buf->p_mcu_msg  =NULL;
    buf->p_mcu_msg = buf_data;
    buf->len = str_packet.length();
}

void RecvTongjiUdp::process_recv_msg()
{
        RecvMSgPacket * msgTemp = NULL;
        if(!Recv_Tongji_msgqueue->isEmpty()){
            msgTemp = Recv_Tongji_msgqueue->dequeue();
        }
        if(msgTemp){
                do_process_recv_msg(msgTemp);

        }
}

void RecvTongjiUdp::do_process_recv_msg(RecvMSgPacket *recv_Tong_msg)
{

       tongj->DeCodePacket(recv_Tong_msg);
//  printf("RecvTongjiUdp::%s\n",recv_Tong_msg->p_mcu_msg);
        char * data_send =NULL;
       if(recv_Tong_msg->type ==1)
       {

           ConstructJsonString_ToAndriod(recv_Tong_msg);
           data_send = recv_Tong_msg->p_mcu_msg;
           printf("Recv_VoiceMsgPacket %s\n",data_send);
           int len = recv_Tong_msg->len;
           emit SendToUdpAndriod(data_send,len);
           recv_Tong_msg ->p_mcu_msg=NULL;
           delete recv_Tong_msg;
           recv_Tong_msg =NULL;
           return;
       }
       if(recv_Tong_msg->type ==2)
       {

           ConstructJsonString_ToCan(recv_Tong_msg);
           printf("Jason_Packet:%s\n",recv_Tong_msg->p_mcu_msg);
           can_msg  * can_packet = new can_msg;
           memset(can_packet,0,sizeof(can_packet->msg));
            if((recv_Tong_msg->len) > sizeof(can_packet->msg))
            {
               LOG_ERROR("Error_TongjiPacket_youmenkongzhi:%s\n",recv_Tong_msg->p_mcu_msg);
               delete recv_Tong_msg ->p_mcu_msg;
               recv_Tong_msg ->p_mcu_msg=NULL;
               delete recv_Tong_msg;
               recv_Tong_msg =NULL;
               return ;
            }
            else
            {
                memcpy(can_packet->msg,recv_Tong_msg->p_mcu_msg,recv_Tong_msg->len);
                //   data_send = p_TongjiToAndriod_msg->p_mcu_msg;
                printf("Send_ToCan:%s\n",can_packet->msg);
                emit SendToCan(can_packet);
                recv_Tong_msg ->p_mcu_msg=NULL;
                delete recv_Tong_msg;
                recv_Tong_msg =NULL;
            }
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

        RecvMSgPacket*  p_TongjiToAndriod_msg = new RecvMSgPacket;

        p_TongjiToAndriod_msg->len = len;
        p_TongjiToAndriod_msg->p_mcu_msg =buf;
        printf("RecvMsg:%d\n",p_TongjiToAndriod_msg->len);

        Recv_Tongji_msgqueue->enqueue(p_TongjiToAndriod_msg);
}

void RecvTongjiUdp::get_process_msg()
{
        process_recv_msg();
}

