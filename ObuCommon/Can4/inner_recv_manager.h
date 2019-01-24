#ifndef INNER_RECV_MANAGER_H
#define INNER_RECV_MANAGER_H

#include "includes.h"


#define INNER_RECV_CHK_PERIOD     10

typedef struct can_msg{
    char msg[MAX_CAN_MSG_LENGTH];
}can_msg;


class inner_recv_manager : public QObject
{
    Q_OBJECT
public:
    explicit inner_recv_manager(QObject *parent = 0);
    ~inner_recv_manager();

    QQueue<can_msg *> *p_inner_recv_queue;
    QTimer            *p_recv_check_timer;

public:
    void   process_recv_msg(can_msg *p_can_msg);
    void   process_rtk_msg(string str_msg);
    void   process_rgy_msg(string str_color, string str_time);
    void   process_obu_fault_msg(string fault);
    void   process_obu_vfront_msg(string dist, string spe);
    void   process_obu_control_msg(string wc_brake_percent, string signal_valid);
    int   analize_rtk_msg(char *msg, KSXT_STRU *p_gps_info);
    int     DetectionDataValidity(int num,char *str);
    int    GetComma(int num, char *str);
    double get_double_number(char *s);
    float  get_float_number(char *s);
    void   UTC2BTC(DATE_TIME * gps_data);
    int get_int_number(char *s);
    double get_locate(double temp);

signals:
    void send_can_packet(obu_can_info_update *p_obu_can_info);

public slots:
    void accept_inner_msg(can_msg *);     //Interface for incomming message
    void check_process_recv_msg();
};

#endif // INNER_RECV_MANAGER_H
