#include "can_recv.h"

can_recv::can_recv(int fd, QObject *parent) :
    QObject(parent)
{
    can_fd            = fd;
    p_recv_timer      = new QTimer(this);
    connect((QObject *)p_recv_timer, SIGNAL(timeout()), this, SLOT(check_can_bus_recv()));
    p_recv_timer->start(CAN_RECV_CHECK_PERIOD);
}


can_recv::~can_recv()
{
    if(p_recv_timer){
        if(p_recv_timer->isActive()){
            p_recv_timer->stop();
        }
        delete p_recv_timer;
        p_recv_timer = NULL;
    }
}


void can_recv::check_can_bus_recv()
{
    struct sockaddr_can addr;
    socklen_t len = sizeof(addr);
    int nbytes;

    can_frame  temp_can_frame;
    memset(&temp_can_frame, 0, sizeof(can_frame));
    nbytes = recvfrom(can_fd, &temp_can_frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, &len);
    if(nbytes > 0){
        can_frame *p_can_frame = new can_frame;
        memset(p_can_frame, 0, sizeof(can_frame));
        memcpy(p_can_frame, &temp_can_frame, sizeof(temp_can_frame));
        emit transmit_can_recv_msg(p_can_frame);
    }
//    else{
//        qDebug("can_recv::check_can_bus_recv: this should not happen in block mode....");
//        if(p_can_frame){
//            delete p_can_frame;
//            p_can_frame = NULL;
//        }
//    }
}
