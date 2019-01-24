#include "commonltev2x.h"

using namespace std;
extern "C"
{
    #include "ltev2x.h"
}

static LTEV2X_HANDLE g_ltev2x_hdl;

CommonLtev2x::CommonLtev2x(UdpComManager*udpCom ,UdpComManager* udpTongji,inner_recv_manager* inner_recv,RtkPacketControl *rtkPacket)
{
        if(v2xInit()==-1)
        {
            exit(0);
        }
        ltevMsg_revThread = new QThread();
        ltev_msg_recv  =   new LtevMsgRecv();
        ltev_msg_recv->moveToThread(ltevMsg_revThread);
       // printf("ltecMsgRecv class \n");
        ltevMsg_sendThread = new QThread();
        ltev_msg_send  =   new LtevMsgSend();
        ltev_msg_send->moveToThread(ltevMsg_sendThread);
        // printf("LtevMsgSend class \n");
        ltevCan_SendThread = new QThread();
        ltevCan_letv = new CommonLtev2x_List();
        ltevCan_letv->moveToThread(ltevCan_SendThread);

        connect(this, SIGNAL(startRecv()), ltev_msg_recv, SLOT(Recv_LtevMsg()));
        connect(ltev_msg_recv, SIGNAL(LtevPacket_ListInsert(char*)), ltevCan_letv, SLOT(RecvLtev_ListInsert(char*)));


        connect(ltev_msg_recv,SIGNAL(Ltev_SendtoUdp(char*,int)),udpCom, SLOT(udp_dataSend_slot(char*,int)));
        connect(ltevCan_letv,SIGNAL(ObuRecvSendToTngji(char*,int)),udpTongji, SLOT(udp_dataSend_slot(char*,int)));
        connect(ltevCan_letv,SIGNAL(LtevLightMsg_SendToCan(can_msg*)),inner_recv, SLOT(accept_inner_msg(can_msg*)));
        connect(ltevCan_letv,SIGNAL(InsertCarRtkMsg_Front(char*)),rtkPacket,SLOT(RecvListInsert_Front(char*)));

        ltevMsg_revThread->start();
        ltevMsg_sendThread->start();
        ltevCan_SendThread->start();
        emit startRecv();
}

CommonLtev2x::~CommonLtev2x()
{
    //ltev_msg_recv->thread_state = false;
    if(ltevMsg_revThread)
    {
       if (ltevMsg_revThread->isRunning())
       {
            ltevMsg_revThread->quit();
            ltevMsg_revThread->wait();
        }
        delete ltevMsg_revThread;
        ltevMsg_revThread = NULL;
    }
    ltev_msg_recv->deleteLater();
    if(ltev_msg_recv)
    {
        delete ltev_msg_recv;
        ltev_msg_recv = NULL;
    }
    if(ltevMsg_sendThread)
    {
        if(ltevMsg_sendThread->isRunning())
        {
            ltevMsg_sendThread->quit();
            ltevMsg_revThread->wait();
            delete ltevMsg_sendThread;
            ltevMsg_sendThread = NULL;
        }
    }
    ltev_msg_send->deleteLater();
    if(ltev_msg_send)
    {
        delete ltev_msg_send;
        ltev_msg_send = NULL;
    }
    if(ltevCan_SendThread)
    {
        if(ltevCan_SendThread->isRunning())
        {
            ltevMsg_sendThread->quit();;
            ltevCan_SendThread->wait();
            delete ltevCan_SendThread;
            ltevCan_SendThread = NULL;
        }
    }
    ltevCan_letv->deleteLater();
    if(ltevCan_letv)
    {
        delete ltevCan_letv;
        ltevCan_letv = NULL;
    }
}

