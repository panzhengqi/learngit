#include "UdpCommon.h"

using namespace std;

UdpCommon::UdpCommon()
{
    p_udpcommanager_thread = new QThread();
    p_UdpComManager      = new UdpComManager();   
    p_UdpComManager->moveToThread(p_udpcommanager_thread);  
    p_udpcommanager_thread->start();
}

UdpCommon::~UdpCommon()
{
    if(p_udpcommanager_thread)
    {
        p_udpcommanager_thread->quit();
        p_udpcommanager_thread->wait();
        delete p_udpcommanager_thread;
        p_udpcommanager_thread = NULL;
    }
    if(p_UdpComManager)
    {
        delete p_UdpComManager;
        p_UdpComManager = NULL;

    }

}

UdpComManager::UdpComManager()
{

    udp   = new QUdpSocket(this);
    udp_timer = new QTimer(this);
    connect((QObject *)udp_timer, SIGNAL(timeout()), this, SLOT(connect_to_server()));
//    connect((QObject *)udp, SIGNAL(error(QAbstractSocket::SocketError)), this,
//           SLOT(udpconnecterror(QAbstractSocket::SocketError)));
//    connect((QObject *)udp, SIGNAL(readyRead()), this, SLOT(udp_dataReceived()));
    config_state = false;
    connectRecvTongji_state =false;
    udp_timer->start(50);
}

UdpComManager::~UdpComManager()
{
    if(udp)
    {
       // udp->disconnectFromHost();;
        udp->close();
        delete udp;
        udp = NULL;
    }
}

bool UdpComManager::Init( QString local_IP, QString local_port,QString remote_IP, QString remote_Port)
{
    remoteIP = remote_IP;
    remotePort = remote_Port;
    localIP = local_IP;
    localPort = local_port;
    set_config_state(true);
}

void UdpComManager::set_connection_state(bool state)
{
    connection_state = state;
}

bool UdpComManager::get_connection_state()
{
    return connection_state;
}


void UdpComManager::set_config_state(bool state)
{
    config_state = state;
}


bool UdpComManager::get_config_state()
{
    return config_state;
}

void UdpComManager::udp_dataSend(char* send_data,int pack_len)
{
    if(connection_state)
    {
//        printf("Send ToAdriod %s\n",send_data);
        if(remoteIP!=NULL)
        {
            udp->writeDatagram(send_data, pack_len, QHostAddress(remoteIP), remotePort.toInt());
            udp->waitForBytesWritten();
        }
        if(phone1IP!=NULL)
        {
            udp->writeDatagram(send_data, pack_len, QHostAddress(phone1IP), remotePort.toInt());
            udp->waitForBytesWritten();
        }
        if(phone2IP!=NULL)
        {
            udp->writeDatagram(send_data, pack_len, QHostAddress(phone2IP), remotePort.toInt());

            udp->waitForBytesWritten();
        }
        delete send_data;
        send_data = NULL;
    }
}

bool UdpComManager::connect_to_server()
{
    if(!get_config_state())
        return false;
    if(get_connection_state())
        return false;
    if((localIP.isNull()) || (0 == localPort.toInt())){
        cout<<"UDP: server-IP or server-Port null, please check configure file..."<<endl;
        return false;
    }
     udp->abort();
    if(udp->bind(QHostAddress(localIP), localPort.toInt()))
    {
        connect((QObject *)udp, SIGNAL(error(QAbstractSocket::SocketError)), this,
               SLOT(udpconnecterror(QAbstractSocket::SocketError)));
        connect((QObject *)udp, SIGNAL(readyRead()), this, SLOT(udp_dataReceived()));
        cout<<"UDP: bind Success\n"<<endl;
    }
    else
    {
          cout<<"UDP: bind failed\n"<<endl;
        return false;

    }
    set_connection_state(true);
    return true;
}

void UdpComManager::udpconnecterror(QAbstractSocket::SocketError error_num)
{
    char error_info[100] = {0};
    sprintf(error_info, "UDP: connection error: %d, try next connection after 10 seconds...", error_num);
    cout<<error_info<<endl; 
    udp->abort();
    set_connection_state(false);
}

void UdpComManager::udp_dataReceived()
{
    int recv_data_length = 0;
    QHostAddress sender;
    quint16 senderPort;    
    while(udp->hasPendingDatagrams())
    {
        recv_data_length = udp->pendingDatagramSize();
        printf("udp_dataReceived_%d\n",recv_data_length);
        if(recv_data_length>0)
         {
            char * msg = new char[recv_data_length+2];
            memset(msg,0,recv_data_length+2);
            udp->readDatagram(msg,recv_data_length+2, &sender, &senderPort);
             printf("udp_dataReceived_%d,%d\n",msg[0],msg[1]);
            emit SendMsgTongjiMsgPacket(msg,recv_data_length);
        }

          //tong->DeCodePacket(msg,recv_data_length);
     }
}

void UdpComManager::udp_dataSend_slot(char *send_data,int pack_len)
{
        // printf("SendTo_UDP%s\n",send_data);
        udp_dataSend(send_data,pack_len);
}






