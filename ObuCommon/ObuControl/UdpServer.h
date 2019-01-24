#ifndef UDPSERVER_H
#define UDPSERVER_H

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
#include <list>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <pthread.h>
#include "ltev2x.h"
#include "config.h"

using namespace std;

void* v2x_RecvMsg(void* arg);
void* v2x_SendMsg2Obu(void *arg);
void* recv_MsgFromAndriod(void *arg);


class UdpServer
{
public:
    UdpServer();
    int v2xInit();
    void UdpInit();
    void SendMsgToObu();
    //void InsertSendMap()
    void  InsertSend_list(string str);
    void  ConstructJsonString(char* buf);
    void  ConstructJsonString_toUDP(char* buf);
    DTConfig*   dtconfig;
private:
    struct sockaddr_in   server_addr;
    socklen_t server_addr_len;
    struct sockaddr_in  client_addr;
    socklen_t client_addr_len;
    int sock_fd;
    //extern int errcode;
    static LTEV2X_HANDLE g_ltev2x_hdl;
   // static pthread_mutex_t testlock;

    list <string> recv_list;


    friend void* v2x_RecvMsg(void* arg);
    friend void* v2x_SendMsg2Obu(void *arg);
    friend void* recv_MsgFromAndriod(void *arg);
};

#endif // UDPSERVER_H
