#include "TcpServer.h"
#include "CommonSerialPort.h"


using namespace std;
TcpServer * Instantialize();
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t TcpServer::mutex;
TcpServer* TcpServer::pInstance =0;

TcpServer*  TcpServer::Instantialize()
{
    if(pInstance == NULL)
     {

        Lock lock(mutex);
        if(pInstance == NULL)
            pInstance = new TcpServer();
    }
    return pInstance;
}
TcpServer::TcpServer()
{

//    Init(port);
//    pthread_mutex_init(&Send_lock,NULL);
//    CreateTcpServer();
//    Bind();
//    Listen(5);
//    Run();


}

TcpServer::~TcpServer()
{
    for(int fd=0; fd<=max_fd; ++fd)
    {
        if(FD_ISSET(fd, &master_set))
        {
           close(fd);
        }
    }


}

void TcpServer::Init(int port)
{

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(port);
    pthread_mutex_init(&Send_lock,NULL);
    CreateTcpServer();
    Bind();
    Listen(5);
    Run();
    TcpServer_process_recv_thread    = new QThread();
    TcpServer_p_process_recv_obj     = new TcpServerProcessRecv();
    TcpServer_p_process_recv_obj->moveToThread(TcpServer_process_recv_thread);
    TcpServer_process_recv_thread->start();
}

void TcpServer::CreateTcpServer()
{
      listen_fd = socket(AF_INET,SOCK_STREAM,0);
      if(listen_fd <0)
          cout<<"Create TcpServer Socket Failed!"<<endl;
      int opt =1;
      setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
     // setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

}

void TcpServer::Bind()
{
    if(-1== bind(listen_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)))
            cout<<"Server bind failed\n!"<<endl;
    cout<< "bind Success!\n"<<endl;
}

void TcpServer::Listen(int len)
{
    listen(listen_fd,len);
}

void TcpServer::Accept()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int new_fd = accept(listen_fd,(struct sockaddr*)&client_addr,&client_addr_len);     
    string ip (inet_ntoa(client_addr.sin_addr));    
    mmap.insert(make_pair(new_fd,make_pair(ip,0)));
    FD_SET(new_fd,&master_set);
    if(new_fd>max_fd)
        max_fd = new_fd;
}

void TcpServer::Recv(int nums)
{

    for(int fd=0; fd<=max_fd; ++fd)
    {
        if(FD_ISSET(fd, &working_set)&&fd!=listen_fd)
        {
            bool close_conn = false;  // 标记当前连接是否断开了

            char buff[1024]={0};
            int len_recv=0;
            len_recv = recv(fd, buff, 1024, 0);   // 先接受包头
            if(len_recv <=0)
                continue;
           // printf("RecvMsgFrom_Tbox:%s\n",buff);
            RecvMec_msg * recvmsg = new RecvMec_msg;
            memset(recvmsg->p_mcu_msg,0,V2X_MAX_MSG_LENGTH);
            memcpy(recvmsg->p_mcu_msg,buff,len_recv);
            recvmsg->len = len_recv;
            emit RecvMsgFromMcuSendtoMec(recvmsg);

            if(close_conn)  // 当前这个连接有问题，关闭它
            {
                close(fd);
                FD_CLR(fd, &master_set);
                if(fd == max_fd)  // 需要更新max_fd;
                {
                    while(FD_ISSET(max_fd, &master_set) == false)
                        --max_fd;
                }
            }
        }
    }

}

void *  tcp_pth(void *arg)
{
    TcpServer* s1 = (TcpServer*)arg;
    s1->max_fd =  s1->listen_fd;   // 初始化max_fd
    FD_ZERO(& s1->master_set);
    FD_SET(s1->listen_fd,  &s1->master_set);  // 添加监听fd
    CommonSerialPort a;
    while(1)
    {
        FD_ZERO(& s1->working_set);
        memcpy(& s1->working_set, & s1->master_set, sizeof( s1->master_set));
        s1->timeout.tv_sec = 30;
        s1->timeout.tv_usec = 0;
        int nums = select(s1->max_fd+1, &s1->working_set, NULL, NULL, &s1->timeout);
        if(nums < 0)
        {
            cout << "select() error!";
          //  exit(1);
        }
        if(nums == 0)
        {//cout << "select() is timeout!";
            continue;
        }
        if(FD_ISSET( s1->listen_fd, & s1->working_set))
             s1->Accept();   // 有新的客户端请求

        else
             s1->Recv(nums); // 接收客户端的消息
    }

}

