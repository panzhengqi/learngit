#include "inner_recv_manager.h"

inner_recv_manager::inner_recv_manager(QObject *parent) :
    QObject(parent)
{
    p_inner_recv_queue   = new QQueue<can_msg *>;
    p_recv_check_timer = new QTimer(this);
    connect((QObject *)p_recv_check_timer, SIGNAL(timeout()), this, SLOT(check_process_recv_msg()));
    p_recv_check_timer->start(INNER_RECV_CHK_PERIOD);
}


inner_recv_manager::~inner_recv_manager()
{
    if(p_recv_check_timer){
        if(p_recv_check_timer->isActive()){
            p_recv_check_timer->stop();
        }
        delete p_recv_check_timer;
        p_recv_check_timer = NULL;
    }
    if(p_inner_recv_queue){
        if(!p_inner_recv_queue->isEmpty()){
            p_inner_recv_queue->clear();
        }
        delete p_inner_recv_queue;
        p_inner_recv_queue =  NULL;
    }
}


void inner_recv_manager::process_recv_msg(can_msg *p_can_msg)
{
    string p_msg = p_can_msg->msg;
  //  qDebug("process_recv_msg: msg->%s", p_can_msg->msg);
    Json::Reader reader;
    Json::Value  p_packet;
    if(reader.parse(p_msg, p_packet)){
        string Msg_type     = p_packet["type"].asString();
        Json::Value content = p_packet["data"];
//        qDebug("process_recv_msg: type -> %s", Msg_type.data());
        if(!Msg_type.compare(INNER_MSG_TYPE_RTK)){
            string str_isSelf   = content["isSelf"].asString();
            if(!str_isSelf.compare(IS_SELF_YES)){
//                qDebug("process_recv_msg: receice a valid RTK message: %s", p_can_msg->msg);
                string str_RTK_info = content["extra"].asString();
                process_rtk_msg(str_RTK_info);
            }
        }
        else if(!Msg_type.compare(INNER_MSG_TYPE_RGY)){
            qDebug("process_recv_msg: receice a valid RGY message: %s", p_can_msg->msg);
            string light_color = content["statue"].asString();
            string light_count = content["time"].asString();
            process_rgy_msg(light_color, light_count);
        }
        else if(!Msg_type.compare(INNER_MSG_TYPE_OBU_FAULT)){
            string obu_fault = content["fault"].asString();
            process_obu_fault_msg(obu_fault);
        }
        else if(!Msg_type.compare(INNER_MSG_TYPE_F_VEHICLE)){

            string front_distance = content["distance"].asString();
            string front_speed    = content["speed"].asString();
            process_obu_vfront_msg(front_distance, front_speed);
        }
        else if(!Msg_type.compare(INNER_MSG_TYPE_CONTROL)){
            string wc_brake = content["wc_brake_per"].asString();
            string sig_val  = content["signal_5g"].asString();
            process_obu_control_msg(wc_brake, sig_val);
        }
        else{
            qDebug("process_recv_msg: unrecogonise this msg...");
        }
        /* add other type msg here... */
    }
    else{
        qDebug("process_recv_msg: parse message failed...");
    }
}


void inner_recv_manager::process_rtk_msg(string str_msg)
{
    KSXT_STRU gps_info;
    memset(&gps_info, 0, sizeof(KSXT_STRU));
    analize_rtk_msg((char *)str_msg.data(), &gps_info);
//    qDebug("GPS_INFO: latitude: %.10f, longitude: %.10f, gps_speed: %f. heading_angle: %f, %d,-%d,-%d, %d,-%d,-%d ,-%d",
//           gps_info.latitude, gps_info.longitude, gps_info.speed, gps_info.direction, gps_info.D.year, gps_info.D.month, gps_info.D.day, gps_info.D.hour, gps_info.D.minute, gps_info.D.second, gps_info.D.tv_ms);
    obu_can_info_update *p_can_info = new obu_can_info_update;
    memset(p_can_info, 0, sizeof(obu_can_info_update));
    p_can_info->update_command_type = OBU_MODIFY_GPS_INFO;
    p_can_info->obu_update_info.p_latitude_info.obu_RTK_latitude           = (gps_info.latitude / OBU_RTK_RESO);
    p_can_info->obu_update_info.p_longitude_info.obu_RTK_longitude         = (gps_info.longitude / OBU_RTK_RESO);
    p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_speed               = (unsigned short)(gps_info.speed / OBU_RTK_SPEED_RESO_0_1);
    p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_heading_angle       = (unsigned short)(gps_info.pitch_angle / OBU_RTK_HEADING_ANGLE_RESO_0_1);
    p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_year    = (quint8)(gps_info.sate_time.year - 2000);
    p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_mouth   = (quint8)gps_info.sate_time.month;
    p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_day     = (quint8)gps_info.sate_time.day;
    p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_hour    = (quint8)gps_info.sate_time.hour;
    p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_minute  = (quint8)gps_info.sate_time.minute;
    p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_second  = (quint8)gps_info.sate_time.second;
    p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_mil_sec = (quint16)gps_info.sate_time.tv_ms;
//    qDebug("latitude: %lld, longitude: %lld, gps_speed: %lu. heading_angle: %lu, %d-%d-%d, %d:%d:%d-%d",
//           p_can_info->obu_update_info.p_latitude_info.obu_RTK_latitude,\
//           p_can_info->obu_update_info.p_longitude_info.obu_RTK_longitude,\
//           p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_speed,\
//           p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_heading_angle,\
//           p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_year,\
//           p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_mouth,\
//           p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_day,\
//           p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_hour,
//           p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_minute,
//           p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_second,
//           p_can_info->obu_update_info.p_obu_time_sync_info.obu_sync_time_mil_sec);
    emit send_can_packet(p_can_info);
}


