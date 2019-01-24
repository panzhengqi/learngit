#ifndef TCPCLIENTCOMMON_H
#define TCPCLIENTCOMMON_H

#include <QThread>
#include <qprocess.h>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QTimer>
#include <QQueue>
#include "../JasonControl/json/json.h"
#include "config.h"
#include "UdpCommon.h"

#include<sys/time.h>
#include<time.h>
#include<stdio.h>
#include<string.h>

#define  CONNECT_CONFIG_TIMER_PERIOD       5000
#define  HEARTBEAT_TIMER_INTERVAL          5000
//#define  PROCESS_RECV_MSG_INTERVAL         100
#define  PROCESS_LIGHT_MSG_INTERVAL        100

#define  MAX_HEARTBEAT_HALT_COUNTS  100

#define  V2X_MAX_MSG_LENGTH         1500
#define  X2V_MAX_MSG_LENGTH         1500
#define  HEART_PACKET_LENGTH         600

#define  HEARTBEAT_EVENT_START      0x01
#define  HEARTBEAT_EVENT_STOP       0x02
#define  HEARTBEAT_EVENT_CLEAR      0x03


typedef struct RecvMec_msg
{
    char p_mcu_msg[V2X_MAX_MSG_LENGTH];
    int len ;
}RecvMec_msg;
//Q_DECLARE_METATYPE(v2x_msg)


typedef struct SendMec_msg
{
    char p_mec_msg[X2V_MAX_MSG_LENGTH];
    int len;
}SendMec_msg;
//Q_DECLARE_METATYPE(x2v_msg)


class TcpComManager : public QObject
{
    Q_OBJECT
public:
    TcpComManager();
    ~TcpComManager();
    bool Init(QString server_IP, QString server_Port, QString local_IP, QString local_port);

    void get_config_info();
    void set_connection_state(bool);
    bool get_connection_state();
    void set_config_state(bool);
    bool get_config_state();


    void tcp_dataSend(char*,int len);
    void connect_to_server();
    void send_config_request();
    void construct_config_req_packet(char* data);

public:
    DTConfig    *dtconfig;

private:
    QTimer      *connect_config_timer;
    QTcpSocket  *tcp;
    bool         connection_state;
    bool         config_state;
    QString      serverIP;
    QString      serverPort;
    QString      localIP;
    QString      localPort;

signals:
    void transmit_recv_msg(RecvMec_msg* recv_msg );
    void control_heartbeat(int);

public slots:
    void try_connect_config();
    void tcpconnected();
    void tcpconnecterror(QAbstractSocket::SocketError);
    void tcpdisconnected();
    void tcp_dataReceived();
    void rebuilt_tcp_sock();
    void tcp_dataSend_slot(RecvMec_msg* send_data);
    void catch_config_state(bool state);
};


class TcpProcessRecv : public QObject
{
    Q_OBJECT
public:
    TcpProcessRecv();
    ~TcpProcessRecv();

    TcpComManager  *p_tcpcommanager;
public:
    void process_recv_msg();
    void do_process_recv_msg(RecvMec_msg *p_mcu_msg);
    void getstimeval();

public slots:
    void accept_net_msg(RecvMec_msg* recv_msg );
    void get_process_msg();

signals:
    void clear_heartbeat(int);
    //void Send_TxBox_toMec_recv_msg(SendMec_msg *);
     void Send_TxBox_toMec_recv_msg(RecvMec_msg *p_mcu_msg);
    //void get_light_config(bool);

private:
    QTimer            *process_recv_timer;
    QQueue<RecvMec_msg *> *mcu_msg_queue;
};


class HeartBeatThread : public QObject
{
    Q_OBJECT
public:
    HeartBeatThread();
    ~HeartBeatThread();

    QTimer      *Heart_timer;

public:
    void inc_heartbeats_count();
    void clear_heartbeats_count();
    int  get_heartbeats_count();
    void start_heartbeat();
    void stop_heartbeat();
    void construct_heartbeat_req_packet(RecvMec_msg* data);
    void construct_packet(int req_type, char* data);
    void  SetObuID(int value)
    {
        Obu_id = value;
    }
    void getstimeval();
public slots:
    void shot_heartbeat();
    void process_heartbeats_event(int);

signals:
    //void send_heartbeats_packet(char* data);
    void send_heartbeats_packet(RecvMec_msg* data);
    void sig_rebuilt_tcp_socket();

private:
    int          Heartbeats_count;
    int          Obu_id =0 ;
};



class TcpClientCommon
{
public:
    TcpClientCommon();
     ~TcpClientCommon();
public:
    QThread         *p_tcpcommanager_thread;
    TcpComManager   *p_TcpComManager;
    QThread         *process_recv_thread;
    TcpProcessRecv  *p_process_recv_obj;
    QThread         *p_heartbeat_thread;
    HeartBeatThread *p_heartbeats;
    QThread         *p_light_manager_thread;
   // LightManager    *p_light_manager;



};

#endif // TCPCLIENTCOMMON_H