int CommonLtev2x::v2xInit()
{
    LTEV2X_Option options;

   string strSystemConfigFile = "/root/Config/Config.ini";

    dtconfig      = new DTConfig(strSystemConfigFile, false);
    string v2x_str;
    g_ltev2x_hdl = LTEV2X_Open();
    if(-1 == g_ltev2x_hdl)
    {
        printf("open ltev2x error!\n");
        return -1;
    }
    else
    {
        printf("OPen Ltev2x Sucess\n");
    }
    //config ltev2x
    dtconfig->GetValue("ue_id",v2x_str);
    options.ue_id = atoi(v2x_str.c_str());
    dtconfig->GetValue("ps_id",v2x_str);
    options.ps_id = atoi(v2x_str.c_str());
    dtconfig->GetValue("tx_power",v2x_str);
    options.tx_power = atoi(v2x_str.c_str());
    dtconfig->GetValue("priority",v2x_str);
    options.priority = atoi(v2x_str.c_str());
    dtconfig->GetValue("period",v2x_str);
    options.period = atoi(v2x_str.c_str());
    dtconfig->GetValue("proc_id",v2x_str);
    options.proc_id = atoi(v2x_str.c_str());
    dtconfig->GetValue("data_len",v2x_str);
    options.data_len = atoi(v2x_str.c_str());
    dtconfig->GetValue("test_mode",v2x_str);
    options.test_mode = atoi(v2x_str.c_str());
    dtconfig->GetValue("test_sfn",v2x_str);
    options.test_sfn = atoi(v2x_str.c_str());
    if (0 != LTEV2X_SetOption(g_ltev2x_hdl, &options))
    {
        printf("set ltev2x error!\n");
        return -1;
    }
    else
    {
        printf("Obu Init Success\n");
        return 0;
    }
}

LtevMsgSend::LtevMsgSend()
{


}

LtevMsgSend::~LtevMsgSend()
{


}

void LtevMsgSend::Ltev_sendMsg(char * buff,int packet_len)
{

    int len_send = LTEV2X_Send(g_ltev2x_hdl,buff,packet_len);
    if(len_send >0)
        printf("Send to OBu Success %s\n",buff);
    else
        printf("Send to OBu Success%d\n", len_send);
    delete buff;
    buff = NULL;
}


LtevMsgRecv::LtevMsgRecv()
{

     thread_state = true;
}


LtevMsgRecv::~LtevMsgRecv()
{
    thread_state = false;

}
void LtevMsgRecv::getstimeval()
{
    char buf[28];
    struct timeval tv;
    struct tm      tm;
    size_t         len = 28;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);
    strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tm);
    len = strlen(buf);
    sprintf(buf + len, ".%06.6d", (int)(tv.tv_usec));
    printf("%s\n",buf);
    //LOG_INFO("Function_%s,lineNum_%d,%s\n" ,,422,CarSelf_data.c_str());
}


void LtevMsgRecv::Recv_LtevMsg()
{
   int power =1;
   while(thread_state)
   {
        char*  buf= new char[LTE_RECV_MSG_LEN];
        memset(buf, 0, LTE_RECV_MSG_LEN);
        int ret =  LTEV2X_Recv(g_ltev2x_hdl, buf, LTE_RECV_MSG_LEN, &power);
        if(ret > 1)
        {
            printf("OBU_recv_success::%d\n",ret);
            char* recvbuf = new char[LTE_RECV_MSG_LEN];
            memset(recvbuf, 0, LTE_RECV_MSG_LEN);
            memcpy(recvbuf, buf, ret);
            printf("RecvLtev_ListInsert%s\n",recvbuf);
            emit LtevPacket_ListInsert(recvbuf);
            emit Ltev_SendtoUdp(buf, ret);
        }
   }
}



CommonLtev2x_List::CommonLtev2x_List()
{

    letv_RecvList = new Letv_RecvList;
   // letv_RecvList->clear();
    Recv_ltev_timer = new QTimer(this);
    tongji = new TongjiXieyiPrcase();
 //   printf("set Recv_ltev_timer  success\n");
    connect((QObject *)Recv_ltev_timer, SIGNAL(timeout()), this, SLOT(Ltev_PackControlThread()));
    printf("set connect Recv_ltev_timer  success\n");
    Recv_ltev_timer->start(Recv_ltev_Period);

}


CommonLtev2x_List::~CommonLtev2x_List()
{
    if(Recv_ltev_timer){
        if(Recv_ltev_timer->isActive()){
            Recv_ltev_timer->stop();
        }
        delete Recv_ltev_timer;
        Recv_ltev_timer = NULL;
    }
    if(letv_RecvList){
        if(!letv_RecvList->empty()){
            letv_RecvList->clear();
        }
        delete letv_RecvList;
        letv_RecvList =  NULL;
    }
    delete tongji;
    tongji =NULL;
}


void  CommonLtev2x_List::RecvLtev_ListInsert(char *buf)
{
    letv_RecvList->enqueue(buf);

}


void CommonLtev2x_List::Ltev_PackControlThread()
{

    if(!letv_RecvList->empty())
    {
        char * temp =NULL;
        temp =letv_RecvList->dequeue();
        if(temp)
        {
             pack_analyse(temp);
                  //  letv_RecvList->pop_front();
        }
    }



    }

