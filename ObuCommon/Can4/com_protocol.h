#ifndef COM_PROTOCOL_H
#define COM_PROTOCOL_H

#define ICM                            0xB4
#define OBU                            0xB5
#define MCU                            0xB6

#define CURRENT_VERSION                0x01

#define CAN_FILTER_ID                  0x18FF7FBF
#define CAN_FILTER_MASK                0x18FF70B0
#define MSG_TYPE_MASK                  0xFF

/* <received_can_id> & mask == can_id & mask */

#define ICM_STATE_1_CAN_ID             0x18FF70B4
#define ICM_STATE_1_PERIOD             10
#define ICM_OBSTACLE_TYPE_NULL         0
#define ICM_OBSTACLE_TYPE_VEHICLE      1
#define ICM_OBSTACLE_TYPE_HUMAN        2
#define ICM_OBSTACLE_TYPE_RIDER        3
#define ICM_OBSTACLE_TYPE_INVALID      0xFF
#define ICM_OBSTACLE_DIS_RESO_1M       1      // 1m/bit
#define ICM_OBSTACLE_SPE_RESO_1KMH     1      // 1Km/h/bit
#define ICM_OBSTACLE_SPE_OFFSET        -125
#define ICM_OBSTACLE_TTC_RESO_10MS     10     // 0.01s/bit
#define ICM_CONTROL_RESO_0_4           0.4    // 0.4%/bit
#define ICM_WC_BRAKE_FB_RESO_0_4       0.4    // 0.4%/bit


#define ICM_STATE_2_CAN_ID             0x18FF71B4
#define ICM_STATE2_PERIOD              50
#define ICM_OBSTACLE_DIRE_NULL         0
#define ICM_OBSTACLE_DIRE_NEARBY       1
#define ICM_OBSTACLE_DIRE_FAR          2
#define ICM_OBSTACLE_DIRE_RESERVE      3
#define ICM_LANE_DEPARTURE_NULL        0
#define ICM_LANE_DEPARTURE_LEFT        1
#define ICM_LANE_DEPARTURE_RIGHT       2
#define ICM_LANE_DEPARTURE_RESERVE     3
#define ICM_CONTROL_MODE_TRADITION     0
#define ICM_CONTROL_MODE_OPTIMAL       1
#define ICM_WC_BRAKE_TURNOFF           0
#define ICM_WC_BRAKE_AEB               1
#define ICM_WC_BRAKE_OPTIMAL           2
#define ICM_WC_BRAKE_MEC_ANTIHIT       3
#define ICM_GEAR_N                     0x00
#define ICM_GEAR_D_LOW                 0x01
#define ICM_GEAR_D_HIGH                0x0A
#define ICM_GEAR_R                     0x0B
#define ICM_GEAR_P                     0x0C
#define ICM_AEB_STATE_INVALID          0
#define ICM_AEB_STATE_VALID            1
#define ICM_AEB_STATE_FAULT            2
#define ICM_AEB_STATE_RESERVE          3
#define ICM_RUNNING_STATE_PARKING      0
#define ICM_RUNNING_STATE_IDLING       1
#define ICM_RUNNING_STATE_RUNNING      2
#define ICM_RUNNING_STATE_RESERVE      3
#define ICM_DOOR_STATE_CLOSE           0
#define ICM_DOOR_STATE_OPEN            1
#define ICM_DOOR_STATE_FAULT           2
#define ICM_DOOR_STATE_RESERVE         3
#define ICM_BRAKE_STATE_CLOSE          0
#define ICM_BRAKE_STATE_OPEN           1
#define ICM_BRAKE_STATE_FAULT          2
#define ICM_BRAKE_STATE_RESERVE        3
#define ICM_STATE_NORMAL               0
#define ICM_STATE_FAULT                1
#define ICM_DEVICE_STATE_TURNOFF       0
#define ICM_DEVICE_STATE_TURNON        1
#define ICM_DEVICE_STATE_FAULT         2
#define ICM_DEVICE_STATE_UNKNOW        3
#define ICM_THROTTLE_FB_RESO_0_4       0.4        // 0.4% / bit
#define ICM_SPEED_RESO_UP              1.0
#define ICM_SPEED_RESO_DOWN            256.0        // 1/256 Km/h/bit


#define ICM_STATE_3_CAN_ID             0x18FF72B4
#define ICM_STATE_3_PERIOD             50
#define ICM_ENGINE_TORQUE_RESO_1       1          // 1%/bit
#define ICM_ENGINE_TORQUE_OFFSET       -125
#define ICM_ENGINE_SPEED_RESO_0_125    0.125      // 0.125rpm/bit
#define ICM_FUEL_CONSUM_RESO_0_05      0.05       // 0.05L/h
#define ICM_FUEL_DY_CONSUM_RESO        1.953      // (1/512 * 1000) km/g


