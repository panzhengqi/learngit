#include "can_recv_manager.h"

can_recv_manager::can_recv_manager()
{
    p_recv_queue = new QQueue<can_frame *>;
    p_process_timer = new QTimer(this);
    connect((QObject *)p_process_timer, SIGNAL(timeout()), this, SLOT(check_process_can_frame()));
    p_process_timer->start(CAN_MSG_PROCESS_PERIOD);
}


can_recv_manager::~can_recv_manager()
{
    if(p_process_timer){
        if(p_process_timer->isActive()){
            p_process_timer->stop();
        }
        delete p_process_timer;
        p_process_timer = NULL;
    }
    if(p_recv_queue){
        if(!p_recv_queue->isEmpty()){
            p_recv_queue->clear();
        }
        delete p_recv_queue;
        p_recv_queue = NULL;
    }
}


void can_recv_manager::process_can_frame(can_frame *p_can_frame)
{
    p_can_frame->can_id = (p_can_frame->can_id & CAN_EFF_MASK);
    char sender = ((p_can_frame->can_id) & MSG_TYPE_MASK);
//    qDebug("process_can_frame id: %x", sender);
    switch(sender){
    case ICM:
        process_icm_can_frame(p_can_frame);
        break;
    case OBU:
        process_obu_can_frame(p_can_frame);
        break;
    case MCU:
        process_mcu_can_frame(p_can_frame);
        break;
    default:
        qDebug("Receive CAN frame with unknow ID, Abort it...");
        break;
    }
}


void can_recv_manager::process_icm_can_frame(can_frame *p_can_frame)
{
    switch(p_can_frame->can_id){
    case ICM_STATE_1_CAN_ID:
        process_icm_state1_msg(p_can_frame->data);
        break;
    case ICM_STATE_2_CAN_ID:
        process_icm_state2_msg(p_can_frame->data);
        break;
    case ICM_STATE_3_CAN_ID:
        process_icm_state3_msg(p_can_frame->data);
        break;
    default:
        qDebug("Receive ICM type CAN_Frame with unknow ID, Abort it...");
        break;
    }
}


void can_recv_manager::process_obu_can_frame(can_frame *p_can_frame)
{
//    qDebug("Receive a OBU can_frame...");
    switch(p_can_frame->can_id){
    case OBU_RTK_LATITUDE_CAN_ID:
        break;
    case OBU_RTK_LONGITUDE_CAN_ID:
        break;
    case OBU_RGY_INFO_ID:
        break;
    case OBU_RTK_INFO_ID:
        break;
    case OBU_CONT_INFO_ID:
        break;
    case OBU_TIME_SYNC_ID:
        break;
    default:
        qDebug("Receive OBU type CAN_Frame with unknow ID, Abort it...");
        break;
    }
}


void can_recv_manager::process_mcu_can_frame(can_frame *p_can_frame)
{
    switch(p_can_frame->can_id){
    case MCU_CONTROL_INFO_CAN_ID:
        process_mcu_control_msg(p_can_frame->data);
        break;
    default:
        qDebug("Receive MCU type CAN_Frame with unknow ID: %d, Abort it...", p_can_frame->can_id);
        break;
    }
}


