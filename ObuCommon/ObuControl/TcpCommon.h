#ifndef TCPCOMMON_H
#define TCPCOMMON_H
#include "QThread"
#include "QMutex"
#include "QMutexLocker"
#include <qprocess.h>
#include <QtCore/QIODevice>
#include <QtNetwork/QTcpSocket>

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
#include "../JasonControl/json/json.h"
#include "list"
#include "../CommonConfig/config.h"


#define  PROCESS_RECV_MSG_INTERVAL         100

class TcpComManager :public QObject
{
    Q_OBJECT
public:
    TcpComManager();
    ~TcpComManager();
    bool get_config_info();
    void tcp_dataSend(char* send_data,int pack_len);
    bool connect_to_server();
    bool SendMsg_toSemaphore(const char *buf,int len);

    DTConfig*  dtconfig;
private:

    QTcpSocket*  tcp;
    QString       serverIP;
    QString       serverPort;
    QString       localIP;
    QString       localPort;
    QHostAddress  localaddr;

public slots:
    void tcpconnecterror(QAbstractSocket::SocketError);
    void tcp_dataReceived();
    void tcp_dataSend_slot(char *send_data,int pack_len);
};


class TcpCommon : public QObject
{
    Q_OBJECT
public:
    TcpCommon();
    ~TcpCommon();
public:
    TcpComManager* gettcpObject() const
    {
        return p_tcpComManager;
    }
private:
    QThread         *p_tcpcommanager_thread;
    TcpComManager   *p_tcpComManager;
    QThread         *process_recv_thread;
    //tcpProcessRecv  *p_process_recv_obj;
    QThread         *p_MecMsg_thread;
    DTConfig        *dtconfig;
};

#endif // TCPCOMMON_H