void TcpServer::Run()
{
//    pthread_t id;     // 创建心跳检测线程
//   int ret = pthread_create(&id, NULL, heart_handler, (void*)this);
//    if(ret != 0)
//   {
//        cout << "Can not create heart-beat checking thread.\n";
//   }
    pthread_t id_run;     // 创建心跳检测线程
   int ret = pthread_create(&id_run, NULL, tcp_pth, (void*)this);

}

void TcpServer::Send(char * msg,int data_len)
{
  //  pthread_mutex_lock(&Send_lock);

   // strncpy(buf_send,msg,data_len);
    for(int fd=0; fd<=max_fd; ++fd)
    {
        if(FD_ISSET(fd, &working_set)&&fd!=listen_fd)
        {
            bool close_conn = false;  // 标记当前连接是否断开了

          //  pthread_mutex_lock(&Send_lock);
            printf("SendMsgToTxBox:%s,packelen:%d\n",msg,data_len);
            int len = send(fd, msg, data_len, 0);
            if(len <=-1)
               continue ;
            // pthread_mutex_unlock(&Send_lock);
            if(close_conn)  // 当前这个连接有问题，关闭它
            {
                close(fd);
                FD_CLR(fd, &master_set);
                if(fd == max_fd)  // 需要更新max_fd;
                {
                    while(FD_ISSET(max_fd, &master_set) == false)
                        --max_fd;
                }
            }
        }
    }
  //  pthread_mutex_unlock(&Send_lock);
}


TcpServerProcessRecv::TcpServerProcessRecv()
{
    process_recv_timer = new QTimer(this);
    mcu_msg_queue      = new QQueue<RecvMec_msg *>;
    mcu_msg_queue->clear();
    connect((QObject *)process_recv_timer, SIGNAL(timeout()), this, SLOT(get_process_msg()));
    process_recv_timer->start(PROCESS_RECV_MSG_INTERVAL);
}


TcpServerProcessRecv::~TcpServerProcessRecv()
{
    if(process_recv_timer){
        if(process_recv_timer->isActive()){
            process_recv_timer->stop();
        }
        delete process_recv_timer;
        process_recv_timer = NULL;
    }
    if(mcu_msg_queue){
        if(!mcu_msg_queue->isEmpty()){
            mcu_msg_queue->clear();
        }
        delete mcu_msg_queue;
        mcu_msg_queue = NULL;
    }
}


void TcpServerProcessRecv::process_recv_msg()
{
    RecvMec_msg *p_x2v_msg = NULL;
    if(!mcu_msg_queue->isEmpty()){
        p_x2v_msg = mcu_msg_queue->dequeue();
    }
    if(p_x2v_msg){
       // do_process_recv_msg(p_x2v_msg);
        printf("RecvMecMsg:%s,PackLen%d\n",p_x2v_msg->p_mcu_msg,p_x2v_msg->len);
        TcpServer::Instantialize()->Send(p_x2v_msg->p_mcu_msg,p_x2v_msg->len);
        //emit send_McuMsg_to_TcpClient(p_x2v_msg);
//        if(p_x2v_msg){
//            memset(p_x2v_msg->p_mcu_msg, 0, sizeof(p_x2v_msg->p_mcu_msg));
//            delete p_x2v_msg;
//            p_x2v_msg = NULL;
//        }
    }
}


