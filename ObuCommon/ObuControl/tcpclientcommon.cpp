#include "tcpclientcommon.h"
#include <unistd.h>
#include "map"
#include <netinet/in.h>
#include <iostream>
#include "TcpServer.h"
#include "../CommonLog/log.h"
using namespace std;

const string msg_type_heartbeat_request        = "0x100001001";      //RSA->CNSS
const string msg_type_heartbeat_answer         = "0x100001002";         //CNSS->RSA
const string msg_type_lt_para_config_request   = "0x100002001";      //RSA->CNSS
const string msg_type_lt_para_config_answer    = "0x100002002";         //CNSS->RSA
const string msg_type_lt_report_config_request = "0x100003001";         //CNSS->RSA
const string msg_type_lt_report_config_answer  = "0x100003002";      //RSA->CNSS
const string msg_type_lt_state_report_request  = "0x100003003";      //RSA->CNSS
const string msg_type_lt_state_report_answer   = "0x100003004";         //CNSS->RSA
const string msg_type_lt_green_request         = "0x100004001";      //CNSS->RSA
const string msg_type_lt_green_answer          = "0x100004002";         //RSA->CNSS


//const string log_path = "/root/Config/Log/";
//class DTLog dtlog(log_path);
//#define LOG(x)  dtlog.LogRecord(x, FL);


const string rsu_id = "1";
const double longitude = 115.21;
const double latitude  = 30;
const int    result = 0;

TcpClientCommon::TcpClientCommon()
{

    p_tcpcommanager_thread = new QThread();
    p_TcpComManager        = new TcpComManager();
    p_TcpComManager->moveToThread(p_tcpcommanager_thread);

    process_recv_thread    = new QThread();
    p_process_recv_obj     = new TcpProcessRecv();
    p_process_recv_obj->moveToThread(process_recv_thread);
    process_recv_thread->start();
     p_tcpcommanager_thread->start();

     p_heartbeat_thread     = new QThread();
      p_heartbeats           = new HeartBeatThread();
      p_heartbeats->moveToThread(p_heartbeat_thread);



    QObject::connect((QObject *)p_process_recv_obj, SIGNAL(Send_TxBox_toMec_recv_msg(RecvMec_msg*)), TcpServer::pInstance->TcpServer_p_process_recv_obj,SLOT(accept_net_msg(RecvMec_msg*)));
    QObject::connect((QObject *)p_TcpComManager, SIGNAL(transmit_recv_msg(RecvMec_msg*)), p_process_recv_obj,SLOT(accept_net_msg(RecvMec_msg*)));

    QObject::connect((QObject *)p_TcpComManager, SIGNAL(control_heartbeat(int)), p_heartbeats, SLOT(process_heartbeats_event(int)));
    QObject::connect((QObject *)p_process_recv_obj, SIGNAL(clear_heartbeat(int)), p_heartbeats, SLOT(process_heartbeats_event(int)));


    QObject::connect((QObject *)p_heartbeats, SIGNAL(sig_rebuilt_tcp_socket()), p_TcpComManager, SLOT(rebuilt_tcp_sock()));
     QObject::connect((QObject *)p_heartbeats, SIGNAL(send_heartbeats_packet(RecvMec_msg*)), p_TcpComManager, SLOT(tcp_dataSend_slot(RecvMec_msg*)));

    if(p_process_recv_obj==NULL)
        printf("p_Process_recv_obj is NULL \n");
       //QObject::connect((QObject *)TcpServer::pInstance,SIGNAL(RecvMsgFromMcuSendtoMec(RecvMec_msg*)),p_process_recv_obj,SLOT(accept_net_msg(RecvMec_msg*)));
     QObject::connect((QObject *)TcpServer::pInstance,SIGNAL(RecvMsgFromMcuSendtoMec(RecvMec_msg*)),p_TcpComManager,SLOT(tcp_dataSend_slot(RecvMec_msg*)));
     //QObject::connect((QObject *)p_process_recv_obj, SIGNAL(Send_TxBox_toMec_recv_msg(RecvMcu_msg *)),p_TcpComManager, SLOT(tcp_dataSend_slot(RecvMcu_msg*)));

    // connect((QObject *)(p_udpCommon->p_process_recv_obj), SIGNAL(SendMsgToMec(light_control*)), p_light_manager, SLOT(accept_light_msg(light_control*)));
    p_heartbeat_thread->start();
}

TcpClientCommon::~TcpClientCommon()
{
    p_TcpComManager->deleteLater();
    if(p_heartbeats){
        delete p_heartbeats;
        p_heartbeats = NULL;
    }
    if(p_heartbeat_thread){
        p_heartbeat_thread->terminate();
        p_heartbeat_thread->wait();
        delete p_heartbeat_thread;
        p_heartbeat_thread = NULL;
    }
    if(p_process_recv_obj){
        delete p_process_recv_obj;
        p_process_recv_obj = NULL;
    }
    if(process_recv_thread){
        process_recv_thread->terminate();;
        process_recv_thread->wait();
        delete process_recv_thread;
        process_recv_thread = NULL;
    }
}

