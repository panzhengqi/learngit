#ifndef TONGJIXIEYIPRCASE_H
#define TONGJIXIEYIPRCASE_H

#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <QTimer>
#include "../JasonControl/json/json.h"
#include  "gpsprotocolparser.h"

using namespace std;

#define    BUFFMATH  1024
#define     uchar unsigned char
#define     ushort  unsigned short


#define CRC16_INITIAL_SEED   0xFFFF
#define MSGTongji_ToAndriod_LENGTH 1024

typedef struct RecvMSgPacket
{
    int type;
    int len ;
    char* p_mcu_msg;
}RecvMSgPacket;

const ushort crc16_x25_table[]=
{
  0x0000, 0x1081, 0x2102, 0x3183,
  0x4204, 0x5285, 0x6306, 0x7387,
  0x8408, 0x9489, 0xa50a, 0xb58b,
  0xc60c, 0xd68d, 0xe70e, 0xf78f
};

typedef  struct MsgPacket
{
    uchar  packHead = 0x7E;
    uchar MsgType;
   ushort datalen;
    //char   data[datalen];
    char * data;
    ushort  checkSum;
    uchar Packend = 0xFF;

}Msg_packet;

typedef struct{

    uchar VoiceGuide_num; // 0 bukongzhi  1 wei qingqiu kongzhi

}IMC2OBU_MSG_VoiceGuid;

typedef struct{

    uchar EHB_CTRL_Enablel; // 0 bukongzhi  1 wei qingqiu kongzhi
    uchar EHB_Ctrl_value;//fenbianlv 0.4%

}IMC2OBU_MSG_EHNCRTL;

typedef  struct Tongji_to_ObuCar_ControlPacket
{
    uchar  packHead = 0xA7;
    uchar MsgType;
    ushort datalen;
    char * data;
    ushort  checkSum;
    uchar Packend = 0x7A;
}Tongji_to_ObuCar_ControlPacket;

class TongjiXieyiPrcase : public QObject
{
    Q_OBJECT
public:
    explicit TongjiXieyiPrcase(QObject *parent = 0);
    void Init();
    char*  InCodePacket(char * buf ,int *len,uchar msgType);// zubao
  //  int   InCodePacket(char * buf ,int len);// zubao
    int     DeCodePacket(char * buf ,int len,uchar msgType);// zubao
    int     DeCodePacket(RecvMSgPacket *recvmsg);
    int     DeCodePacket(char * buf ,int len);// zubao
    void   IntBigDuan(int value,char* temp);
    void   UshortBigDuan(ushort value,char* temp);
    void  IntSmallDuan(int value,char* temp);
    void  UshortSmallDuan(ushort value,char* temp);
    ushort crc16_x25(uchar *buf, ushort buflen);
    char * ConstructJsonString(char data,int* len);
    char CovertHexChar(char ch);
    void StringToHex(QString str, QByteArray &senddata);
 };

#endif // TONGJIXIEYIPRCASE_H
