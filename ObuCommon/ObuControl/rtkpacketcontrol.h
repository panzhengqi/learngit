#ifndef RTKPACKETCONTROL_H
#define RTKPACKETCONTROL_H

#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <QThread>
#include <QTimer>
#include <QQueue>
#include "list"
#include "gpsprotocolparser.h"
#include "inner_recv_manager.h"
#include "map"
#include "../CommonLog/log.h"
#include  <QQueue>

using namespace std;

#define  rtk_msg_Len 1024
#define  Distance_Speed_Len  256
typedef  QQueue<char *>    Car_RtkPacket;
//typedef multimap<int,GPS_INFO*> MultiMap_packet;
class RtkPacketControl:public QObject
{
    Q_OBJECT
public:
    RtkPacketControl(inner_recv_manager* recv_manger);
    ~RtkPacketControl();

private:
    Car_RtkPacket *     car_rtk_Packet;
//    MultiMap_packet*  multimap_packet;
    Car_RtkPacket *     car_rtk_Packet_Front;
//    QThread *           rtkpacket_thrd;
    GpsProtocolParser*  gpspro_parser;
    int condition = 0;
    QTimer * rtkPacket_Control_timer;
public:
    RtkPacketControl* GetRtkObject()
    {
        return this;
    }
private:
    void RtkPacketSend();

public:

    void  ConstructJsonString( double distance,float speed,char *buf);
//    void RecvMsgInsert_Map(int key ,GPS_INFO *gps);
private slots:
 //   void   RtkSendToCanMsgControlThread();
    void   RtkPackControlThread();
    void   RecvListInsert(char *buf,int len);
    void   RecvListInsert_Front(char *buf);
signals:
    void SendToCarFrontMsgCan(can_msg *can_msg);
public:
    RtkPacketControl* GetRtkPacketControl()
    {
        return this;
    }
};

#endif // RTKPACKETCONTROL_H