TcpComManager::TcpComManager()
{
    tcp                  = new QTcpSocket(this);
    tcp->setSocketOption(QAbstractSocket::LowDelayOption, "1");
    connect_config_timer  = new QTimer(this);
    set_connection_state(false);
    set_config_state(false);

   connect((QObject *)connect_config_timer, SIGNAL(timeout()), this, SLOT(try_connect_config()));
   connect((QObject *)tcp, SIGNAL(connected()), this, SLOT(tcpconnected()));
    connect((QObject *)tcp, SIGNAL(error(QAbstractSocket::SocketError)), this,
          SLOT(tcpconnecterror(QAbstractSocket::SocketError)));
    connect((QObject *)tcp, SIGNAL(disconnected()), this, SLOT(tcpdisconnected()));

    connect((QObject *)tcp, SIGNAL(readyRead()), this, SLOT(tcp_dataReceived()));

    connect_config_timer->start(100);


}

TcpComManager::~TcpComManager()
{
    if(connect_config_timer){
        if(connect_config_timer->isActive()){
            connect_config_timer->stop();
        }
        delete connect_config_timer;
        connect_config_timer = NULL;
    }
    if(tcp){
        tcp->disconnectFromHost();;
        tcp->close();
        delete tcp;
        tcp = NULL;
    }
    if(dtconfig){
        delete dtconfig;
        dtconfig = NULL;
    }
}

bool TcpComManager::Init(QString server_IP, QString server_Port, QString local_IP, QString local_port)
{

    serverIP = server_IP;
    serverPort = server_Port;
    localIP = local_IP;
    localPort = local_port;
    set_config_state(true);
}

void TcpComManager::get_config_info()
{
        string strTemp;
        dtconfig->GetValue("LocalIp", strTemp);
        localIP = strTemp.data();
        dtconfig->GetValue("ServerIp", strTemp);
        serverIP = strTemp.data();
        dtconfig->GetValue("LocalPort", strTemp);
        localPort = strTemp.data();
        dtconfig->GetValue("ServerPort", strTemp);
        serverPort = strTemp.data();
}


void TcpComManager::set_connection_state(bool state)
{
    connection_state = state;
}


bool TcpComManager::get_connection_state()
{
    return connection_state;
}


void TcpComManager::set_config_state(bool state)
{
    config_state = state;
}


bool TcpComManager::get_config_state()
{
    return config_state;
}


void TcpComManager::tcp_dataSend(char* send_data,int  len)
{
   // printf("Send _msg_toMec:%s\n",send_data);
    //QString  str = send_data;
    tcp->write(send_data, len);
    tcp->flush();
    tcp->waitForBytesWritten();

}


void TcpComManager::connect_to_server()
{
    cout<<"TCP: Try connect to Server..."<<endl;
    if((serverIP.isNull()) || (0 == serverPort.toInt())){
        cout<<"TCP: server-IP or server-Port null, please check configure file..."<<endl;
        return;
    }
    tcp->abort();
    QHostAddress ServerIP = QHostAddress(serverIP);
    qint16 Port = serverPort.toInt();
    tcp->connectToHost(ServerIP, Port);
    tcp->waitForConnected(3000);
   // int res = tcp->ConnectedState;
   // set_connection_state(true);
}


void TcpComManager::send_config_request()
{
    char *config_request_packet = new char[V2X_MAX_MSG_LENGTH];
    memset(config_request_packet, 0, V2X_MAX_MSG_LENGTH);
    construct_config_req_packet(config_request_packet);
    tcp_dataSend(config_request_packet,V2X_MAX_MSG_LENGTH);
}


void TcpComManager::construct_config_req_packet(char *data)
{
    Json::Value packet;
    packet["strRsuIdx"]  = Json::Value(rsu_id.data());
    packet["lLongitude"] = Json::Value(longitude);
    packet["lLatitude"]  = Json::Value(latitude);
    packet["strMsgType"] = Json::Value(msg_type_lt_para_config_request.data());

    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    memcpy(data, str_packet.c_str(), str_packet.length());
}


void TcpComManager::try_connect_config()
{
    if(get_config_state())
    {
        if(!get_connection_state())
        {
            connect_to_server();
        //   connect_config_timer->start(CONNECT_CONFIG_TIMER_PERIOD);
        }
    }
}

void TcpComManager::tcpconnected()
{
    cout<<"TCP: Connect to Server sucess..."<<endl;
    set_connection_state(true);
   emit control_heartbeat(HEARTBEAT_EVENT_START);
}


