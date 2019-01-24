#ifndef CAN_RECV_H
#define CAN_RECV_H

#include "includes.h"

#define CAN_RECV_CHECK_PERIOD   1

class can_recv : public QObject
{
    Q_OBJECT
public:
    explicit can_recv(int fd, QObject *parent = 0);
    ~can_recv();

    int                can_fd;
    QTimer            *p_recv_timer;


signals:
    void transmit_can_recv_msg(can_frame *);

public slots:
    void check_can_bus_recv();
};

#endif // CAN_RECV_H
