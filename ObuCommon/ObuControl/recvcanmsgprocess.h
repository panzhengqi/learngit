#ifndef RECVCANMSGPROCESS_H
#define RECVCANMSGPROCESS_H

#include  <QObject>
#include  <QTimer>
#include  <QQueue>
#include <sys/time.h>
#include  <QThread>
#include  "tongjixieyiprcase.h"
#include "UdpCommon.h"
#include "can_recv_manager.h"



#define PERIOD_10MS    10

class RecvCanMsgProcess : public QObject
{
    Q_OBJECT
public:
    explicit RecvCanMsgProcess(QObject *parent = 0);
    ~RecvCanMsgProcess();
    void Init();
    void process_recv_msg();
    void do_process_recv_msg(Obu_algorithm_paras  *p_recv_canMsg);
    void getstimeval();

private:
    QTimer            *recv_can_timer;
    int                timeout_count;
     QQueue<Obu_algorithm_paras *> *CanToTongji_msgqueue;
     QThread * recvCanProcessThread;
     UdpComManager*  udpCom_Tongji;
     TongjiXieyiPrcase * tongj;
        can_recv_manager * can_recv_man;
     int datalen=0;

public:
       UdpComManager* SetUdpSendProcess(UdpComManager* value)
       {
                udpCom_Tongji = value;
       }
    can_recv_manager * Set_can_recv_manager(can_recv_manager* value)
    {
                can_recv_man =value;
    }
public slots:
    void RecvMsg(Obu_algorithm_paras *p_TongjiToAndriod_msg );
    void get_process_msg();

signals:
  //  void clear_heartbeat(int);
    //void Send_TxBox_toMec_recv_msg(SendMec_msg *);
     //void Send_TxBox_toMec_recv_msg(RecvMec_msg *p_mcu_msg);
    //void get_light_config(bool);
    void SendToUdpTongji(char* buff,int len);

public slots:

};
#endif // RECVCANMSGPROCESS_H
