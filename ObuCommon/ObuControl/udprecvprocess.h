#ifndef UDPRECVPROCESS_H
#define UDPRECVPROCESS_H

#include  <QObject>
#include  <QTimer>
#include  <QQueue>
#include <sys/time.h>
#include  <QThread>
#include  "tongjixieyiprcase.h"
#include "UdpCommon.h"

#define PERIOD_10MS    10
#define MSGTongji_ToAndriod_LENGTH 1024
typedef struct RecvMSgPacket
{
    int len ;
    int msgTyp;
    char p_mcu_msg[MSGTongji_ToAndriod_LENGTH];
}RecvMSgPacket;
class UdpRecvProcess : public QObject
{
    Q_OBJECT
public:
    explicit UdpRecvProcess(QObject *parent = 0);
    ~UdpRecvProcess();
    void Init();
    void process_recv_msg();
    void do_process_recv_msg(RecvMSgPacket  *p_TongjiToAndriod_msg);
    void getstimeval();

private:
    QTimer            *udp_send_timer;
    int                timeout_count;
     QQueue<RecvMSgPacket *> *TongjiToAndriod_msgqueue;
     QThread * udpSendProcessThread;
     UdpComManager*  udpCom_Andriod;
     TongjiXieyiPrcase * tongj;
public:
       UdpComManager* SetUdpSendProcess(UdpComManager* value)
       {
                udpCom_Andriod = value;
       }

public slots:
    void RecvMsg(RecvMSgPacket *p_TongjiToAndriod_msg );
    void get_process_msg();

signals:
  //  void clear_heartbeat(int);
    //void Send_TxBox_toMec_recv_msg(SendMec_msg *);
     //void Send_TxBox_toMec_recv_msg(RecvMec_msg *p_mcu_msg);
    //void get_light_config(bool);
    void SendToUdpAndriod(char* buff,int len);

public slots:

};
#endif // #endif // UDPRECVPROCESS_H
