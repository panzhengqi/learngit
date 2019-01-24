#include <QCoreApplication>
#include <iostream>
#include "CommonSerialPort.h"
#include "TcpServer.h"
#include "../Can/can_manager.h"
#include "TcpServer.h"
#include "../CommonLog/log.h"
#include "recvcanmsgprocess.h"
#include "recvtongjiudp.h"

using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString LocalIp_str;
    QString udpLocalPort_str;
    QString udpRemoteIp_str;
    QString udpRemotePort_str;
    QString udpTongjiIp_str;
    QString udpTongjiPort_str;
    QString udpTongji_LocalPort_str;
    QString UdpPhone1_RemoteIP_str;
   QString UdpPhone2_RemoteIP_str;
   // QString  Local_TcpClientPort;
    int Local_tcpserver_port;
    int serialport_num;
    int serialPort_baud_rate;
    int cardId=0;
    QString Mec_tcpServerIp_str;
    QString Mec_tcpserver_port;
    QString tcpClientPort_str;

    string strSystemConfigFile = "/root/Config/Config.ini";
    string LogFile = "/root/Config/Log/ObuControl.log";

    DTConfig * dtconfig      = new DTConfig(strSystemConfigFile, false);
    Log::get_instance()->init(LogFile.c_str(),1024*1024*1024,50000,1);
    string strTemp;
