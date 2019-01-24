#include "can_manager.h"

can_manager::can_manager()
{
    while(-1 == (can_fd = can_sock_init(CAN_INTERFACE)))
    {
        qDebug("CAN socket Initial failed, check you device...");
        sleep(5);
    }
    set_can_filter();
    can_receive_frame_count = 0;
    can_send_frame_count    = 0;
    can_failed_count        = 0;
    check_recv_timer        = new QTimer(this);
    connect((QObject *)check_recv_timer, SIGNAL(timeout()), this, SLOT(check_can_bus_recv()));
    p_can_send_manager      = new can_send_manager(can_fd);
    p_can_send_thread       = new QThread();
//    connect((QObject *)p_can_send_manager, SIGNAL(send_can_frame(can_frame*)), this, SLOT(can_frame_send(can_frame*)));
    p_can_send_manager->moveToThread(p_can_send_thread);

    p_can_recv_manager      = new can_recv_manager();
    p_can_recv_thread       = new QThread();
    connect((QObject *)this, SIGNAL(transmit_can_recv_msg(can_frame *)), p_can_recv_manager, SLOT(accept_can_frame(can_frame*)));
    p_can_recv_manager->moveToThread(p_can_recv_thread);

    p_inner_recv_manager    = new inner_recv_manager();
    p_inner_recv_thread     = new QThread();
    connect((QObject *)p_inner_recv_manager, SIGNAL(send_can_packet(obu_can_info_update *)), p_can_send_manager, SLOT(accept_modify_can_info(obu_can_info_update*)));
    p_inner_recv_manager->moveToThread(p_inner_recv_thread);

    p_can_send_thread->start();
    p_can_recv_thread->start();
    p_inner_recv_thread->start();
    check_recv_timer->start(CAN_RECV_CHECK_PERIOD);
}


can_manager::~can_manager()
{
    release_can_socket();
    if(check_recv_timer){
        if(check_recv_timer->isActive()){
            check_recv_timer->stop();
        }
        delete check_recv_timer;
        check_recv_timer = NULL;
    }
    if(p_can_send_manager){
        delete p_can_send_manager;
        p_can_send_manager = NULL;
    }
    if(p_can_send_thread){
        p_can_send_thread->terminate();
        p_can_send_thread->wait();
        delete p_can_send_thread;
        p_can_send_thread = NULL;
    }
    if(p_can_recv_manager){
        delete p_can_recv_manager;
        p_can_recv_manager = NULL;
    }
    if(p_can_recv_thread){
        p_can_recv_thread->terminate();
        p_can_recv_thread->wait();
        delete p_can_recv_thread;
        p_can_recv_thread = NULL;
    }
    if(p_inner_recv_manager){
        delete p_inner_recv_manager;
        p_inner_recv_manager = NULL;
    }
    if(p_inner_recv_thread){
        p_inner_recv_thread->terminate();
        p_inner_recv_thread->wait();
        delete p_inner_recv_thread;
        p_inner_recv_thread = NULL;
    }
}


int can_manager::can_sock_init(const char *interface)
{
    int can_fd;
    int if_index = 0;
    struct sockaddr_can addr;

    system("ip link set " CAN_INTERFACE " type can bitrate 250000");
    system("ip link set " CAN_INTERFACE " up");

    can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (can_fd < 0) {
        fprintf(stderr, "Try to create can socket failed, %s...\n", strerror(errno));
        return -1;
    }

    if (NULL != interface)
        if_index = get_can_if_index(can_fd, interface);

    if(-1 == if_index){
        if(can_fd > 0){
            close(can_fd);
            can_fd = 0;
        }
        return if_index;
    }

    addr.can_family = AF_CAN;
    /* index 0 means to bind to all interfaces */
    addr.can_ifindex = if_index;

    bind(can_fd, (struct sockaddr *)&addr, sizeof(addr));

    return can_fd;
}


void can_manager::set_can_filter()
{
    can_filter_set(can_fd, CAN_FILTER_ID, CAN_FILTER_MASK);
}


int can_manager::get_can_if_index(int fd, const char *interface)
{
    int ret;
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, interface);
    if(-1 == ioctl(fd, SIOCGIFINDEX, &ifr)){
        ret = -1;
    }
    else
        ret = ifr.ifr_ifindex;
    return ret;
}


void can_manager::release_can_socket()
{
    if(can_fd){
        close(can_fd);
        can_fd = 0;
    }
}


void can_manager::can_filter_set(int fd, unsigned int id, unsigned int mask)
{
    struct can_filter rfilter;
    /* <received_can_id> & mask == can_id & mask */
    rfilter.can_id   = id;
    rfilter.can_mask = mask;
    setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
    return;
}


void can_manager::check_can_bus_recv()
{
    struct sockaddr_can addr;
    socklen_t len = sizeof(addr);
    int nbytes;
//    can_frame  temp_can_frame;
//    memset(&temp_can_frame, 0, sizeof(can_frame));
//    while(0 < (nbytes = recvfrom(can_fd, &temp_can_frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, &len))){
//        can_frame *p_can_frame = new can_frame;
//        memset(p_can_frame, 0, sizeof(can_frame));
//        memcpy(p_can_frame, &temp_can_frame, sizeof(can_frame));
//        emit transmit_can_recv_msg(p_can_frame);
//    }

    can_frame *p_can_frame = new can_frame;
    memset(p_can_frame, 0, sizeof(can_frame));
    nbytes = recvfrom(can_fd, p_can_frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, &len);
    if(nbytes > 0){
        emit transmit_can_recv_msg(p_can_frame);
    }
    else{
        if(p_can_frame){
            delete p_can_frame;
            p_can_frame = NULL;
        }
    }
}


void can_manager::can_frame_send(can_frame *p_can_frame)
{
    struct sockaddr_can addr;
    socklen_t len = sizeof(addr);
    int nbytes;
    if (!CAN_INTERFACE)
        qDebug("OBU Send can_frame failed, Invalid can interface...");
    addr.can_ifindex = get_can_if_index(can_fd, CAN_INTERFACE);
    addr.can_family  = AF_CAN;
//    qDebug("%d-%d-%d-%d-%d-%d-%d-%d", p_can_frame->data[0], p_can_frame->data[1], p_can_frame->data[2],
//            p_can_frame->data[3], p_can_frame->data[4], p_can_frame->data[5], p_can_frame->data[6],
//            p_can_frame->data[7]);
    nbytes = sendto(can_fd, p_can_frame, sizeof(struct can_frame),
                      0, (struct sockaddr*)&addr, len);
    if(-1 == nbytes){
        can_failed_count++;
//        qDebug("OBU Send can_frame failed, failed count %d, Frame id: %d...", can_failed_count, p_can_frame->can_id);
//        system("ip link set " CAN_INTERFACE " down");
//        system("ip link set " CAN_INTERFACE " up");
//        system("ip link set " CAN_INTERFACE " type can bitrate 250000");
    }
//    else{
//        can_send_frame_count++;
//        qDebug("CAN send frame count: %lld", can_send_frame_count);
//        qDebug("OBU Send can_frame(id: %x): %d Bytes...", p_can_frame->can_id, nbytes);
//    }
    if(p_can_frame){
        delete p_can_frame;
        p_can_frame = NULL;
    }
}