void can_recv_manager::process_icm_state1_msg(__u8 *msg)
{
    icm_state_1 *p_icm_state_1 = (icm_state_1 *)msg;
    qDebug("process_icm_state1_msg: Version -> %d \r\n" \
           "                        icm_Obstacle_Type -> %d \r\n" \
           "                        icm_Obstacle_distance -> %d m \r\n" \
           "                        icm_Obstacle_speed -> %d km/h \r\n" \
           "                        icm_Obstacle_TTC -> %f s  \r\n" \
           "                        icm_Control_level -> %f%% \r\n" \
           "                        icm_WC_brake_feedback -> %f%% \r\n",
           p_icm_state_1->icm_Version, p_icm_state_1->icm_Obstacle_Type,
           (p_icm_state_1->icm_Obstacle_distance * ICM_OBSTACLE_DIS_RESO_1M),
           (p_icm_state_1->icm_Obstacle_speed * ICM_OBSTACLE_SPE_RESO_1KMH),
           ((double)p_icm_state_1->icm_Obstacle_TTC * ICM_OBSTACLE_TTC_RESO_10MS / 1000),
           ((double)p_icm_state_1->icm_Control_level * ICM_CONTROL_RESO_0_4),
           ((double)p_icm_state_1->icm_WC_brake_feedback * ICM_WC_BRAKE_FB_RESO_0_4));
    Obu_algorithm_paras *p_algorithm_paras = new Obu_algorithm_paras;
    memset(p_algorithm_paras, 0, sizeof(Obu_algorithm_paras));
    p_algorithm_paras->type = OBU_ALGORITHM_PARA_OBSTACLE;
    p_algorithm_paras->p_obstacle_para.Obstacle_type       = p_icm_state_1->icm_Obstacle_Type;
    p_algorithm_paras->p_obstacle_para.Obstacle_distance   = p_icm_state_1->icm_Obstacle_distance * ICM_OBSTACLE_DIS_RESO_1M;
    p_algorithm_paras->p_obstacle_para.Obstacle_speed      = (p_icm_state_1->icm_Obstacle_speed * ICM_OBSTACLE_SPE_RESO_1KMH) + ICM_OBSTACLE_SPE_OFFSET;
    p_algorithm_paras->p_obstacle_para.Obstacle_TTC        = p_icm_state_1->icm_Obstacle_TTC * ICM_OBSTACLE_TTC_RESO_10MS / 1000;
    p_algorithm_paras->p_obstacle_para.WC_brake_feedback   = p_icm_state_1->icm_WC_brake_feedback *ICM_WC_BRAKE_FB_RESO_0_4;
    emit send_algorithm_paras(p_algorithm_paras);
}


void can_recv_manager::process_icm_state2_msg(__u8 *msg)
{
    icm_state_2 *p_icm_state_2 = (icm_state_2 *)msg;
    qDebug("process_icm_state2_msg: Version -> %d \r\n" \
           "                        icm_Obstacle_direction -> %d \r\n" \
           "                        icm_Lane_departure -> %d \r\n" \
           "                        icm_Control_mode -> %d \r\n" \
           "                        icm_WC_brake -> %d \r\n" \
           "                        icm_Current_gear -> %d \r\n" \
           "                        icm_AEB_state -> %d \r\n" \
           "                        icm_Running_state -> %d \r\n" \
           "                        icm_Front_door_state -> %d \r\n" \
           "                        icm_Middle_door_state -> %d \r\n" \
           "                        icm_Last_door_state -> %d \r\n" \
           "                        icm_Brake_state -> %d \r\n" \
           "                        icm_Work_state -> %d \r\n" \
           "                        icm_Airconditionor_state -> %d \r\n" \
           "                        icm_Parking_state -> %d \r\n" \
           "                        icm_Retarder_state -> %d \r\n" \
           "                        icm_Real_throttle_fb -> %f%%\r\n" \
           "                        icm_Current_speed -> %f km/h\r\n",
           p_icm_state_2->icm_Version, p_icm_state_2->icm_Obstacle_direction,
           p_icm_state_2->icm_Lane_departure, p_icm_state_2->icm_Control_mode,
           p_icm_state_2->icm_WC_brake, p_icm_state_2->icm_Current_gear,
           p_icm_state_2->icm_AEB_state, p_icm_state_2->icm_Running_state,
           p_icm_state_2->icm_Front_door_state, p_icm_state_2->icm_Middle_door_state,
           p_icm_state_2->icm_Last_door_state, p_icm_state_2->icm_Brake_state,
           p_icm_state_2->icm_Work_state, p_icm_state_2->icm_Airconditionor_state,
           p_icm_state_2->icm_Parking_state, p_icm_state_2->icm_Retarder_state,
           (p_icm_state_2->icm_Real_throttle_fb * ICM_THROTTLE_FB_RESO_0_4),
           (p_icm_state_2->icm_Current_speed * ICM_SPEED_RESO_UP / ICM_SPEED_RESO_DOWN));
    Obu_algorithm_paras *p_algorithm_paras = new Obu_algorithm_paras;
    memset(p_algorithm_paras, 0, sizeof(Obu_algorithm_paras));
    p_algorithm_paras->type = OBU_ALGORITHM_PARA_CONTROL;
    p_algorithm_paras->p_control_para.Current_speed       = p_icm_state_2->icm_Current_speed * ICM_SPEED_RESO_UP / ICM_SPEED_RESO_DOWN;
    p_algorithm_paras->p_control_para.Real_throttle_fb    = p_icm_state_2->icm_Real_throttle_fb * ICM_THROTTLE_FB_RESO_0_4;
    emit send_algorithm_paras(p_algorithm_paras);
}


