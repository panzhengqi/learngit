#include "can_send_manager.h"


can_send_manager::can_send_manager(QObject *parent) :
    QObject(parent)
{
    p_send_timer      = new QTimer(this);
    timeout_count     = 0;
    memset(&obu_can_info_store, 0, sizeof(obu_can_info));
    connect((QObject *)p_send_timer, SIGNAL(timeout()), this, SLOT(send_can_data()));
    p_send_timer->start(PERIOD_10MS);
}


can_send_manager::~can_send_manager()
{
    if(p_send_timer){
        if(p_send_timer->isActive()){
            p_send_timer->stop();
        }
        delete p_send_timer;
        p_send_timer = NULL;
    }
}


void can_send_manager::update_obu_light_info(obu_can_info *p_obu_can_info)
{
    obu_can_info_store.p_obu_rgy_info.obu_RGY_state = p_obu_can_info->p_obu_rgy_info.obu_RGY_state;
    obu_can_info_store.p_obu_rgy_info.obu_RGY_time  = p_obu_can_info->p_obu_rgy_info.obu_RGY_time;
}


void can_send_manager::update_obu_rtk_info(obu_can_info *p_obu_can_info)
{
    obu_can_info_store.p_latitude_info.obu_RTK_latitude     = p_obu_can_info->p_latitude_info.obu_RTK_latitude;
    obu_can_info_store.p_longitude_info.obu_RTK_longitude   = p_obu_can_info->p_longitude_info.obu_RTK_longitude;
    obu_can_info_store.p_obu_rtk_info.obu_RTK_heading_angle = p_obu_can_info->p_obu_rtk_info.obu_RTK_heading_angle;
    obu_can_info_store.p_obu_rtk_info.obu_RTK_speed         = p_obu_can_info->p_obu_rtk_info.obu_RTK_speed;
    memcpy(&(obu_can_info_store.p_obu_time_sync_info), &(p_obu_can_info->p_obu_time_sync_info), sizeof(obu_Time_sync_info));
}


void can_send_manager::update_obu_fault_info(obu_can_info *p_obu_can_info)
{
    obu_can_info_store.p_obu_rgy_info.obu_Fault_info = p_obu_can_info->p_obu_rgy_info.obu_Fault_info;
}


void can_send_manager::update_obu_vfront_info(obu_can_info *p_obu_can_info)
{
    obu_can_info_store.p_obu_rtk_info.obu_RTK_front_distance = p_obu_can_info->p_obu_rtk_info.obu_RTK_front_distance;
    obu_can_info_store.p_obu_rtk_info.obu_RTK_front_speed    = p_obu_can_info->p_obu_rtk_info.obu_RTK_front_speed;
//    qDebug("can_send_manager::update_obu_vfront_info: %d, %d", obu_can_info_store.p_obu_rtk_info.obu_RTK_front_distance,
//           obu_can_info_store.p_obu_rtk_info.obu_RTK_front_speed);
}


void can_send_manager::update_obu_control_info(obu_can_info *p_obu_can_info)
{
    obu_can_info_store.p_obu_control_info.obu_Con_WC_brake_percent = p_obu_can_info->p_obu_control_info.obu_Con_WC_brake_percent;
    obu_can_info_store.p_obu_control_info.obu_Con_5G_signal_state  = p_obu_can_info->p_obu_control_info.obu_Con_5G_signal_state;
}


void can_send_manager::send_10ms_info()
{
    send_obu_control_info();
}


void can_send_manager::send_100ms_info()
{
    send_rtk_latitude_info();
    send_rtk_longitude_info();
    send_obu_light_info();
    send_obu_rtk_info();
}


void can_send_manager::send_1000ms_info()
{
    send_obu_time_sync_info();
}


