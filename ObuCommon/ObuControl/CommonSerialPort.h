#ifndef COMMONSERIALPORT_H
#define COMMONSERIALPORT_H

#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include <termios.h>
#include "errno.h"
#include "string.h"
#include <string>
#include "pthread.h"
#include "sys/time.h"
#include "sys/epoll.h"
#include <list>
#include <vector>
#include "GpsAnalyse.h"


using namespace std;

typedef list<char *> RECVList;


#define  SERIALSIZE  255
#define  MAXLEN 1024

class CommonSerialPort: public QObject
{
     Q_OBJECT
public:
    CommonSerialPort();
    virtual ~CommonSerialPort();

    bool OpenCom(const int Com_port);//根据端口号打开com口，并创建接收线程
    bool SetCom(const int Baud_rate, const int Data_bits, char Parity, const int Stop_bits);//设置com口
    void ReadDataProc(char*dataBuff,const int BuffLen);//数据的处理函数。
    int ComWrite(char *WriteBuff,const int WriteLen);//com口写数据
    int ComRead(const int ReadLen);//com口读数据
    RECVList   list_rev;
public:
    void SetGpsObject( GpsAnalyse *gps_res)
    {
        gps = gps_res;
    }

private:
    int fd;//文件描述符
    int epid; //epoll标识符
    static pthread_mutex_t testlock;
    epoll_event event;
    epoll_event events[6];//事件集合
    char RecvBuff[MAXLEN];//接受到的数据
    pthread_t pid ;//接受数据线程的Id
    static void * ReadThreadFunction(void * arg);//接受数据的线程函数
    pthread_t pid_control ;//接受数据线程的Id    
    void ComClose();//关闭com口
    GpsAnalyse *gps;
signals:
    void SendToGpsAnlyse(char *buf,int len);


};
#endif /* CommonSerialPort_H_ */
