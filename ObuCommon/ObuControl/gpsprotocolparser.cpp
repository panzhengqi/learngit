#include "gpsprotocolparser.h"


using namespace std;

GpsProtocolParser::GpsProtocolParser(QObject *parent) :
    QObject(parent)
{
}

//对应int32大小的成员 的转换 范例
int   GpsProtocolParser::swapInt32(int value)
{
    return ((value & 0x000000FF) << 24) |
                 ((value & 0x0000FF00) << 8) |
                 ((value & 0x00FF0000) >> 8) |
                 ((value & 0xFF000000) >> 24) ;
}

void  GpsProtocolParser::ConstructJsonString_toRtkPacket(char* buf,KSXT_STRU *GPS)
{
    Json::Value packet;
    Json::Value packet_data;

    packet["id"]  = Json::valueToString(condition);
    packet["type"] = Json::valueToString(Car_Type);
    packet_data["isSelf"] = Json::valueToString(Car_IsSelf);
    packet_data["extra"] = Json::Value(buf);
    packet["data"] = packet_data;
    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    memset(buf,0,1024);
    memcpy(buf, str_packet.c_str(), str_packet.length());

}

int GpsProtocolParser::GPS_KSXT_Parse(char *line, KSXT_STRU *KSXT)
{
    U8 ch;
    char *buf = line;
    ch = buf[4];
    if (ch == 'T') //如果第五个字符是T，($KSXT)
    {
        if(17 == DetectionDataValidity(1,buf))
        {
            KSXT->sate_time.year = (buf[6] - '0')*1000 + (buf[7] - '0')*100 + (buf[8] - '0')*10 + (buf[9] - '0');
            KSXT->sate_time.month = (buf[10] - '0')*10 + (buf[11] - '0');
            KSXT->sate_time.day = (buf[12] - '0')*10 + (buf[13] - '0');
            KSXT->sate_time.hour = (buf[14] - '0')*10 + (buf[15] - '0');
            KSXT->sate_time.minute = (buf[16] - '0')*10 + (buf[17] - '0');
            KSXT->sate_time.second = (buf[18] - '0')*10 + (buf[19] - '0');
            KSXT->sate_time.tv_ms = (buf[21] - '0')*100 + (buf[22] - '0')*10;
            UTC2BTC(&KSXT->sate_time);
        }else
        {
            KSXT->sate_time.year = 0;
            KSXT->sate_time.month = 0;
            KSXT->sate_time.day = 0;
            KSXT->sate_time.hour = 0;
            KSXT->sate_time.minute = 0;
            KSXT->sate_time.second = 0;
            KSXT->sate_time.tv_ms = 0;
        }

        KSXT->longitude = get_double_number(&buf[GetComma(2, buf)]);
        KSXT->latitude = get_double_number(&buf[GetComma(3, buf)]);
        KSXT->altitude = get_double_number(&buf[GetComma(4, buf)]);
        KSXT->azimuth_angle = get_float_number(&buf[GetComma(5, buf)]);
        KSXT->pitch_angle = get_float_number(&buf[GetComma(6, buf)]);
        KSXT->speed_angle = get_float_number(&buf[GetComma(7, buf)]);
        KSXT->speed = get_float_number(&buf[GetComma(8, buf)]);
        KSXT->roll_angle = get_float_number(&buf[GetComma(9, buf)]);
        KSXT->sate_pos_state = buf[GetComma(10, buf)];
        KSXT->sate_orien_state = buf[GetComma(11, buf)];

        KSXT->front_ant_avail_star_num = get_int_number(&buf[GetComma(12, buf)]);
        KSXT->post_ant_avail_star_num = get_int_number(&buf[GetComma(13, buf)]);
        KSXT->east_pos_coor = get_double_number(&buf[GetComma(14, buf)]);
        KSXT->north_pos_coor = get_double_number(&buf[GetComma(15, buf)]);
        KSXT->sky_pos_coor = get_double_number(&buf[GetComma(16, buf)]);
        KSXT->east_speed = get_double_number(&buf[GetComma(17, buf)]);
        KSXT->north_speed = get_double_number(&buf[GetComma(18, buf)]);
        KSXT->sky_speed = get_double_number(&buf[GetComma(19, buf)]);
    }

    return 0;
}

