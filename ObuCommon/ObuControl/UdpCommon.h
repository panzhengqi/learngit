#ifndef UDPCOMMON_H
#define UDPCOMMON_H

#include "QThread"
#include "QMutex"
#include "QMutexLocker"
#include <qprocess.h>
#include <QtCore/QIODevice>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include <QTimer>
#include <QQueue>
#include "arpa/inet.h"
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "../JasonControl//json/json.h"
#include "list"
#include "../CommonConfig/config.h"


#define  PROCESS_RECV_MSG_INTERVAL         100

class UdpComManager :public QObject
{
    Q_OBJECT
public:
    UdpComManager();
    ~UdpComManager();
    bool get_config_info();
    void udp_dataSend(char* send_data,int pack_len);
    void set_connection_state(bool);
    bool get_connection_state();
    void set_config_state(bool);
    bool get_config_state();

    bool Init(QString local_IP, QString local_port,QString remote_IP, QString remote_Port);

private:
    QUdpSocket   *udp;
    QString       remoteIP;
    QString       remotePort;
    QString       phone1IP;
    QString       phone2IP;
    QString       localIP;
    QString       localPort;
    QHostAddress  localaddr;
    QTimer*      udp_timer;
    bool         connection_state;
    bool         config_state;
    bool         connectRecvTongji_state;

    //RecvTongjiUdp *recvtongjiudp;

public:
      void  SetPhone1IP(QString value)
      {
          phone1IP = value;
      }
      void   SetPhone2IP(QString value)
      {

          phone2IP = value;
      }
//    RecvTongjiUdp* SetRecvTongjiUdpProcess(RecvTongjiUdp* value)
//    {
//             recvtongjiudp = value;
//    }
public slots:
    void udpconnecterror(QAbstractSocket::SocketError);
    void udp_dataReceived();
    void udp_dataSend_slot(char *send_data,int pack_len);
    bool connect_to_server();
signals:
      void SendMsgTongjiMsgPacket(char* send_data,int pack_len);
       void SendGpsMsgPacket(char* send_data,int pack_len);
};


class UdpCommon : public QObject
{
    Q_OBJECT
public:
    UdpCommon();
    ~UdpCommon();  
public:
    UdpComManager* getUdpObject() const
    {
        return p_UdpComManager;
    }
private:
    QThread         *p_udpcommanager_thread;
    UdpComManager   *p_UdpComManager;
    QThread         *process_recv_thread;
    //UdpProcessRecv  *p_process_recv_obj;
    QThread         *p_MecMsg_thread;
    DTConfig        *dtconfig;

};

#endif // UDPCOMMON_H