void inner_recv_manager::process_rgy_msg(string str_color, string str_time)
{
    QString light_color = str_color.data();
    QString light_time  = str_time.data();
    obu_can_info_update *p_can_info = new obu_can_info_update;
    memset(p_can_info, 0, sizeof(obu_can_info_update));
    p_can_info->update_command_type = OBU_MODIFY_LIGHT_STATE;
    if(0 == light_color.indexOf(RSU_LIGHT_COLOR_UNKNOW)){
        p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_state = OBU_RGY_STATE_NULL;
        p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_time  = OBU_RGY_NULL_TIME;
    }
    else if(0 == light_color.indexOf(RSU_LIGHT_COLOR_GREEN)){
        p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_state = OBU_RGY_STATE_G;
        p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_time  = (unsigned char)light_time.toUInt();
    }
    else if(0 == light_color.indexOf(RSU_LIGHT_COLOR_RED)){
        p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_state = OBU_RGY_STATE_R;
        p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_time  = (unsigned char)light_time.toUInt();
    }
    else if(0 == light_color.indexOf(RSU_LIGHT_COLOR_YELLOW)){
        p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_state = OBU_RGY_STATE_Y;
        p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_time  = (unsigned char)light_time.toUInt();
    }
    else{
        qDebug("process_rgy_msg: failed, unknow Traffic light color...");
        if(p_can_info){
            delete p_can_info;
            p_can_info = NULL;
        }
        return;
    }
    qDebug("light: %d, time: %d", p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_state,\
           p_can_info->obu_update_info.p_obu_rgy_info.obu_RGY_time);
    emit send_can_packet(p_can_info);
}


void inner_recv_manager::process_obu_fault_msg(string fault)
{
    QString str_fault = fault.data();
    obu_can_info_update *p_can_info = new obu_can_info_update;
    memset(p_can_info, 0, sizeof(obu_can_info_update));
    p_can_info->update_command_type = OBU_MODIFY_FAULT_INFO;
    if(0 == str_fault.indexOf(OBU_FAULT_NORMAL)){
        p_can_info->obu_update_info.p_obu_rgy_info.obu_Fault_info = OBU_FAULT_INFO_NORMAL;
    }
    else if(0 == str_fault.indexOf(OBU_FAULT_OTHER)){
        p_can_info->obu_update_info.p_obu_rgy_info.obu_Fault_info = OBU_FAULT_INFO_OTHER;
    }
    else{
        qDebug("process_obu_fault_msg: Unsupport OBU fault info...");
        if(p_can_info){
            delete p_can_info;
            p_can_info = NULL;
        }
        return;
    }
    emit send_can_packet(p_can_info);
}


void inner_recv_manager::process_obu_vfront_msg(string dist, string spe)
{
    QString distance = dist.data();
    QString speed    = spe.data();
    obu_can_info_update *p_can_info = new obu_can_info_update;
    memset(p_can_info, 0, sizeof(obu_can_info_update));
    p_can_info->update_command_type = OBU_MODIFY_VFRONT_INFO;
    p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_front_distance = (unsigned short)(distance.toFloat()/OBU_RTK_FRONT_DIST_RESO_0_1);
    p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_front_speed    = (char)(speed.toInt()/OBU_RTK_FRONT_SPEED_RESO_1);
    p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_front_speed    += OBU_RTK_FRONT_SPEED_OFFSET;
    qDebug("inner_recv_manager::process_obu_vfront_msg: %d, %d", p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_front_distance,
           p_can_info->obu_update_info.p_obu_rtk_info.obu_RTK_front_speed);
    emit send_can_packet(p_can_info);
}