/*
    if(dtconfig->GetValue("UdpPhone1_RemoteIP", strTemp))
    {
        UdpPhone1_RemoteIP_str = strTemp.data();
        printf("Get UdpPhone1_RemoteIP Success\n");
    }
    else
    {
        printf("Get UdpPhone1_RemoteIP failed\n");
        return false;
    }
    if(dtconfig->GetValue("UdpPhone2_RemoteIP", strTemp))
    {
        UdpPhone2_RemoteIP_str = strTemp.data();
        printf("Get UdpPhone2_RemoteIP Success\n");
    }
    else
    {
        printf("Get UdpPhone2_RemoteIP failed\n");
        return false;
    }
    if(dtconfig->GetValue("UdpLocalIp", strTemp))
    {
        LocalIp_str = strTemp.data();
        printf("Get UdpLocalIp Success\n");
    }
    else
    {
        printf("Get UdpLocalIp failed\n");
        return false;
    }

    if(dtconfig->GetValue("UdpRemoteIp", strTemp))
    {
         udpRemoteIp_str = strTemp.data();
         printf("Get UdpRemoteIp Sucess\n");
    }
    else
    {
        printf("Get UdpRemoteIp failed\n");
        return false;
    }

    if(dtconfig->GetValue("UdpLocalPort", strTemp))
    {
        udpLocalPort_str = strTemp.data();
        printf("Get UdpLocalPort Sucess\n");
    }
    else
    {
        printf("Get UdpLocalPort failed\n");
        return false;
    }

    if(dtconfig->GetValue("UdpRemotePort", strTemp))
    {
        udpRemotePort_str = strTemp.data();
        printf("Get UdpRemotePort Sucess\n");
    }
    else
    {
        printf("Get UdpRemotePort failed\n");
        return false;
    }

    if(dtconfig->GetValue("ue_id", strTemp))
    {
        cardId = atoi(strTemp.c_str());
    }
    else
    {
        printf("Get CardID failed\n");
        return false;
    }

    if(dtconfig->GetValue("LocalTcpServerPort", strTemp))
    {
        Local_tcpserver_port = atoi(strTemp.c_str());
        printf("Get TcpServerPort Success\n");
    }
    else
    {
        printf("Get LocalIP failed\n");
        return false;
    }

    if(dtconfig->GetValue("MecTcpServerIp", strTemp))
    {
        Mec_tcpServerIp_str = strTemp.data();
        printf("Get MecTcpServerIp Success\n");
    }
    else
    {
        printf("Get MecTcpServerIp  failed\n");
        return false;
    }

    if(dtconfig->GetValue("MecTcpServerPort", strTemp))
    {
        Mec_tcpserver_port = strTemp.data();
        printf("Get MecTcpServerPort Success\n");
    }
    else
    {
        printf("Get MecTcpServerPort  failed\n");
        return false;
    }

    if(dtconfig->GetValue("TcpClientPort", strTemp))
    {
        tcpClientPort_str = strTemp.data();
        printf("Get TcpClientPort Success\n");
    }
    else
    {
        printf("Get TcpClientPort failed\n");
        return false;
    }

    if(dtconfig->GetValue("SerialPortNum", strTemp))
    {
        serialport_num = atoi(strTemp.c_str());
        printf("Get Serialport_num Success\n");
    }
    else
    {
        printf("Get Serialport_num failed\n");
        return false;
    }

    if(dtconfig->GetValue("baud_rate", strTemp))
    {
        serialPort_baud_rate = atoi(strTemp.c_str());
        printf("Get Serialport_num Success\n");
    }
    else
    {
        printf("Get Serialport_num failed\n");
        return false;
    }

    if(dtconfig->GetValue("UdpTongJiIp", strTemp))
    {
        udpTongjiIp_str =  strTemp.data();
        printf("Get UdpTongJiIp Success\n");
    }
    else
    {
        printf("Get UdpTongJiIp failed\n");
        return false;
    }

    if(dtconfig->GetValue("UdpTongjiPort", strTemp))
    {
        udpTongjiPort_str =  strTemp.data();
        printf("Get UdpTongjiPort Success\n");
    }
    else
    {
        printf("Get UdpTongjiPort failed\n");
        return false;
    }

    if(dtconfig->GetValue("UdpTongji_LocalPort", strTemp))
    {
        udpTongji_LocalPort_str =  strTemp.data();
        printf("Get UdpTongji_LocalPort Success\n");
    }
    else
    {
        printf("Get UdpTongji_LocalPort failed\n");
        return false;
    }
*/
    UdpCommon  *udp_Andriod = new UdpCommon();
    udp_Andriod->getUdpObject()->SetPhone1IP(UdpPhone1_RemoteIP_str);
    udp_Andriod->getUdpObject()->SetPhone2IP(UdpPhone2_RemoteIP_str);
    udp_Andriod->getUdpObject()->Init(LocalIp_str,udpLocalPort_str,udpRemoteIp_str,udpRemotePort_str);


    UdpCommon *udp_tongji = new UdpCommon();
    udp_tongji->getUdpObject()->Init(LocalIp_str,udpTongji_LocalPort_str,udpTongjiIp_str,udpTongjiPort_str);

    can_manager *can = new can_manager();
    QThread*  can_thread = new QThread();
    can->moveToThread(can_thread);
    can_thread->start();

    RtkPacketControl *rtkpacket = new RtkPacketControl(can->p_inner_recv_manager);
    QThread*   rtkpacket_thrd = new QThread();
    rtkpacket->moveToThread(rtkpacket_thrd);
    rtkpacket_thrd->start();


    RecvCanMsgProcess * recvCan = new RecvCanMsgProcess();
    QThread* recvCanProcessThread = new QThread();
    recvCan->moveToThread(recvCanProcessThread);
    recvCanProcessThread->start();
    recvCan->SetUdpSendProcess(udp_tongji->getUdpObject());
    recvCan->Set_can_recv_manager(can->p_can_recv_manager);
    recvCan->Init();


    RecvTongjiUdp *  recvTongjiUdp = new RecvTongjiUdp();
    QThread*  udpSendProcessThread = new QThread();
    recvTongjiUdp->moveToThread(udpSendProcessThread);
    udpSendProcessThread->start();
    recvTongjiUdp->SetRecvAndriodUdpProcess(udp_Andriod->getUdpObject());
    recvTongjiUdp->SetRecvTongjiUdpProcess(udp_tongji->getUdpObject());
    recvTongjiUdp->Set_inner_recv_manager(can->p_inner_recv_manager);
    recvTongjiUdp->SetCardID(cardId);
    recvTongjiUdp->Init();



    CommonLtev2x * com_ltev2x = new CommonLtev2x(udp_Andriod->getUdpObject(),udp_tongji->getUdpObject(),can->p_inner_recv_manager,rtkpacket);

    GpsAnalyse*  gprs = new GpsAnalyse(udp_Andriod->getUdpObject(),udp_tongji->getUdpObject(),can->p_inner_recv_manager,com_ltev2x->ltev_msg_send);

    QThread*  gprs_thrd = new QThread();
    gprs->moveToThread(gprs_thrd);
    gprs_thrd->start();
    gprs->SetCardID(cardId);
    gprs->SetRtkPacketControl(rtkpacket);  
    gprs->Init();

    
    TcpServer::Instantialize();
    TcpServer::pInstance->Init(Local_tcpserver_port);

    TcpClientCommon* tcp_client = new TcpClientCommon();
    tcp_client->p_TcpComManager->Init(Mec_tcpServerIp_str,Mec_tcpserver_port,LocalIp_str,tcpClientPort_str);
    tcp_client->p_heartbeats->SetObuID(cardId);
    
    CommonSerialPort * serialport = new CommonSerialPort();
    serialport->SetGpsObject(gprs);
    serialport->OpenCom(serialport_num);
    serialport->SetCom(serialPort_baud_rate,8,'N',1);

    return a.exec();
}