void can_send_manager::send_obu_control_info()
{
    can_frame *p_control_can_frame               = new can_frame;
    memset(p_control_can_frame, 0xFF, sizeof(can_frame));
    p_control_can_frame->can_id                  = (OBU_CONT_INFO_ID | CAN_EFF_FLAG);
    p_control_can_frame->can_dlc = sizeof(obu_can_info_store.p_obu_control_info);
    obu_Control_info *p_obu_Control_info         = (obu_Control_info *)p_control_can_frame->data;
    p_obu_Control_info->obu_Version              = CURRENT_VERSION;
    p_obu_Control_info->obu_Con_WC_brake_percent = obu_can_info_store.p_obu_control_info.obu_Con_WC_brake_percent;
    p_obu_Control_info->obu_Con_5G_signal_state  = obu_can_info_store.p_obu_control_info.obu_Con_5G_signal_state;
    emit send_can_frame(p_control_can_frame);
}


void can_send_manager::send_rtk_latitude_info()
{
    can_frame *p_latitude_can_frame = new can_frame;
    memset(p_latitude_can_frame, 0, sizeof(can_frame));
    p_latitude_can_frame->can_id  = (OBU_RTK_LATITUDE_CAN_ID | CAN_EFF_FLAG);
//    qDebug("send a rtk_latitude frame,ID is: %x\n", p_latitude_can_frame->can_id);
    p_latitude_can_frame->can_dlc = sizeof(obu_can_info_store.p_latitude_info.obu_RTK_latitude);
    obu_RTK_latitude_info *p_obu_RTK_latitude_info = (obu_RTK_latitude_info *)p_latitude_can_frame->data;
    p_obu_RTK_latitude_info->obu_RTK_latitude = (obu_can_info_store.p_latitude_info.obu_RTK_latitude);
//    qDebug("send_rtk_latitude_info: latitude: %lld", p_obu_RTK_latitude_info->obu_RTK_latitude);
//    qDebug("send a bu_extra_vehicle_info frame,ID is: %x\n", p_latitude_can_frame->can_id);
    emit send_can_frame(p_latitude_can_frame);
}


void can_send_manager::send_rtk_longitude_info()
{
    can_frame *p_longitude_can_frame                 = new can_frame;
    memset(p_longitude_can_frame, 0, sizeof(can_frame));
    p_longitude_can_frame->can_id                    = (OBU_RTK_LONGITUDE_CAN_ID | CAN_EFF_FLAG);
//    qDebug("send a rtk_longitude frame,ID is: %x\n", p_longitude_can_frame->can_id);
    p_longitude_can_frame->can_dlc                   = sizeof(obu_can_info_store.p_longitude_info.obu_RTK_longitude);
    obu_RTK_longitude_info *p_obu_RTK_longitude_info = (obu_RTK_longitude_info *)p_longitude_can_frame->data;
//    qDebug("send_rtk_longitude_info:->obu_RTK_longitude: before-> %d", obu_can_info_store.p_longitude_info.obu_RTK_longitude);
    p_obu_RTK_longitude_info->obu_RTK_longitude      = (obu_can_info_store.p_longitude_info.obu_RTK_longitude);
//    qDebug("send_rtk_longitude_info:->obu_RTK_longitude: after-> %d", p_obu_RTK_longitude_info->obu_RTK_longitude );
//    qDebug("send a bu_extra_vehicle_info frame,ID is: %x\n", p_longitude_can_frame->can_id);
    emit send_can_frame(p_longitude_can_frame);
}


void can_send_manager::send_obu_light_info()
{
    can_frame *p_light_can_frame   = new can_frame;
    memset(p_light_can_frame, 0xFF, sizeof(can_frame));
    p_light_can_frame->can_id      = (OBU_RGY_INFO_ID | CAN_EFF_FLAG);
    p_light_can_frame->can_dlc     = sizeof(obu_can_info_store.p_obu_rgy_info);
    obu_RGY_info *p_obu_RGY_info   = (obu_RGY_info *)p_light_can_frame->data;
    p_obu_RGY_info->obu_Version    = CURRENT_VERSION;
    p_obu_RGY_info->obu_RGY_state  = obu_can_info_store.p_obu_rgy_info.obu_RGY_state;
    p_obu_RGY_info->obu_RGY_time   = obu_can_info_store.p_obu_rgy_info.obu_RGY_time;
    p_obu_RGY_info->obu_Fault_info = obu_can_info_store.p_obu_rgy_info.obu_Fault_info;
    emit send_can_frame(p_light_can_frame);
}