void TcpComManager::tcpconnecterror(QAbstractSocket::SocketError error_num)
{
    char error_info[100] = {0};
    sprintf(error_info, "TCP: connection error: %d, try next connection after 10 seconds...", error_num);
    cout<<error_info<<endl;
    set_connection_state(false);
    //set_config_state(false);
    tcp->abort();
}


void TcpComManager::tcpdisconnected()
{
    cout<<"TCP: connection break, restart connection..."<<endl;
    set_connection_state(false);
    //set_config_state(false);
    emit control_heartbeat(HEARTBEAT_EVENT_STOP);
    tcp->abort();
    if(connect_config_timer->isActive()){
        connect_config_timer->stop();
    }
    connect_config_timer->start(CONNECT_CONFIG_TIMER_PERIOD);
}


void TcpComManager::tcp_dataReceived()
{
    int recv_data_length = 0;    
    while((recv_data_length = tcp->bytesAvailable()) > 0)
    {
        if(recv_data_length > X2V_MAX_MSG_LENGTH)
        {
            cout<<"TCP: Receive data length exceed the maximum config, Abort..."<<endl;
            tcp->read(recv_data_length);
        }
        else{
            RecvMec_msg *p_mec = new RecvMec_msg;
            memset(p_mec, 0, sizeof(p_mec));
            tcp->read(p_mec->p_mcu_msg, recv_data_length);
            p_mec->len =recv_data_length;
            printf("RecvMsf_from_Mec:%s\n",p_mec->p_mcu_msg);
            emit  transmit_recv_msg(p_mec);
            }
        }

}


void TcpComManager::rebuilt_tcp_sock()
{
         tcp->abort();
         set_connection_state(false);
}


void TcpComManager::tcp_dataSend_slot(RecvMec_msg* send_data)
{

    printf("RecvTcp_severMsgSisnal.%s\n",send_data->p_mcu_msg);
    if(get_connection_state()){

        printf("SendToMECMsgSisnal.%s\n",send_data->p_mcu_msg);

        tcp_dataSend (send_data->p_mcu_msg,send_data->len);
        if(send_data)
        {
            delete send_data;
            send_data = NULL;
        }

    }
    else{
        if(send_data){
            delete send_data;
            send_data = NULL;
        }
    }
}


void TcpComManager::catch_config_state(bool state)
{
    set_config_state(state);
}

TcpProcessRecv::TcpProcessRecv()
{
    process_recv_timer = new QTimer(this);
    mcu_msg_queue      = new QQueue<RecvMec_msg *>;
    mcu_msg_queue->clear();
    connect((QObject *)process_recv_timer, SIGNAL(timeout()), this, SLOT(get_process_msg()));
    process_recv_timer->start(PROCESS_RECV_MSG_INTERVAL);
}


TcpProcessRecv::~TcpProcessRecv()
{
    if(process_recv_timer){
        if(process_recv_timer->isActive()){
            process_recv_timer->stop();
        }
        delete process_recv_timer;
        process_recv_timer = NULL;
    }
    if(mcu_msg_queue){
        if(!mcu_msg_queue->isEmpty()){
            mcu_msg_queue->clear();
        }
        delete mcu_msg_queue;
        mcu_msg_queue = NULL;
    }
}


void TcpProcessRecv::process_recv_msg()
{
    RecvMec_msg *p_x2v_msg = NULL;
    if(!mcu_msg_queue->isEmpty()){
          printf("process_recv_msg1\n");
        p_x2v_msg = mcu_msg_queue->dequeue();
    }
    if(p_x2v_msg){
        printf("process_recv_msg\n");
        do_process_recv_msg(p_x2v_msg);

//        if(p_x2v_msg){
//            memset(p_x2v_msg->p_mcu_msg, 0, sizeof(p_x2v_msg->p_mcu_msg));
//            delete p_x2v_msg;
//            p_x2v_msg = NULL;
//        }
    }
}


void TcpProcessRecv::do_process_recv_msg(RecvMec_msg *p_x2v_msg)
{
         printf("do_process_recv_msg,%s,%d\n",p_x2v_msg->p_mcu_msg,p_x2v_msg->len);
        printf("msgId%d\n",p_x2v_msg->p_mcu_msg[6]);
        if(p_x2v_msg->p_mcu_msg [6]==84)
        {
         //  printf("HeartBeat FeedBack receive time: ");
//            Log("HeartBeat FeedBack receive time: ");
//            getstimeval();
           printf("HeartBeat:%s,Packelen:%d\n,",p_x2v_msg->p_mcu_msg,p_x2v_msg->len);

            emit  clear_heartbeat(HEARTBEAT_EVENT_CLEAR);
            delete p_x2v_msg;
            p_x2v_msg = NULL;
        }
       else
       {
            printf("Send_TxBox_toMec_recv_msg:%s,Packelen:%d\n",p_x2v_msg->p_mcu_msg,p_x2v_msg->len);
            emit  Send_TxBox_toMec_recv_msg(p_x2v_msg);
       }

}


