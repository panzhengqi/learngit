#ifndef INNER_PROTOCOL_H
#define INNER_PROTOCOL_H

#include "includes.h"

#define INNER_MSG_TYPE_RTK        "0"
#define INNER_MSG_TYPE_RGY        "1"
#define INNER_MSG_TYPE_OBU_FAULT  "2"
#define INNER_MSG_TYPE_F_VEHICLE  "3"
#define INNER_MSG_TYPE_CONTROL    "4"

#define IS_SELF_YES               "1"
#define IS_SELF_NO                "0"

#define RSU_LIGHT_COLOR_UNKNOW    "0"
#define RSU_LIGHT_COLOR_GREEN     "1"
#define RSU_LIGHT_COLOR_RED       "2"
#define RSU_LIGHT_COLOR_YELLOW    "3"

#define OBU_FAULT_NORMAL          "0"
#define OBU_FAULT_OTHER           "1"   // Other fault info could be add here...
//Command type
#define OBU_MODIFY_LIGHT_STATE    0x01
#define OBU_MODIFY_GPS_INFO       0x02
#define OBU_MODIFY_FAULT_INFO     0x03
#define OBU_MODIFY_VFRONT_INFO    0x04
#define OBU_MODIFY_CONTROL_INFO   0x05


#endif // INNER_PROTOCOL_H