void can_send_manager::send_obu_rtk_info()
{
    can_frame *p_rtk_can_frame             = new can_frame;
    memset(p_rtk_can_frame, 0xFF, sizeof(can_frame));
    p_rtk_can_frame->can_id                = (OBU_RTK_INFO_ID | CAN_EFF_FLAG);
    p_rtk_can_frame->can_dlc               = sizeof(obu_can_info_store.p_obu_rtk_info);
    obu_RTK_info *p_obu_RTK_info           = (obu_RTK_info *)p_rtk_can_frame->data;
    p_obu_RTK_info->obu_Version            = CURRENT_VERSION;
    if(obu_can_info_store.p_obu_rtk_info.obu_RTK_front_distance){
        p_obu_RTK_info->obu_RTK_front_distance = (obu_can_info_store.p_obu_rtk_info.obu_RTK_front_distance);
    }

    if(obu_can_info_store.p_obu_rtk_info.obu_RTK_front_speed){
        p_obu_RTK_info->obu_RTK_front_speed    = obu_can_info_store.p_obu_rtk_info.obu_RTK_front_speed;
    }
    p_obu_RTK_info->obu_RTK_heading_angle  = (obu_can_info_store.p_obu_rtk_info.obu_RTK_heading_angle);
    p_obu_RTK_info->obu_RTK_speed          = (obu_can_info_store.p_obu_rtk_info.obu_RTK_speed);
//    qDebug("can_send_manager::send_obu_rtk_info: %d, %d", p_obu_RTK_info->obu_RTK_front_distance,
//           p_obu_RTK_info->obu_RTK_front_speed);
    emit send_can_frame(p_rtk_can_frame);
}


void can_send_manager::send_obu_time_sync_info()
{
    can_frame *p_time_can_frame                 = new can_frame;
    memset(p_time_can_frame, 0, sizeof(can_frame));
    p_time_can_frame->can_id                    = (OBU_TIME_SYNC_ID | CAN_EFF_FLAG);
    p_time_can_frame->can_dlc                   = sizeof(obu_can_info_store.p_obu_time_sync_info);
    obu_Time_sync_info *p_obu_Time_sync_info    = (obu_Time_sync_info *)p_time_can_frame->data;
    p_obu_Time_sync_info->obu_sync_time_year    = obu_can_info_store.p_obu_time_sync_info.obu_sync_time_year;
    p_obu_Time_sync_info->obu_sync_time_mouth   = obu_can_info_store.p_obu_time_sync_info.obu_sync_time_mouth;
    p_obu_Time_sync_info->obu_sync_time_day     = obu_can_info_store.p_obu_time_sync_info.obu_sync_time_day;
    p_obu_Time_sync_info->obu_sync_time_hour    = obu_can_info_store.p_obu_time_sync_info.obu_sync_time_hour;
    p_obu_Time_sync_info->obu_sync_time_minute  = obu_can_info_store.p_obu_time_sync_info.obu_sync_time_minute;
    p_obu_Time_sync_info->obu_sync_time_second  = obu_can_info_store.p_obu_time_sync_info.obu_sync_time_second;
    p_obu_Time_sync_info->obu_sync_time_mil_sec = (obu_can_info_store.p_obu_time_sync_info.obu_sync_time_mil_sec);
    emit send_can_frame(p_time_can_frame);
}


quint16 can_send_manager::reversebytes_uint16t(quint16 value)
{
    return (((value & 0x00FF) << 8) | ((value &0xFF00) >> 8));
}


quint32 can_send_manager::reversebytes_uint32t(quint32 value)
{
    return ((value & 0x000000FF) << 24 | (value & 0x0000FF00) << 8 | (value & 0x00FF0000) >> 8 | (value & 0xFF000000) >> 24);
}