void TcpProcessRecv::getstimeval()
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
       // printf("%s\n",buf);
        //LOG_INFO("Function_%s,lineNum_%d,%s\n" ,,422,CarSelf_data.c_str());
}


void TcpProcessRecv::accept_net_msg(RecvMec_msg* recv_mcuMsg)
{

       //   printf("accept_net_msg:%s,accept_net_msg_Pack_len:%d\n",recv_mcuMsg->p_mcu_msg,recv_mcuMsg->len);
         mcu_msg_queue->enqueue(recv_mcuMsg);
}


void TcpProcessRecv::get_process_msg()
{
  //   printf("get_process_msg\n") ;
    process_recv_msg();
}


HeartBeatThread::HeartBeatThread()
{
    Heart_timer = new QTimer(this);
    clear_heartbeats_count();
    connect((QObject *)Heart_timer, SIGNAL(timeout()), this, SLOT(shot_heartbeat()));
    Heart_timer->start(HEARTBEAT_TIMER_INTERVAL);
}


HeartBeatThread::~HeartBeatThread()
{
    if(Heart_timer){
        if(Heart_timer->isActive()){
            Heart_timer->stop();
        }
        delete Heart_timer;
        Heart_timer = NULL;
    }
}

void HeartBeatThread::inc_heartbeats_count()
{
    Heartbeats_count = +1;
}


void HeartBeatThread::clear_heartbeats_count()
{
    Heartbeats_count = 0;
}


int HeartBeatThread::get_heartbeats_count()
{
    return Heartbeats_count;
}


void HeartBeatThread::start_heartbeat()
{
    clear_heartbeats_count();
    Heart_timer->start(HEARTBEAT_TIMER_INTERVAL);
}


void HeartBeatThread::stop_heartbeat()
{
    Heart_timer->stop();
    clear_heartbeats_count();
}


void HeartBeatThread::construct_heartbeat_req_packet(RecvMec_msg *data)
{
        *(data ->p_mcu_msg) = 0x7E;
        *((data ->p_mcu_msg) + 1 ) = 0x7E;
        *((data ->p_mcu_msg) + 2 ) = 0x7E;
        *((data ->p_mcu_msg) + 3 ) = 0x7E;
        *((data ->p_mcu_msg)+4) = 0x01;
        //  char addr = 0x01;
         if(Obu_id ==20)
         {
             *((data ->p_mcu_msg) + 5) = 0x02;
         }
         if(Obu_id==10)
         {
             *((data ->p_mcu_msg) + 5) = 0x01;
         }
        // char cid = msg_type_RSU_TO_MEC_resqust_time;
        *((data ->p_mcu_msg) + 6) = 0x54;
        unsigned short length =1;
        *(((unsigned char*)(data ->p_mcu_msg) + 7)) = length & 0x00FF;
        *(((unsigned char*)(data ->p_mcu_msg) + 8)) = length >> 8 & 0x00FF;
        /********************************************/
        *((data ->p_mcu_msg)+9) =0;
        char sum = (data ->p_mcu_msg)[0];
        for(int i = 1;i < (length + 9);i++ )
        {
            sum = sum ^ (data ->p_mcu_msg)[i];
        }
        /********************************************/
        *((data ->p_mcu_msg) + length +9) = (0x7F & sum);
         *((data ->p_mcu_msg) + length+ 10) = 0x0D;

         *((data ->p_mcu_msg) + length + 11) = 0x0A;
            data->len = length+11;

}

void HeartBeatThread::shot_heartbeat()
{
    if(MAX_HEARTBEAT_HALT_COUNTS < get_heartbeats_count())
    {
        stop_heartbeat();
        emit sig_rebuilt_tcp_socket();
    }
    else{
        RecvMec_msg * data  = new RecvMec_msg;
//        char *heartbeat_packet = new char[HEART_PACKET_LENGTH];
        //memset(data, 0, V2X_MAX_MSG_LENGTH);
        construct_heartbeat_req_packet(data);
        emit send_heartbeats_packet(data);
      //  printf("HeartBeat send time :");
//        Log("HeartBeat send time :");
//        getstimeval();
        inc_heartbeats_count();
    }
}


void HeartBeatThread::process_heartbeats_event(int event)
{
    switch(event){
    case HEARTBEAT_EVENT_START:
        start_heartbeat();
        break;
    case HEARTBEAT_EVENT_STOP:
        stop_heartbeat();
        break;
    case HEARTBEAT_EVENT_CLEAR:
        clear_heartbeats_count();
        break;
    }
}


void HeartBeatThread::getstimeval()
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
      //  printf("%s\n",buf);
}



















