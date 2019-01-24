#ifndef  RECVTONGJIUDP_H
#define RECVTONGJIUDP_H


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
class RecvTongjiUdp : public QObject
{
    Q_OBJECT
public:
    explicit RecvTongjiUdp(QObject *parent = 0);
    ~RecvTongjiUdp();
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
    void RecvMsg(char *buf,int len);
    void get_process_msg();

signals:
    void SendToCan(uchar data);
    void SendToUdpAndriod(char* buff,int len);
};



#endif // RECVTONGJIUDP_H