void inner_recv_manager::process_obu_control_msg(string wc_brake_percent, string signal_valid)
{
    QString wc_brake = wc_brake_percent.data();
    QString sig_val  = signal_valid.data();
    obu_can_info_update *p_can_info = new obu_can_info_update;
    memset(p_can_info, 0, sizeof(obu_can_info_update));
    p_can_info->update_command_type = OBU_MODIFY_CONTROL_INFO;
    p_can_info->obu_update_info.p_obu_control_info.obu_Con_WC_brake_percent = (unsigned char)(wc_brake.toFloat()/OBU_CONT_WC_BRAKE_RESO_0_4);
    p_can_info->obu_update_info.p_obu_control_info.obu_Con_5G_signal_state  = (unsigned char)(sig_val.toUInt());
    emit send_can_packet(p_can_info);
}
int inner_recv_manager::analize_rtk_msg(char *line, KSXT_STRU *p_gps_info)
{
    U8 ch;
    char *buf = line;
    ch = buf[4];
    if (ch == 'T') //如果第五个字符是T，($KSXT)
    {
        if(17 == DetectionDataValidity(1,buf))
        {
            p_gps_info->sate_time.year = (buf[6] - '0')*1000 + (buf[7] - '0')*100 + (buf[8] - '0')*10 + (buf[9] - '0');
            p_gps_info->sate_time.month = (buf[10] - '0')*10 + (buf[11] - '0');
            p_gps_info->sate_time.day = (buf[12] - '0')*10 + (buf[13] - '0');
            p_gps_info->sate_time.hour = (buf[14] - '0')*10 + (buf[15] - '0');
            p_gps_info->sate_time.minute = (buf[16] - '0')*10 + (buf[17] - '0');
            p_gps_info->sate_time.second = (buf[18] - '0')*10 + (buf[19] - '0');
            p_gps_info->sate_time.tv_ms = (buf[21] - '0')*100 + (buf[22] - '0')*10;
        }else
        {
            p_gps_info->sate_time.year = 0;
            p_gps_info->sate_time.month = 0;
            p_gps_info->sate_time.day = 0;
            p_gps_info->sate_time.hour = 0;
            p_gps_info->sate_time.minute = 0;
            p_gps_info->sate_time.second = 0;
            p_gps_info->sate_time.tv_ms = 0;
        }

        p_gps_info->latitude = get_double_number(&buf[GetComma(2, buf)]);
        p_gps_info->longitude = get_double_number(&buf[GetComma(3, buf)]);
        p_gps_info->altitude = get_double_number(&buf[GetComma(4, buf)]);
        p_gps_info->azimuth_angle = get_float_number(&buf[GetComma(5, buf)]);
        p_gps_info->pitch_angle = get_float_number(&buf[GetComma(6, buf)]);
        p_gps_info->speed_angle = get_float_number(&buf[GetComma(7, buf)]);
        p_gps_info->speed = get_float_number(&buf[GetComma(8, buf)]);
        p_gps_info->roll_angle = get_float_number(&buf[GetComma(9, buf)]);
        p_gps_info->sate_pos_state = buf[GetComma(10, buf)];
        p_gps_info->sate_orien_state = buf[GetComma(11, buf)];

        p_gps_info->front_ant_avail_star_num = get_int_number(&buf[GetComma(12, buf)]);
        p_gps_info->post_ant_avail_star_num = get_int_number(&buf[GetComma(13, buf)]);
        p_gps_info->east_pos_coor = get_double_number(&buf[GetComma(14, buf)]);
        p_gps_info->north_pos_coor = get_double_number(&buf[GetComma(15, buf)]);
        p_gps_info->sky_pos_coor = get_double_number(&buf[GetComma(16, buf)]);
        p_gps_info->east_speed = get_double_number(&buf[GetComma(17, buf)]);
        p_gps_info->north_speed = get_double_number(&buf[GetComma(18, buf)]);
        p_gps_info->sky_speed = get_double_number(&buf[GetComma(19, buf)]);
    }

    return 0;
}


int inner_recv_manager::DetectionDataValidity(int num,char *str)
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
int inner_recv_manager::GetComma(int num,char *str)
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
double inner_recv_manager::get_double_number(char *s)
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

float  inner_recv_manager::get_float_number(char *s)
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

int inner_recv_manager::get_int_number(char *s)
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

double inner_recv_manager::get_locate(double temp)
{
    int m;
    double  n;
    m=(int)temp/100;
    n=(temp-m*100)/60;
    n=n+m;
    return n;
}

void   inner_recv_manager::UTC2BTC(DATE_TIME * gps_data)
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





