#include "UdpServer.h"
#include "json/json.h"

extern "C"
{
    #include "ltev2x.h"
}
using namespace std;

string  str_filePath = "/root/Config/Config.ini";

list  <string> send_list;

pthread_mutex_t testlock;

LTEV2X_HANDLE UdpServer::g_ltev2x_hdl;

int pth_state =0;
int id =0;
UdpServer::UdpServer()
{
   UdpInit();
}


int UdpServer::v2xInit()
{   
    LTEV2X_Option options;
    string v2x_str;
    int ret ;
    pthread_t thread_v2x_id = 0;
    pthread_t thread_RecvGps_id = 0;
    pthread_t thread_v2x_send2obu_id = 0;
    pthread_mutex_init(&testlock, NULL);
    ret = pthread_create(&thread_v2x_id, NULL, v2x_RecvMsg, (void*)this);
    if (ret)
    {
        printf("Open v2xRecvThread error\n！");
        return -1;
    }
    ret = pthread_create(&thread_v2x_send2obu_id, NULL, v2x_SendMsg2Obu, (void*)this);
    if (ret)
    {
        printf("Open  send2Ltev_Thread error\n！");
        return -1;
    }
    ret = pthread_create(&thread_RecvGps_id, NULL, recv_MsgFromAndriod, (void*)this);
    if (ret)
    {
        printf("Open UdpRecv_Thread error\n！");
        return -1;
    }
    //LTEV2X_Open();
  //  g_ltev2x_hdl = LTEV2X_Open();
    if(-1 == g_ltev2x_hdl)
    {
        printf("open ltev2x error!\n");
        return -1;
    }
    else
    {
        printf("OPen Ltev2x Sucess\n");
    }
    //config ltev2x
    dtconfig->GetValue("ue_id",v2x_str);
    options.ue_id = atoi(v2x_str.c_str());
    printf("%s\n",v2x_str.c_str());
    dtconfig->GetValue("ps_id",v2x_str);
    options.ps_id = atoi(v2x_str.c_str());
    dtconfig->GetValue("tx_power",v2x_str);
    options.tx_power = atoi(v2x_str.c_str());
    dtconfig->GetValue("priority",v2x_str);
    options.priority = atoi(v2x_str.c_str());
    dtconfig->GetValue("period",v2x_str);
    options.period = atoi(v2x_str.c_str());
    dtconfig->GetValue("proc_id",v2x_str);
    options.proc_id = atoi(v2x_str.c_str());
    dtconfig->GetValue("data_len",v2x_str);
    options.data_len = atoi(v2x_str.c_str());
    dtconfig->GetValue("test_mode",v2x_str);
    options.test_mode = atoi(v2x_str.c_str());
    dtconfig->GetValue("test_sfn",v2x_str);
    options.test_sfn = atoi(v2x_str.c_str());
//    if (0 != LTEV2X_SetOption(g_ltev2x_hdl, &options))
//    {
//        printf("set ltev2x error!\n");
//        return -1;
//    }
//    else
//    {
//        printf("Obu Init Success\n");
//        pth_state = 1;
//        return 0;
//    }


 }


void UdpServer::UdpInit()
{
    dtconfig = new DTConfig(str_filePath,true);
    string  str_temp;
    dtconfig->GetValue("ServerPort",str_temp);
    //dtconfig->GetValue("192.168.111.100");
   // cout<<str_temp<<endl;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(str_temp.c_str()));
    printf("%s\n",str_temp.c_str());
    // server_addr.sin_port = htons(4411);
    dtconfig->GetValue("ServerIp",str_temp);
    server_addr.sin_addr.s_addr = inet_addr(str_temp.c_str());
    //server_addr.sin_addr.s_addr = inet_addr("192.168.111.100");
  //  cout<<str_temp<<endl;
     printf("%s\n",str_temp.c_str());
    client_addr.sin_family = AF_INET;
    dtconfig->GetValue("LocalPort",str_temp);
  // cout<<str_temp<<endl;
    client_addr.sin_port = htons(atoi(str_temp.c_str()));
     printf("%s\n",str_temp.c_str());
    // client_addr.sin_port = htons(4410);
    dtconfig->GetValue("LocalIp",str_temp);
   //  cout<<str_temp<<endl;
    client_addr.sin_addr.s_addr = inet_addr(str_temp.c_str());
    //client_addr.sin_addr.s_addr = inet_addr("192.168.111.110");
     printf("%s\n",str_temp.c_str());
    client_addr_len = sizeof(client_addr);
    if((sock_fd = socket(AF_INET, SOCK_DGRAM, 0))<0)
    {
       perror("socket");
       exit(1);
    }
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
         perror("bind");
         exit(1);
     }
     int  nRecvBuf = 5*1024*1024;
     if(0 != setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, (const char*)&nRecvBuf, sizeof(nRecvBuf)))
     {
         printf("setsockopt SO_RCVBUF error !\n");
         exit(1);
         return ;
     }
     printf("Udp Init Success\n");
     v2xInit();
}


