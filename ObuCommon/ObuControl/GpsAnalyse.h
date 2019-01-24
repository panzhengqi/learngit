#ifndef GPSANALYSE_H
#define GPSANALYSE_H

#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "pthread.h"
#include "TcpServer.h"
#include "commonltev2x.h"
#include "UdpCommon.h"
#include "../Can/inner_recv_manager.h"
#include "rtkpacketcontrol.h"
#include "commonltev2x.h"
#include "tongjixieyiprcase.h"
#include <QDateTime>
#include <QProcess>
#include "inner_recv_manager.h"

#define Rtk_MAX_MSG_LENGTH 1024
#define PACKID_Condition_MAX_VALUE 10000
#define  Timer_Period 50

typedef struct RecvRtk_msg
{
    char p_mcu_msg[Rtk_MAX_MSG_LENGTH];
    int len ;
}RecvRtk_msg;


class GpsAnalyse :public QObject
{
    Q_OBJECT
public:
    GpsAnalyse(UdpComManager* udpcom,UdpComManager* udp_Tongji,inner_recv_manager* recv_manger,LtevMsgSend*com_ltev);
    ~GpsAnalyse();

    void  RecvListInsert(char *buf,int len);
    void Init();
    void   ConstructJsonString(char* buf);
    void   ConstructJsonString_toUDP(char* buf);

    RtkPacketControl* SetRtkPacketControl(RtkPacketControl* value)
    {
        rtkpackcontrol = value;
    }
    int  SetCardID(int value)
    {

        CardID = value;
    }

private:
    int packId;
    TcpServer* tcp_new;
   QThread * gprs_thrd;
    RecvList  recv_list;
    RtkPacketControl * rtkpackcontrol;
    int CardID =1;
    char buff_temp[Rtk_MAX_MSG_LENGTH];
    TongjiXieyiPrcase *tongji;
    QTimer * timer_recv;
    QQueue<char *> *rtk_msg_queue;
    QString  time_str=NULL;
    bool  time_Control ;
    QTimer*  time_flash;
private:
    void process_recv_msg();
    void do_process_recv_msg(char *buff_data);
    void getstimeval();
    int GetComma(int num,char *str);
     bool modify_os_time();

public slots:
    void accept_net_msg(char *buf,int len);
    void get_process_msg();
    void SendCanMessage();
    void gps_analyse(char *buf,int len_packet);

   void  system_time_flash();

signals:
    void   SendToLtev(char *buf,int pack_len);
    void   Gps_SendToUdp(char *buf,int pack_len);
    void   SendToTongji(char *buf,int pack_len);
    void   SendToCan(can_msg * canmsg);
    void   InsertToRtkSelf(char* buf,int len);
};


#endif // GPSANALYSE_H