#define OBU_RTK_LATITUDE_CAN_ID        0x18FF73B5
#define OBU_RTK_LONGITUDE_CAN_ID       0x18FF74B5
#define OBU_RTK_PERIOD                 100
#define OBU_RTK_RESO                   0.00000001  // 0.00000001/BIT


#define OBU_RGY_INFO_ID                0x18FF75B5
#define OBU_RGY_INFO_PERIOD            100
#define OBU_RGY_STATE_NULL             0x00
#define OBU_RGY_STATE_G                0x01
#define OBU_RGY_STATE_Y                0x02
#define OBU_RGY_STATE_R                0x03
#define OBU_RGY_NULL_TIME              0xFF
#define OBU_RGY_TIME_RESO_1            1             // 1s/bit
#define OBU_FAULT_INFO_NORMAL          0x00
#define OBU_FAULT_INFO_OTHER           0x01      //ADD some other fault reason


#define OBU_RTK_INFO_ID                0x18FF76B5
#define OBU_RTK_INFO_PERIOD            100
#define OBU_RTK_FRONT_SPEED_RESO_1     1             // 1km/h      offset -127
#define OBU_RTK_FRONT_SPEED_OFFSET     127
#define OBU_RTK_HEADING_ANGLE_RESO_001 0.01          // 0.01 degree/bit
#define OBU_RTK_SPEED_RESO_0_1         0.1           // 0.1km/s
#define OBU_RTK_FRONT_DIST_RESO_0_1    0.1           // 0.1m/bit


#define OBU_CONT_INFO_ID               0x18FF77B5
#define OBU_CONT_INFO_PERIOD           10
#define OBU_CONT_WC_BRAKE_RESO_0_4     0.4           // 0.4%/bit
#define OBU_CONT_5G_SIGNAL_INVALID     0x00
#define OBU_CONT_5G_SIGNAL_VALID       0x01


#define OBU_TIME_SYNC_ID               0x18FF78B5


#define MCU_CONTROL_INFO_CAN_ID        0x18FF79B6
#define MCU_CONTROL_INFO_PERIOD        10
#define MCU_WC_THROTTLE_RESO_0_4       0.4       // 0.4% / bit
#define MCU_WC_BRAKE_RESO_0_4          0.4       // 0.4% / bit
#define MCU_REAL_THROTTLE_RESO_0_4     0.4       // 0.4% / bit
#define MCU_BRAKE_CONT_STATE_INVALID   0x00
#define MCU_BRAKE_CONT_STATE_VALID     0x01
#define MCU_BRAKE_CONT_STATE_FAULT     0x02
#define MCU_BRAKE_CONT_STATE_RESERVE   0x03
#define MCU_THROTTLE_MODE_INVALID      0x00
#define MCU_THROTTLE_MODE_DRIVER       0x01
#define MCU_THROTTLE_MODE_COMMAND      0x02
#define MCU_THROTTLE_MODE_FAULT        0x03
#define MCU_BSPEED_STATE_NORMAL        0x00
#define MCU_BSPEED_STATE_GETTING       0x01
#define MCU_BSPEED_STATE_FAULT_VER     0x02     // version fault
#define MCU_BSPEED_STATE_FAULT_CHK     0x03     // check code fault
#define MCU_BSPEED_STATE_FAULT_POS     0x04     // position fault
#define MCU_BSPEED_STATE_TIMEOUT       0x05
#define MCU_VOICE_ANN_NULL             0x00
#define MCU_VOICE_ANN_START            0x01
#define MCU_VOICE_ANN_STOP             0x02
#define MCU_VOICE_ANN_SHUT             0x03
#define MCU_VOICE_ANN_NORMAL           0x04
#define MCU_VOICE_ANN_ABNORMAL         0x05
#define MCU_VOICE_ANN_BRAKE_VALID      0x06
#define MCU_THROTTLE_FAULT_NORMAL      0x00
#define MCU_THROTTLE_FAULT_UNSTART     0x01
#define MCU_THROTTLE_FAULT_IN1_MORE    0x02
#define MCU_THROTTLE_FAULT_IN1_LESS    0x03
#define MCU_THROTTLE_FAULT_OUT1_MORE   0x04
#define MCU_THROTTLE_FAULT_OUT1_LESS   0x05
#define MCU_THROTTLE_FAULT_IN2_MORE    0x06
#define MCU_THROTTLE_FAULT_IN2_LESS    0x07
#define MCU_THROTTLE_FAULT_OUT2_MORE   0x08
#define MCU_THROTTLE_FAULT_OUT2_LESS   0x09
#define MCU_TBOX_FAULT_STATE_INVALID   0x00
#define MCU_TBOX_FAULT_STATE_VALID     0x01
#define MCU_TBOX_FAULT_STATE_FAULT     0x02
#define MCU_TBOX_FAULT_STATE_RESERVE   0x03
#define MCU_DRIVE_FAULT_STATE_INVALID  0x00
#define MCU_DRIVE_FAULT_STATE_VALID    0x01
#define MCU_DRIVE_FAULT_STATE_FAULT    0x02
#define MCU_DRIVE_FAULT_STATE_RESERVE  0x03


