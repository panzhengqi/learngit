#ifndef INCLUDES_H
#define INCLUDES_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <math.h>

#include <QProcess>
#include <QQueue>
#include <QTimer>
#include <QThread>
#include <iostream>
#include <math.h>
#include "list"
#include "com_protocol.h"
#include "inner_protocol.h"
#include "../JasonControl/json/json.h"

using namespace std;


#define   Car_Type 0
#define   Car_IsSelf 1
#define   Car_IsNoSelf 0
#define   Car_Front_Info 3
#define   Car_ObuControl_Info 4
#define   Car_ObuFault_Info 5
#define    VoiceGuid_INFO 6
#define MAX_CAN_MSG_LENGTH      1024
#define CAN_INTERFACE          "can0"

typedef list<char *> RecvList;
typedef unsigned int UINT;
typedef int BYTE;
typedef unsigned char U8;
typedef struct{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int tv_ms;
}DATE_TIME;


typedef struct _gprmc_
{
    UINT time; /*gps 定位时间*/
    char pos_state; /*gps定位状态*/
    float latitude;/* 维度*/
    float longitude;/*经度*/
    float speed;/*速度*/
    float direction;/*航向*/
    UINT  date;/*日期*/
    float declinationl;/*磁偏角*/
    char dd;
    char mode;/*GPS模式*/
}GPRMC;

typedef  struct KSXT_STRU
{
        DATE_TIME sate_time;    //格式为yyyymmddhhmmss.ss，如2016040106284180表示2016年4月1日06时28分41.80秒
        double latitude;        //经度,0.00000000,度,小数点后8位，单位：度
        double longitude;       //纬度,0.00000000,度,小数点后8位，单位：度
        double altitude;        //high ,0.0000,m,小数点后4位，单位：米
        float azimuth_angle;    //0.00~360.00,前后天线连线与正北方向夹角（前天线为方向，后天线为位置），范围0°～360°，小数点后2位
        float pitch_angle;      //-90.00~90.00,范围-90°～90°，小数点后2位
        float speed_angle;      //0.00~360.00,范围-90°～90°，小数点后2位
        float speed;            //速度,0.000,km/h,车辆行进方向速度，小数点后3位，单位：km/h
        float roll_angle;       //-90.00~90.00,范围-90°～90°，小数点后2位
        char  sate_pos_state;   //0表示未定位，1表示单点定位，2表示RTK浮点解，3表示RTK固定解
        char  sate_orien_state; //0表示未定向，1表示单点定向，2表示RTK浮点解，3表示RTK固定解
        int   front_ant_avail_star_num;//前天线当前参与解算的卫星数量
        int   post_ant_avail_star_num;//后天线当前参与解算的卫星数量
        double east_pos_coor;       //以基站为原点的地理坐标系下的东向位置，单位为m，小数点后3位（如无为空）
        double north_pos_coor;      //以基站为原点的地理坐标系下的北向位置，单位为m，小数点后3位（如无为空）
        double sky_pos_coor;        //以基站为原点的地理坐标系下的天向位置，单位为m，小数点后3位（如无为空）
        double east_speed;          //地理坐标系下的东向速度，小数点后3位，单位：km/h（如无为空）
        double north_speed;         //地理坐标系下的北向速度，小数点后3位，单位：km/h（如无为空）
        double sky_speed;           //地理坐标系下的天向速度，小数点后3位，单位：km/h（如无为空）
}KSXT_STRU;

typedef  struct data{
    double latitude; //经度
     double longitude; //纬度
    int latitude_Degree;    //度
    int        latitude_Cent;   //分
    int     latitude_Second; //秒
    int longitude_Degree;    //度
    int       longitude_Cent;  //分
    int     longitude_Second; //秒
    float     speed; //速度
    float     direction; //航向
    float     height; //海拔高度
    int satellite;
    U8     NS;
    U8     EW;
    DATE_TIME D;
}GPS_INFO;

#endif // INCLUDES_H