/*
void inner_recv_manager::analize_rtk_msg(char *msg, GPS_INFO *p_gps_info)
{
    U8 ch, status, tmp;
    float lati_cent_tmp, lati_second_tmp;
    float long_cent_tmp, long_second_tmp;
    float speed_tmp;
    char *buf = msg;
    ch = buf[5];
    status = buf[GetComma(2, buf)];

    if (ch == 'C')
    {
        if (status == 'A')
        {
            p_gps_info->NS = buf[GetComma(4, buf)];
            p_gps_info->EW = buf[GetComma(6, buf)];

            p_gps_info->latitude = get_double_number(&buf[GetComma(3, buf)]);
            p_gps_info->longitude = get_double_number(&buf[GetComma(5, buf)]);

            p_gps_info->latitude_Degree = (int)p_gps_info->latitude / 100;
            lati_cent_tmp = (p_gps_info->latitude - p_gps_info->latitude_Degree * 100);
            p_gps_info->latitude_Cent = (int)lati_cent_tmp;
            lati_second_tmp = (lati_cent_tmp - p_gps_info->latitude_Cent) * 60;
            p_gps_info->latitude_Second = (int)lati_second_tmp;
            p_gps_info->latitude =   p_gps_info->latitude_Degree +  (p_gps_info->latitude - p_gps_info->latitude_Degree * 100)/60;

            p_gps_info->longitude_Degree = (int)p_gps_info->longitude / 100;
            long_cent_tmp = (p_gps_info->longitude - p_gps_info->longitude_Degree * 100);
            p_gps_info->longitude_Cent = (int)long_cent_tmp;
            long_second_tmp = (long_cent_tmp - p_gps_info->longitude_Cent) * 60;
            p_gps_info->longitude_Second = (int)long_second_tmp;

            p_gps_info->longitude = p_gps_info->longitude_Degree+(p_gps_info->longitude - p_gps_info->longitude_Degree * 100)/60;

            speed_tmp = get_float_number(&buf[GetComma(7, buf)]);
            p_gps_info->speed = speed_tmp * 1.85;
            p_gps_info->direction = get_float_number(&buf[GetComma(8, buf)]);

            p_gps_info->D.hour = (buf[7] - '0') * 10 + (buf[8] - '0');
            p_gps_info->D.minute = (buf[9] - '0') * 10 + (buf[10] - '0');
            p_gps_info->D.second = (buf[11] - '0') * 10 + (buf[12] - '0');
            p_gps_info->D.tv_ms = (buf[14] - '0') * 100 + (buf[15] - '0')*10;
            tmp = GetComma(9, buf);
            p_gps_info->D.day = (buf[tmp + 0] - '0') * 10 + (buf[tmp + 1] - '0');
            p_gps_info->D.month = (buf[tmp + 2] - '0') * 10 + (buf[tmp + 3] - '0');
            p_gps_info->D.year = (buf[tmp + 4] - '0') * 10 + (buf[tmp + 5] - '0') + 2000;
            UTC2BTC(&p_gps_info->D);
        }
    }
    if(ch == 'A')
    {
        p_gps_info->height = get_double_number(&buf[GetComma(9,buf)]);
    }
}


int inner_recv_manager::GetComma(int num, char *str)
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
            return i+1;
    }
    return 0;
}


double inner_recv_manager::get_double_number(char *s)
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


float inner_recv_manager::get_float_number(char *s)
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


void inner_recv_manager::UTC2BTC(DATE_TIME * gps_data)
{
//***************************************************
//如果秒号先出,再出时间数据,则将时间数据+1秒
    //gps_data->tv_ms;
    if(gps_data->tv_ms>999)
    {
        gps_data->second++;
        gps_data->tv_ms=0;
    }
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
    gps_data->hour+=8;
    if(gps_data->hour>23)
    {
        gps_data->hour-=24;
        gps_data->day+=1;
        if(gps_data->month==2 ||gps_data->month==4 ||gps_data->month==6 ||gps_data->month==9 ||gps_data->month==11 )
        {
            if(gps_data->day>30)
            {
                gps_data->day=1;
                gps_data->month++;
            }
        }
        else
        {
            if(gps_data->day>31)
            {
                gps_data->month++;
            }
        }
        if(gps_data->year % 4 == 0 )
        {
            if(gps_data->day > 29 && gps_data->month ==2){
                gps_data->month++;
            }
        }
        else
        {
            if(gps_data->day>28 &&gps_data->month ==2){
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


*/
void inner_recv_manager::accept_inner_msg(can_msg *p_can_msg)
{
    p_inner_recv_queue->enqueue(p_can_msg);
}


void inner_recv_manager::check_process_recv_msg()
{
//    string temp = "40.123456";
//    process_rtk_msg(temp);
    if(!p_inner_recv_queue->isEmpty()){
        can_msg *p_can_msg = p_inner_recv_queue->dequeue();
        process_recv_msg(p_can_msg);
        if(p_can_msg){
            delete p_can_msg;
            p_can_msg = NULL;
        }
    }
}