/*
int   GpsProtocolParser::GPS_RMC_Parse(char *line, GPS_INFO *GPS)
{
    U8 ch, status, tmp;
    float lati_cent_tmp, lati_second_tmp;
    float long_cent_tmp, long_second_tmp;
    float speed_tmp;
    char *buf = line;
    ch = buf[5];
    status = buf[GetComma(2, buf)];

    if (ch == 'C') //如果第五个字符是C，($GPRMC)
    {
        if (status == 'A') //如果数据有效，则分析
        {
            GPS->NS = buf[GetComma(4, buf)];
            GPS->EW = buf[GetComma(6, buf)];

            GPS->latitude = get_double_number(&buf[GetComma(3, buf)]);
            GPS->longitude = get_double_number(&buf[GetComma(5, buf)]);

            GPS->latitude_Degree = (int)GPS->latitude / 100; //分离纬度
            lati_cent_tmp = (GPS->latitude - GPS->latitude_Degree * 100);
            GPS->latitude_Cent = (int)lati_cent_tmp;
            lati_second_tmp = (lati_cent_tmp - GPS->latitude_Cent) * 60;
            GPS->latitude_Second = (int)lati_second_tmp;
            GPS->latitude = GPS->latitude_Degree+(GPS->latitude - GPS->latitude_Degree * 100)/60;

            GPS->longitude_Degree = (int)GPS->longitude / 100;    //分离经度
            long_cent_tmp = (GPS->longitude - GPS->longitude_Degree * 100);
            GPS->longitude_Cent = (int)long_cent_tmp;
            long_second_tmp = (long_cent_tmp - GPS->longitude_Cent) * 60;
            GPS->longitude_Second = (int)long_second_tmp;
            GPS->longitude = GPS->longitude_Degree+ (GPS->longitude - GPS->longitude_Degree * 100)/60;

            speed_tmp = get_float_number(&buf[GetComma(7, buf)]); //速度(单位：海里/时)
            GPS->speed = speed_tmp * 1.85; //1海里=1.85公里
            GPS->direction = get_float_number(&buf[GetComma(8, buf)]); //角度

            GPS->D.hour = (buf[7] - '0') * 10 + (buf[8] - '0');        //时间
            GPS->D.minute = (buf[9] - '0') * 10 + (buf[10] - '0');
            GPS->D.second = (buf[11] - '0') * 10 + (buf[12] - '0');
            GPS->D.tv_ms = (buf[14] - '0') * 100 + (buf[15] - '0')*10;
            tmp = GetComma(9, buf);
            GPS->D.day = (buf[tmp + 0] - '0') * 10 + (buf[tmp + 1] - '0'); //日期
            GPS->D.month = (buf[tmp + 2] - '0') * 10 + (buf[tmp + 3] - '0');
            GPS->D.year = (buf[tmp + 4] - '0') * 10 + (buf[tmp + 5] - '0') + 2000;
            UTC2BTC(&GPS->D);
            return 1;
        }
    }
    if(ch=='A')//"$GPGGA"
    {
        GPS->height = get_double_number(&buf[GetComma(9,buf)]);

    }

    return 0;
}*/

int GpsProtocolParser::DetectionDataValidity(int num,char *str)
{
    bool controlflag = true;
    int i,j=0,v1=0,v2=0;
    int len=strlen(str);
    for(i=0;i<len;i++)
    {
        if(str[i]==',')
        {
             j++;
        }

        if(j==num && controlflag)
        {
             v1 = i+1;
             controlflag = false;
        }

        if(j==num+1)
        {
            v2 = i;
            return v2 - v1;
        }

    }
    return v2-v1;
}

//得到指定序号的逗号位置
int GpsProtocolParser::GetComma(int num,char *str)
{
    int i,j=0;
    int len=strlen(str);
    for(i=0;i<len;i++)
    {
        if(str[i]==',')
        {
             j++;
        }
        if(j==num)
        {
            if(i+1 < len)
            {
                return i+1;
            }else
            {
                return 0;
            }

        }
    }
    return 0;
}

//将获取文本信息转换为double型
double GpsProtocolParser::get_double_number(char *s)
{
    char buf[128];
    int i;
    double rev;
    i=GetComma(1,s);
    strncpy(buf,s,i);
    buf[i]=0;
    rev=atof(buf);
    return rev;
}

float  GpsProtocolParser::get_float_number(char *s)
{
    char buf[128];
    int i;
    float rev;
    i=GetComma(1,s);
    strncpy(buf,s,i);
    buf[i]=0;
    rev=atof(buf);
    return rev;

}

int GpsProtocolParser::get_int_number(char *s)
{
    char int_number_buf[128];
    int i;
    int rev;
    i=GetComma(1,s);
    strncpy(int_number_buf,s,i);
    int_number_buf[i-1]=0;
    rev=atoi(int_number_buf);
    return rev;
}

double GpsProtocolParser::get_locate(double temp)
{
    int m;
    double  n;
    m=(int)temp/100;
    n=(temp-m*100)/60;
    n=n+m;
    return n;
}

void   GpsProtocolParser::UTC2BTC(DATE_TIME * gps_data)
{
//***************************************************
//如果秒号先出,再出时间数据,则将时间数据+1秒
    gps_data->tv_ms;
    if(gps_data->tv_ms>999)
        gps_data->second++;
    //gps_data->second++; //加一秒
    if(gps_data->second>59)
    {
        gps_data->second=0;
        gps_data->minute++;
        if(gps_data->minute>59)
        {
            gps_data->minute=0;
            gps_data->hour++;
        }
    }
//***************************************************
    gps_data->hour+=8;        //北京时间跟UTC时间相隔8小时
    if(gps_data->hour>23)
    {
        gps_data->hour-=24;
        gps_data->day+=1;
        if(gps_data->month==2 ||gps_data->month==4 ||gps_data->month==6 ||gps_data->month==9 ||gps_data->month==11 )
        {
            if(gps_data->day>30)
            {          //上述几个月份是30天每月，2月份还不足30
                gps_data->day=1;
                gps_data->month++;
            }
        }
        else
        {
            if(gps_data->day>31)
            {          //剩下的几个月份都是31天每月
                gps_data->day=1;
                gps_data->month++;
            }
        }
        if(gps_data->year % 4 == 0 )
        {//
            if(gps_data->day > 29 && gps_data->month ==2){       //闰年的二月是29天
                gps_data->day=1;
                gps_data->month++;
            }
        }
        else
        {
            if(gps_data->day>28 &&gps_data->month ==2){      //其他的二月是28天每月
                gps_data->day=1;
                gps_data->month++;
            }
        }
        if(gps_data->month>12)
        {
            gps_data->month-=12;
            gps_data->year++;
        }
    }
}

double GpsProtocolParser::rad(double d)
{

     return d * M_PI / 180.0;
}

double GpsProtocolParser::GetDistance(double lat1, double lng1, double lat2, double lng2)
{
  double radLat1 = rad(lat1);
  double radLat2 = rad(lat2);
  double a = radLat1 - radLat2;
  double b = rad(lng1) - rad(lng2);

  double s = 2 * asin(sqrt(pow(sin(a/2),2) +cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
  s = s * EARTH_RADIUS;
  s = round(s * 10000) / 10000;
  return s*1000;
}
