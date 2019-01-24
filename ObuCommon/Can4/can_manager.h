#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include "can_recv_manager.h"
#include "can_send_manager.h"
#include "inner_recv_manager.h"

#define CAN_RECV_CHECK_PERIOD   1

class can_manager : public QObject
{
    Q_OBJECT
public:
    can_manager();
    ~can_manager();

    int                  can_fd;
    QTimer              *check_recv_timer;
    can_recv_manager    *p_can_recv_manager;
    QThread             *p_can_recv_thread;
    can_send_manager    *p_can_send_manager;
    QThread             *p_can_send_thread;
    inner_recv_manager  *p_inner_recv_manager;
    QThread             *p_inner_recv_thread;

    long long            can_receive_frame_count;
    long long            can_send_frame_count;
    int                  can_failed_count;

public:
    int  can_sock_init(const char *interface);
    void set_can_filter();
    int  get_can_if_index(int fd, const char *interface);
    void release_can_socket();
    void can_filter_set(int fd, unsigned int id,unsigned int mask);


signals:
    void transmit_can_recv_msg(can_frame *);

public slots:
    void check_can_bus_recv();
    void can_frame_send(can_frame *p_can_frame);
};

#endif // CAN_MANAGER_H
