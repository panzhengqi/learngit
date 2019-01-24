#ifndef COMMONLTEV2X_H
#define COMMONLTEV2X_H

#include <string>
#include <stdio.h>
#include <QTimer>
#include <QThread>
#include "stdlib.h"
#include "UdpCommon.h"
#include "ltev2x.h"
#include "../CommonConfig/config.h"
#include "../Can/inner_recv_manager.h"
#include "../JasonControl/json/json.h"
#include "rtkpacketcontrol.h"
#include "../CommonLog/log.h"
#include "tongjixieyiprcase.h"

#define LTE_RECV_MSG_LEN        1024
#define Recv_ltev_Period        50

#define OBSTACLE_DATA_LEN       44
#define FIXED_LENGTH            7


typedef struct Obstacle_stru{
    unsigned char I2V_Crosscol;
    unsigned char I2V_Target_ID;
    unsigned char I2V_Target_Location;
    unsigned char I2V_Target_Type;
    short I2V_Target_Range_X;
    unsigned short I2V_Target_Range_Y;
    short I2V_Target_Range_velocity_X;
    short I2V_Target_Range_velocity_Y;
    double I2V_Target_longitude;
    double I2V_Target_latitude;
    double I2V_Target_altitude;
    double UTC_Time;
}Obstacle_stru;

class LtevMsgSend :public QObject
{
    Q_OBJECT
    public:
      LtevMsgSend();
      ~LtevMsgSend();
     public slots:
     void Ltev_sendMsg(char * buff,int packet_len);

};

//typedef list<char *> Letv_RecvList;
typedef QQueue<char *>  Letv_RecvList;
class CommonLtev2x_List:public QObject
{
    Q_OBJECT
public:
    CommonLtev2x_List ();
    ~CommonLtev2x_List();


private:
    TongjiXieyiPrcase * tongji;
    Letv_RecvList * letv_RecvList;
    //Letv_RecvList  ltevlist_recv;
    static pthread_mutex_t CommonLtev2x_List_lock;
    void  pack_analyse(char *buf);
     QTimer            *Recv_ltev_timer;

     void   parse_obstacle_string(char *source, char *dest);
     int    get_int_number(char *s);
     double get_double_number(char *s);
     int    GetComma(int num,char *str);


public slots:
    void   Ltev_PackControlThread();
    void   RecvLtev_ListInsert(char *buf);
signals:
    void   LtevLightMsg_SendToCan(can_msg *canmsg);
    void   ObuRecvSendToTngji(char *buff,int len);

    void   InsertCarRtkMsg_Front(char *buf);
};

class LtevMsgRecv :public QObject
{
  Q_OBJECT
  public:
    LtevMsgRecv();
    ~LtevMsgRecv();
        void getstimeval();
public slots:
    void Recv_LtevMsg();
signals:
    // void clear_heartbeat(int);
    void Ltev_SendtoUdp(char*buff,int pack_len);
    void LtevPacket_ListInsert(char *buf);
private:
   // QTimer            *Ltev_recv_timer;
    bool               thread_state ;
};

class CommonLtev2x :public QObject
{
     Q_OBJECT
public:
    CommonLtev2x(UdpComManager*udpCom ,UdpComManager* udpTongji,inner_recv_manager* inner_recv,RtkPacketControl *rtkPacket);
  //  CommonLtev2x (UdpComManager*udpCom,inner_recv_manager *inner_recv,RtkPacketControl* rtkPacket);
    //CommonLtev2x (UdpComManager*udpCom);
     ~CommonLtev2x();
    int  v2xInit();


public:
    DTConfig*   dtconfig;

    QThread          *ltevMsg_revThread;
    LtevMsgRecv      *ltev_msg_recv;
    QThread          *ltevMsg_sendThread;
    LtevMsgSend      *ltev_msg_send;
    QThread          *ltevCan_SendThread;
    CommonLtev2x_List *ltevCan_letv;

signals:
    void startRecv();
};


#endif // COMMONLTEV2X_H