#pragma pack(push, 1)

typedef struct icm_state_1{
    unsigned char  icm_Version;
    unsigned char  icm_Obstacle_Type;
    unsigned char  icm_Obstacle_distance;
    unsigned char  icm_Obstacle_speed;
    unsigned short icm_Obstacle_TTC;
    unsigned char  icm_Control_level;
    unsigned char  icm_WC_brake_feedback;
}icm_state_1;


typedef struct icm_state_2{
    unsigned char  icm_Version;
    unsigned char  icm_Obstacle_direction:2;
    unsigned char  icm_Lane_departure:2;
    unsigned char  icm_Control_mode:2;
    unsigned char  icm_WC_brake:2;
    unsigned char  icm_Current_gear:4;
    unsigned char  icm_AEB_state:2;
    unsigned char  icm_Running_state:2;
    unsigned char  icm_Front_door_state:2;
    unsigned char  icm_Middle_door_state:2;
    unsigned char  icm_Last_door_state:2;
    unsigned char  icm_Brake_state:2;
    unsigned char  icm_Work_state:2;
    unsigned char  icm_Airconditionor_state:2;
    unsigned char  icm_Parking_state:2;
    unsigned char  icm_Retarder_state:2;
    unsigned char  icm_Real_throttle_fb;
    unsigned short icm_Current_speed;
}icm_state_2;


typedef struct icm_state_3{
    unsigned char  icm_Version;
    unsigned char  icm_Engine_torque_percent;
    unsigned short icm_Engine_speed;
    unsigned short icm_Fuel_consumption_rate;
    unsigned short icm_Dynamical_fuel_consumption;
}icm_state_3;


typedef struct obu_RTK_latitude_info{
    long long  obu_RTK_latitude;
}obu_RTK_latitude_info;


typedef struct obu_RTK_longitude_info{
    long long  obu_RTK_longitude;
}obu_RTK_longitude_info;


typedef struct obu_RGY_info{
    unsigned char  obu_Version;
    unsigned char  obu_RGY_state:2;
    unsigned char  obu_RGY_reserved1:6;
    unsigned char  obu_RGY_time;
    unsigned char  obu_Fault_info;
    unsigned int   obu_RGY_reserved2;
}obu_RGY_info;


typedef struct obu_RTK_info{
    unsigned char  obu_Version;
    char           obu_RTK_front_speed;
    unsigned short obu_RTK_heading_angle;
    unsigned short obu_RTK_speed;
    unsigned short obu_RTK_front_distance;
}obu_RTK_info;


typedef struct obu_Control_info{
    unsigned char  obu_Version;
    unsigned char  obu_Con_WC_brake_percent;
    unsigned char  obu_Con_5G_signal_state:1;
    unsigned char  obu_Con_reserved1:7;
    unsigned char  obu_Con_reserved2;
    unsigned int   obu_Con_reserved3;
}obu_Control_info;


typedef struct obu_Time_sync_info{
    unsigned char  obu_sync_time_year;
    unsigned char  obu_sync_time_mouth;
    unsigned char  obu_sync_time_day;
    unsigned char  obu_sync_time_hour;
    unsigned char  obu_sync_time_minute;
    unsigned char  obu_sync_time_second;
    unsigned short obu_sync_time_mil_sec;
}obu_Time_sync_info;


typedef struct mcu_control_info{
    unsigned char  mcu_Version;
    unsigned char  mcu_WC_throttle_percent;
    unsigned char  mcu_WC_brake_percent;
    unsigned char  mcu_Real_throttle_percent;
    unsigned char  mcu_Brake_control_state:2;
    unsigned char  mcu_Throttle_control_mode:2;
    unsigned char  mcu_Best_speed_control_state:4;
    unsigned char  mcu_Voice_announce_command;
    unsigned char  mcu_Throttle_fault_info:4;
    unsigned char  mcu_Fault_state:2;
    unsigned char  mcu_Drive_control_state:2;
    unsigned char  mcu_Reserve;
}mcu_control_info;


typedef struct obu_can_info{
    obu_RTK_latitude_info   p_latitude_info;
    obu_RTK_longitude_info  p_longitude_info;
    obu_RGY_info            p_obu_rgy_info;
    obu_RTK_info            p_obu_rtk_info;
    obu_Control_info        p_obu_control_info;
    obu_Time_sync_info      p_obu_time_sync_info;
}obu_can_info;


typedef struct obu_can_info_update{
    unsigned char   update_command_type;
    obu_can_info    obu_update_info;
}obu_can_info_update;


#pragma pack(pop)


class com_protocol
{
public:
    com_protocol();
};

#endif // COM_PROTOCOL_H
