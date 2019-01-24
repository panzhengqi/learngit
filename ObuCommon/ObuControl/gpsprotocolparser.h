#ifndef GPSPROTOCOLPARSER_H
#define GPSPROTOCOLPARSER_H

#include <QObject>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../Can/includes.h"
#include "../CommonLog/log.h"


const double EARTH_RADIUS = 6378.137;

class GpsProtocolParser : public QObject
{
    Q_OBJECT
public:
    explicit GpsProtocolParser(QObject *parent = 0);
public:
    //对应int32大小的成员 的转换 范例
    int    swapInt32(int32_t value);

    int    GetComma(int num,char *str);
    double get_locate(double temp);
    double get_double_number(char *s);

    float  get_float_number(char *s);

    void   UTC2BTC(DATE_TIME * gps_data);

    double rad(double d);
    double GetDistance(double lat1, double lng1, double lat2, double lng2);
    //int   GPS_RMC_Parse(char *line, GPS_INFO *GPS);
     int    GPS_KSXT_Parse(char *line, KSXT_STRU *GPS);
    //void  Cons//tructJsonString_toRtkPacket(char* buf,GPS_INFO *GPS);
    int   DetectionDataValidity(int num,char *str);
    int   get_int_number(char *s);
    void  ConstructJsonString_toRtkPacket(char* buf,KSXT_STRU *GPS);
private:
    int condition = 0;
};



#endif // GPSPROTOCOLPARSER_H