void  CommonLtev2x_List::pack_analyse(char *buf)
{
   printf("ltev_recv_lightpack_analyse\n");
    Json::Reader reader;
    Json::Value  p_packet;

    string Msg_type;
    Msg_type.clear();
    if(reader.parse(buf, p_packet))
    {
        Msg_type = p_packet["type"].asString();
        if(Msg_type.compare("1") == 0)
        {
            printf("ltev_recv_lightPacket\n");
            can_msg*  ltev_canmsg = new can_msg;
            memset(ltev_canmsg, 0, sizeof(can_msg));
            memcpy(ltev_canmsg->msg, buf, MAX_CAN_MSG_LENGTH);
            emit LtevLightMsg_SendToCan(ltev_canmsg);
        }
        else if(Msg_type.compare("0") == 0)
        {
            printf("ltev_recv_RTK_Packet_send\n");
            char* msg_rtk = new char[LTE_RECV_MSG_LEN];
            memset(msg_rtk, 0, LTE_RECV_MSG_LEN);
            memcpy(msg_rtk, buf, LTE_RECV_MSG_LEN);
            emit InsertCarRtkMsg_Front(msg_rtk);

            string car_type = p_packet ["data"]["isSelf"].asString();
            if(car_type.compare("0")==0)
            {
                string str_data = p_packet ["data"]["extra"].asString();
                int len = str_data.length();
                if(len>0)
                {
                     char  sendToTongji[len+2]={0};
                   //  memset(sendToTongji,0,len+2);
                     memcpy(sendToTongji,str_data.c_str(),len);
                     char *data  =NULL;
                     data=  tongji->InCodePacket(sendToTongji,&len,Car_IsNoSelf);
                     if(len>0&&data!=NULL)
                     {
                         printf("Send_toTongji%s\n",data);
                         emit ObuRecvSendToTngji(data,len);
                     }

                }
            }
        }
        else if(Msg_type.compare("6") == 0){

            string str_obstacle_data = p_packet["data"]["control"].asString();
            char   obstacle_data[OBSTACLE_DATA_LEN];
            parse_obstacle_string((char *)str_obstacle_data.data(), obstacle_data);
            char *data  = NULL;
            int length = OBSTACLE_DATA_LEN;
            data = tongji->InCodePacket(obstacle_data, &length, 5);
            if(data != NULL)
            {
                emit ObuRecvSendToTngji(data, OBSTACLE_DATA_LEN + FIXED_LENGTH);
            }
            qDebug("CommonLtev2x_List::pack_analyse: receive a RSU-Obstacle packet...");

        }
        else{
            qDebug("CommonLtev2x_List::pack_analyse: this lte-v packet has no valid type...");
        }
    }
    delete buf;
    buf =NULL;

}


void CommonLtev2x_List::parse_obstacle_string(char *source, char *dest)
{
    Obstacle_stru obstacle;
    char *buf = source;
    obstacle.I2V_Crosscol = buf[0];
    obstacle.I2V_Target_ID = (unsigned char)get_int_number(&buf[GetComma(1, buf)]);
    obstacle.I2V_Target_Location = buf[GetComma(2, buf)];
    obstacle.I2V_Target_Type = buf[GetComma(3, buf)];
    obstacle.I2V_Target_Range_X = (short)get_int_number(&buf[GetComma(4, buf)]);
    obstacle.I2V_Target_Range_Y = (unsigned short)get_int_number(&buf[GetComma(5, buf)]);
    obstacle.I2V_Target_Range_velocity_X = (short)get_int_number(&buf[GetComma(6, buf)]);
    obstacle.I2V_Target_Range_velocity_Y = (short)get_int_number(&buf[GetComma(7, buf)]);
    obstacle.I2V_Target_longitude = get_double_number(&buf[GetComma(8, buf)]);
    obstacle.I2V_Target_latitude = get_double_number(&buf[GetComma(9, buf)]);
    obstacle.I2V_Target_altitude = get_double_number(&buf[GetComma(10, buf)]);
    obstacle.UTC_Time = get_double_number(&buf[GetComma(11, buf)]);
    memcpy(dest,&obstacle,44);
}


int CommonLtev2x_List::get_int_number(char *s)
{
    char buf[128] = {0};
    int i;
    int rev;
    i=GetComma(1,s);
    strncpy(buf,s,i);
    buf[i]=0;
    rev=atoi(buf);
    return rev;
}

double CommonLtev2x_List::get_double_number(char *s)
{
    char buf[128] = {0};
    int i;
    double rev;
    i=GetComma(1,s);
    strncpy(buf,s,i);
    buf[i]=0;
    rev=atof(buf);
    return rev;
}

int CommonLtev2x_List::GetComma(int num,char *str)
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