void TcpServerProcessRecv::do_process_recv_msg(RecvMec_msg *p_x2v_msg)
{

        //emit SendTo

//    string p_msg = p_x2v_msg->p_x2v_msg;
//    Json::Reader reader;
//    Json::Value  p_packet;
//    if(reader.parse(p_msg, p_packet)){
//        string Msg_type = p_packet["strMsgType"].asString();
//        if(!Msg_type.compare(msg_type_heartbeat_answer)){
//            long   result   = p_packet["result"].asInt();
//            if(!result)
//                emit clear_heartbeat(HEARTBEAT_EVENT_CLEAR);
//        }
//        else if(!Msg_type.compare(msg_type_lt_para_config_answer)){
//            long   result   = p_packet["result"].asInt();
//            if(!result){
//                light_control *p_light_control = new light_control;

//                p_light_control->command_type = COMMAND_CONFIG_LIGHT_PARA;
//                p_light_control->p_light_control_para.enType        = (int)p_packet["enType"].asInt();
//                p_light_control->p_light_control_para.lAzimuthAngle = (long)p_packet["lAzimuthAngle"].asInt();
//                p_light_control->p_light_control_para.emStartColor  = (int)p_packet["emStartColor"].asInt();
//                p_light_control->p_light_control_para.lGreenCount   = (long)p_packet["lGreenCount"].asInt();
//                p_light_control->p_light_control_para.lYelloCOunt   = (long)p_packet["lYelloCOunt"].asInt();
//                p_light_control->p_light_control_para.lRedCount     = (long)p_packet["lRedCount"].asInt();
//                emit send_command_to_light(p_light_control);    //ADD when combine code with light control thread
//                emit get_light_config(true);        //this signal may be moved to light thread later
//            }
//        }
//        else if(!Msg_type.compare(msg_type_lt_report_config_request)){
//            light_control *p_light_control = new light_control;
//            p_light_control->command_type = COMMAND_REPORT_LIGHT_STATE;
//            p_light_control->p_light_control_para.emOperFlag = (int)p_packet["emOperFlag"].asInt();
//            emit send_command_to_light(p_light_control);    //ADD when combine code with light control thread
//        }
//        else if(!Msg_type.compare(msg_type_lt_state_report_answer)){
//            /* do nothing here? */
//        }
//        else if(!Msg_type.compare(msg_type_lt_green_request)){
//            light_control *p_light_control = new light_control;
//            p_light_control->command_type = COMMAND_LIGHT_CONTROL;
//            p_light_control->p_light_control_para.lAzimuthAngle = (long)p_packet["lAzimuthAngle"].asInt();
//            p_light_control->p_light_control_para.emStartColor  = (int)p_packet["emStartColor"].asInt();
//            p_light_control->p_light_control_para.lGreenCount   = (long)p_packet["lGreenCount"].asInt();
//            emit send_command_to_light(p_light_control);    //ADD when combine code with light control thread
//        }
//        else
//            cout<<"Invalid Command or Response type..."<<endl;
//    }
//    else
//    {}





}


void TcpServerProcessRecv::accept_net_msg(RecvMec_msg* recv_mcuMsg)
{

  //   printf("%s\n",recv_mcuMsg->p_mcu_msg);
     mcu_msg_queue->enqueue(recv_mcuMsg);
}


void TcpServerProcessRecv::get_process_msg()
{
    process_recv_msg();
}


// thread function
void* heart_handler(void* arg)
{
    cout << "The heartbeat checking thread started.\n";
    TcpServer* s = (TcpServer*)arg;
    while(1)
    {
       s->mmap.begin();

        map<int, pair<string, int> >::iterator it = s->mmap.begin();
        for( ; it!=s->mmap.end(); )
        {
            if(it->second.second ==5)   // 3s*5没有收到心跳包，判定客户端掉线
            {
                cout << "The client " << it->second.first << " has be offline.\n";

                int fd = it->first;
                close(fd);            // 关闭该连接
                FD_CLR(fd, &s->master_set);
                if(fd == s->max_fd)      // 需要更新max_fd;
                {
                    while(FD_ISSET(s->max_fd, &s->master_set) == false)
                        s->max_fd--;
                }

                s->mmap.erase(it++);  // 从map中移除该记录
            }
            else if(it->second.second < 5 && it->second.second >= 0)
            {
                it->second.second += 1;
                ++it;
            }
            else
            {
                ++it;
            }
        }
        sleep(3);   // 定时三秒
    }
}

string TcpServer::HeartPacket(int id)
{
    Pack_ToPhone pack_tophone;
    pack_tophone.pack_id =id;
    pack_tophone.packType = 0;
    pack_tophone.data = "\0";
    pack_tophone.pack_len = 12+pack_tophone.data.length();
    return "sdfsf";
}