void* v2x_SendMsg2Obu(void *arg)
{
    UdpServer* s1 = (UdpServer*)arg;
    printf("Thread_Send_ToObu_LTEv start\n");
    int len_send =0;
    int packet_lenth=0;
    int i=0;
    while(1)
    {

        printf("send_list.size=%d\n",send_list.size());
        int size = send_list.size();
        if(size>=5)
        {
            printf("send_list.size=%d",send_list.size());
            pthread_mutex_lock(&testlock);
            for(i=0;i<5;i++)
            {
               // string str= s1->send_list.front();
                printf("SendTo OBU%s\n",send_list.front().c_str());
                packet_lenth = strlen(send_list.front().c_str());
                //len_send = LTEV2X_Send( s1->g_ltev2x_hdl,(void*)send_list.front().c_str(),packet_lenth);
                send_list.pop_front();
            }
            pthread_mutex_unlock(&testlock);
        }
     }
}


void  UdpServer::InsertSend_list(string str)
{

    send_list.push_back(str);
}

void  UdpServer::ConstructJsonString(char* buf)
{
    Json::Value packet;
    Json::Value packet_data;
    packet["id"]  = Json::Value("111");
    packet["type"] = Json::Value("0");
    packet_data["UUID"] = Json::Value("2");
    packet_data["isSelf"] = Json::Value("0");
    packet_data["extra"] = Json::Value(buf);
    packet["data"] = packet_data;
    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    memset(buf,0,1024);
    memcpy(buf, str_packet.c_str(), str_packet.length());
  //  printf("%s\n",buf);
}


void  UdpServer::ConstructJsonString_toUDP(char* buf)
{
    Json::Value packet;
    Json::Value packet_data;
    packet["id"]  = Json::Value("222");
    packet["type"] = Json::Value("0");
    packet_data["UUID"] = Json::Value("2");
    packet_data["isSelf"] = Json::Value("1");
    packet_data["extra"] = Json::Value(buf);
    packet["data"] = packet_data;
    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    memset(buf,0,1024);
    memcpy(buf, str_packet.c_str(), str_packet.length());
 //   printf("%s\n",buf);

}

void* v2x_RecvMsg(void *arg)
{
    int ret = 0;
    char buf[2048] = {0};
    int power = 0;
    int n =0;
    UdpServer* s_recv= (UdpServer*)arg;
    //printf("Thread_Recv_form_LTEv start\n");
    while(1)
    {
       memset(buf,0,2048);
       printf("Thread_Recv_form_LTEv\n");
       //ret = LTEV2X_Recv(s_recv->g_ltev2x_hdl, buf, 2048, &power);
       if(ret>0)
       {

            n= sendto(s_recv->sock_fd, buf, ret, 0, (struct sockaddr*)&s_recv->client_addr, s_recv->client_addr_len);
            printf("Obu_recv%d;%s\n",n,buf);
            if (n < 0)
            {
               printf("send To PC Failed");
            }
       }
    }
}


void* recv_MsgFromAndriod(void *arg)
{
     int n =0;
     UdpServer* recv_udp = (UdpServer*)arg;
     printf("Udp recv Thread Msg \n");
     int packet_lenth = 0;
     int len_send = 0;
     char buf[1024]={0};
     while(1)
     {

          memset(buf,0,1024);
          printf("Udp Recv _in\n");
          //n = recvfrom(recv_udp->sock_fd, buf, 1024, 0, (struct sockaddr*)&recv_udp->client_addr, &recv_udp->client_addr_len);
          if(n>0)
          {
             // printf("%s\n",buf);
              char * buffer = new char[1024];
              memcpy(buffer,buf,1024);
              recv_udp->ConstructJsonString_toUDP(buf);
              n= sendto(recv_udp->sock_fd, buf, strlen(buf), 0, (struct sockaddr*)&recv_udp->client_addr, recv_udp->client_addr_len);
              pthread_mutex_lock(&testlock);
              recv_udp->ConstructJsonString(buffer);
              recv_udp->InsertSend_list(buffer);
              printf("Udp_send_list.size=%d\n",send_list.size());
              if( send_list.size()>5)
              {
                  printf("send_list.size=%d",send_list.size());
               //  pthread_mutex_lock(&s1->testlock);
                  for(int i=0;i<5;i++)
                  {
                      string str = send_list.front();
                      packet_lenth = str.length();
                      //len_send = LTEV2X_Send( recv_udp->g_ltev2x_hdl,(void*)send_list.front().c_str(), packet_lenth);
                      if(len_send >0)
                          printf("OBU_Send%d\n",len_send);
                      send_list.pop_front();
                  }
              }
              pthread_mutex_unlock(&testlock);
          }
     }

}
