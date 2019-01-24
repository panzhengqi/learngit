#ifndef CAN_RECV_MANAGER_H
#define CAN_RECV_MANAGER_H

#include "includes.h"

#define CAN_MSG_PROCESS_PERIOD        2


#define OBU_ALGORITHM_PARA_OBSTACLE    0x01
#define OBU_ALGORITHM_PARA_CONTROL    0x02


typedef struct Obu_algorithm_para_obstacle{
    unsigned char  Obstacle_type;
    unsigned char  Obstacle_distance;
    char           Obstacle_speed;
    unsigned short Obstacle_TTC;
    unsigned char  WC_brake_feedback;
}Obu_algorithm_para_obstacle;


typedef struct Obu_algorithm_para_control{
    unsigned char  Current_speed;
    unsigned char  Real_throttle_fb;
}Obu_algorithm_para_control;


typedef struct Obu_algorithm_paras{
    int                         type;
    Obu_algorithm_para_obstacle p_obstacle_para;
    Obu_algorithm_para_control  p_control_para;
}Obu_algorithm_paras;


class can_recv_manager : public QObject
{
    Q_OBJECT
public:
    can_recv_manager();
    ~can_recv_manager();

    QQueue<can_frame *> *p_recv_queue;
    QTimer              *p_process_timer;

public:
    void process_can_frame(can_frame *p_can_frame);
    void process_icm_can_frame(can_frame *p_can_frame);
    void process_obu_can_frame(can_frame *p_can_frame);
    void process_mcu_can_frame(can_frame *p_can_frame);

    void process_icm_state1_msg(__u8 *msg);
    void process_icm_state2_msg(__u8 *msg);
    void process_icm_state3_msg(__u8 *msg);

    void process_mcu_control_msg(__u8 *msg);

signals:
    void send_algorithm_paras(Obu_algorithm_paras *);

public slots:
    void check_process_can_frame();
    void accept_can_frame(can_frame *p_can_frame);
};

#endif // CAN_RECV_MANAGER_H