void can_recv_manager::process_icm_state3_msg(__u8 *msg)
{
    icm_state_3 *p_icm_state_3 = (icm_state_3 *)msg;
    qDebug("process_icm_state3_msg: Version -> %d \r\n" \
           "                        icm_Engine_torque_percent -> %d%% \r\n" \
           "                        icm_Engine_speed -> %f rpm \r\n" \
           "                        icm_Fuel_consumption_rate -> %f L/h \r\n" \
           "                        icm_Dynamical_fuel_consumption -> %f km/g  \r\n",
           p_icm_state_3->icm_Version,
           (p_icm_state_3->icm_Engine_torque_percent * ICM_ENGINE_TORQUE_RESO_1 +  ICM_ENGINE_TORQUE_OFFSET),
           (p_icm_state_3->icm_Engine_speed * ICM_ENGINE_SPEED_RESO_0_125),
           (p_icm_state_3->icm_Fuel_consumption_rate * ICM_FUEL_CONSUM_RESO_0_05),
           (p_icm_state_3->icm_Dynamical_fuel_consumption * ICM_FUEL_DY_CONSUM_RESO));
}


void can_recv_manager::process_mcu_control_msg(__u8 *msg)
{
    mcu_control_info *p_mcu_control_info = (mcu_control_info *)msg;
    qDebug("process_mcu_control_msg: Version -> %d \r\n" \
           "                         WC_Throttle_percent: -> %f%% \r\n" \
           "                         WC_Brake_percent: -> %f%% \r\n" \
           "                         Real_Throttle_percent: -> %f%% \r\n" \
           "                         Brake_Command: -> %d \r\n" \
           "                         Throttle_Mode: -> %d \r\n" \
           "                         Best_Speed_state: -> %d \r\n" \
           "                         Voice_Command: -> %d \r\n" \
           "                         Throttle_Fault_info: -> %d \r\n" \
           "                         T-BOX fault_info: -> %d \r\n" \
           "                         Drive Controller fault_info: %d \r\n",
           p_mcu_control_info->mcu_Version, (p_mcu_control_info->mcu_WC_throttle_percent*MCU_WC_THROTTLE_RESO_0_4),
           (p_mcu_control_info->mcu_WC_brake_percent*MCU_WC_BRAKE_RESO_0_4), (p_mcu_control_info->mcu_Real_throttle_percent*MCU_REAL_THROTTLE_RESO_0_4),
           p_mcu_control_info->mcu_Brake_control_state, p_mcu_control_info->mcu_Throttle_control_mode,
           p_mcu_control_info->mcu_Best_speed_control_state, p_mcu_control_info->mcu_Voice_announce_command,
           p_mcu_control_info->mcu_Throttle_fault_info, p_mcu_control_info->mcu_Fault_state,
           p_mcu_control_info->mcu_Drive_control_state);
}


void can_recv_manager::check_process_can_frame()
{
//    can_frame *p_can_frame = new can_frame;
//    p_can_frame->can_id = ICM_STATE_1_CAN_ID;
//    p_can_frame->can_dlc = 8;
//    icm_state_1 *p_icm_state_1 = (icm_state_1 *)p_can_frame->data;
//    p_icm_state_1->icm_Obstacle_Type = ICM_OBSTACLE_TYPE_VEHICLE;
//    p_icm_state_1->icm_Obstacle_distance = 0x1F;
//    p_icm_state_1->icm_Obstacle_speed = 0x1F;
//    p_icm_state_1->icm_Obstacle_TTC = 0x1F;
//    p_icm_state_1->icm_WC_brake_feedback = 0x1F;
//    p_recv_queue->enqueue(p_can_frame);

    if(!p_recv_queue->isEmpty()){
        can_frame *p_can_frame = p_recv_queue->dequeue();
        process_can_frame(p_can_frame);
        if(p_can_frame){
            delete p_can_frame;
            p_can_frame = NULL;
        }
    }
}


void can_recv_manager::accept_can_frame(can_frame *p_can_frame)
{
    if(p_recv_queue){
        p_recv_queue->enqueue(p_can_frame);
    }
}
