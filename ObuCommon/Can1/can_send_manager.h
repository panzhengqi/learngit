#ifndef CAN_SEND_MANAGER_H
#define CAN_SEND_MANAGER_H

#include "includes.h"

#define PERIOD_10MS               10
#define PERIOD_100MS_COUNT        10
#define PERIOD_1000MS_COUNT       100

class can_send_manager : public QObject
{
    Q_OBJECT
public:
    explicit can_send_manager(QObject *parent = 0);
    ~can_send_manager();

    QTimer            *p_send_timer;
    int                timeout_count;
    obu_can_info       obu_can_info_store;

public:
    void update_obu_light_info(obu_can_info *p_obu_can_info);
    void update_obu_rtk_info(obu_can_info *p_obu_can_info);
    void update_obu_fault_info(obu_can_info *p_obu_can_info);
    void update_obu_vfront_info(obu_can_info *p_obu_can_info);
    void update_obu_control_info(obu_can_info *p_obu_can_info);
    void send_10ms_info();
    void send_100ms_info();
    void send_1000ms_info();
    void send_obu_control_info();
    void send_rtk_latitude_info();
    void send_rtk_longitude_info();
    void send_obu_fault_info();
    void send_obu_light_info();
    void send_obu_rtk_info();
    void send_obu_time_sync_info();
    quint16 reversebytes_uint16t(quint16 value);
    quint32 reversebytes_uint32t(quint32 value);
    quint64 reversebytes_uint64t(quint64 value);

signals:
    void send_can_frame(can_frame *p_can_frame);

public slots:
    void send_can_data();
    void accept_modify_can_info(obu_can_info_update *);
};

#endif // CAN_SEND_MANAGER_H
