#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <netinet/in.h>   // sockaddr_in
#include <sys/types.h>    // socket
#include <sys/socket.h>   // socket
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>   // select
#include <sys/ioctl.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tcpclientcommon.h"


using namespace std;
#define  BUFFER_SIZE 1024;

enum PackType {HEART,OTHER};

struct PACKET_HEAD
{
    PackType type;
    int length;

};


typedef struct  PACKET_TOPHONE
{
    int pack_id;
    int pack_len;
    int packType;
    string data;
}Pack_ToPhone;

void* heart_handler(void *arg);


class Lock
{
  private:
    pthread_mutex_t m_lock;
   public:
    Lock(pthread_mutex_t cs):m_lock(cs)
    {
        pthread_mutex_lock(&m_lock);
    }
    ~Lock()
    {
        pthread_mutex_unlock(&m_lock);

    }


};


class TcpServerProcessRecv : public QObject
{
    Q_OBJECT
public:
    TcpServerProcessRecv();
    ~TcpServerProcessRecv();

//    TcpComManager  *p_tcpcommanager;
public:
    void process_recv_msg();
    void do_process_recv_msg(RecvMec_msg *p_mcu_msg);

public slots:
    void accept_net_msg(RecvMec_msg* recv_mcuMsg );
    void get_process_msg();
signals:
     void send_Mcu_to_Udp(RecvMec_msg *p_mcu_msg);


private:
    QTimer            *process_recv_timer;
    QQueue<RecvMec_msg *> * mcu_msg_queue;
};


void *  tcp_pth(void *arg);
class TcpServer :public QObject
{
    Q_OBJECT
private:
    struct sockaddr_in   server_addr;
    socklen_t server_addr_len;
    int listen_fd;
    int max_fd;
    fd_set master_set;
    fd_set working_set;
    struct timeval  timeout;
    map <int, pair<string,int>  > mmap;
    int tcpserverport;
    TcpServer();
signals:
    void RecvMsgFromMcuSendtoMec(RecvMec_msg* recv_mcuMsg);

public:
    static TcpServer * Instantialize();
    static TcpServer *pInstance;
    TcpProcessRecv * tcp;
    static pthread_mutex_t mutex;
    ~TcpServer();
    void Init(int port);
    void Bind();
    void Listen(int len);
    void Accept();
    void Recv(int nums);
    void Run();
    void Send(char * msg,int data_len);
    void CreateTcpServer();
    string HeartPacket(int id);
    pthread_t id;
    friend void * heart_handler(void *arg);
    friend void *  tcp_pth(void *arg);
    pthread_mutex_t Send_lock;
    TcpProcessRecv*  GetTcpProcessRecv(TcpProcessRecv* tcp_recv)
    {
        tcp = tcp_recv;
    }
    QThread * TcpServer_process_recv_thread  ;
    TcpServerProcessRecv *TcpServer_p_process_recv_obj  ;
};

#endif // TCPSERVER_H
