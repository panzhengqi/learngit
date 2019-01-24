#ifndef  RECVTONGJIUDP_H
#define RECVTONGJIUDP_H

#include  <QObject>
#include  <QTimer>
#include  <QQueue>
#include <sys/time.h>
#include  <QThread>
#include <UdpCommon.h>
#include  "tongjixieyiprcase.h"
#include  "../Can/inner_recv_manager.h"
#include  "../Can/includes.h"
#include  "../CommonLog/log.h"
#define TJ_PERIOD_10MS    10


class RecvTongjiUdp : public QObject
{
    Q_OBJECT
public:
    explicit RecvTongjiUdp(QObject *parent = 0);
    ~RecvTongjiUdp();
    void Init();
    void process_recv_msg();
    void do_process_recv_msg(RecvMSgPacket  *recv_Tong_msg);
    void getstimeval();
    void ConstructJsonString_ToCan(RecvMSgPacket* buf);
    void  ConstructJsonString_ToAndriod(RecvMSgPacket* buf);

private:
    QTimer            *udp_send_timer;
    int                timeout_count;
     QQueue<RecvMSgPacket *> *Recv_Tongji_msgqueue;
    // QThread * udpSendProcessThread;
     UdpComManager*  udpCom_Tongji;
     UdpComManager*  udpCom_Andriod;
     TongjiXieyiPrcase * tongj;
     inner_recv_manager* recv_manger;
     int condition =0;
     int CardID;
public:
     void   SetCardID(int value)
     {

         CardID = value;
     }
public:
       void SetRecvTongjiUdpProcess(UdpComManager* value)
       {
                udpCom_Tongji = value;
       }
      void  SetRecvAndriodUdpProcess(UdpComManager* value)
       {
                 udpCom_Andriod = value;
       }
       void  Set_inner_recv_manager(inner_recv_manager* value)
       {
                 recv_manger = value;
       }

public slots:
    void RecvMsg(char *buf,int len);
    void get_process_msg();

signals:
    void SendToCan(can_msg* data);

    void SendToUdpAndriod(char* buff,int len);

};



#endif // RECVTONGJIUDP_H