quint64 can_send_manager::reversebytes_uint64t(quint64 value)
{
    quint64 tail_u32    =  (quint64)reversebytes_uint32t((quint32)(value & 0xFFFFFFFF));
    quint64 head_u32 = (quint64)reversebytes_uint32t((value & 0xFFFFFFFF00000000) >> 32);
    return  ((tail_u32 << 32) | head_u32);
}
//void can_send_manager::send_obu_vehicle_info()
//{
//    can_frame *p_vehicle_can_frame = new can_frame;
//    memset(p_vehicle_can_frame, 0, sizeof(can_frame));
//    p_vehicle_can_frame->can_id  = (OBU_STATE_INFO_CAN_ID | CAN_EFF_FLAG);
////    qDebug("send a obu_vehicle_info frame,ID is: %x\n", p_vehicle_can_frame->can_id);
//    p_vehicle_can_frame->can_dlc = sizeof(obu_can_info_store.p_vehicle_info);
//    obu_vehicle_info *p_obu_vehicle_info = (obu_vehicle_info *)p_vehicle_can_frame->data;
//    memcpy(p_obu_vehicle_info, &(obu_can_info_store.p_vehicle_info), sizeof(obu_can_info_store.p_vehicle_info));
////    qDebug("send a bu_extra_vehicle_info frame,ID is: %x\n", p_vehicle_can_frame->can_id);
//    emit send_can_frame(p_vehicle_can_frame);
//}


//void can_send_manager::send_obu_extra_vehicle_info()
//{
//    can_frame *p_extra_can_frame = new can_frame;
//    memset(p_extra_can_frame, 0, sizeof(can_frame));
//    p_extra_can_frame->can_id  = (OBU_EXTRA_INFO_CAN_ID | CAN_EFF_FLAG);
////    qDebug("send a bu_extra_vehicle_info frame,ID is: %x\n", p_extra_can_frame->can_id);
//    p_extra_can_frame->can_dlc = sizeof(obu_can_info_store.p_extra_vehicle_info);
//    obu_extra_vehicle_info *p_extra_vehicle_info = (obu_extra_vehicle_info *)p_extra_can_frame->data;
//    memcpy(p_extra_vehicle_info, &(obu_can_info_store.p_extra_vehicle_info), sizeof(obu_can_info_store.p_extra_vehicle_info));
////    qDebug("send a bu_extra_vehicle_info frame,ID is: %x\n", p_extra_can_frame->can_id);
//    emit send_can_frame(p_extra_can_frame);
//}


void can_send_manager::send_can_data()
{
    if(!(timeout_count % PERIOD_100MS_COUNT)){
        send_100ms_info();
    }
    if(!(timeout_count % PERIOD_1000MS_COUNT)){
        send_1000ms_info();
        timeout_count = 0;
    }
    send_10ms_info();
    timeout_count++;
//     send_rtk_latitude_info();
//     send_rtk_longitude_info();
//     send_obu_vehicle_info();
}


void can_send_manager::accept_modify_can_info(obu_can_info_update *p_can_info)
{
    int command_type = p_can_info->update_command_type;
    switch(command_type){
    case OBU_MODIFY_LIGHT_STATE:
        update_obu_light_info(&(p_can_info->obu_update_info));
        break;
    case OBU_MODIFY_GPS_INFO:
        update_obu_rtk_info(&(p_can_info->obu_update_info));
        break;
    case OBU_MODIFY_FAULT_INFO:
        update_obu_fault_info(&(p_can_info->obu_update_info));
        break;
    case OBU_MODIFY_VFRONT_INFO:
        update_obu_vfront_info(&(p_can_info->obu_update_info));
        break;
    case OBU_MODIFY_CONTROL_INFO:
        update_obu_control_info(&(p_can_info->obu_update_info));
        break;
        /* other cases add below */
    }
    if(p_can_info){
        delete p_can_info;
        p_can_info = NULL;
    }
}





