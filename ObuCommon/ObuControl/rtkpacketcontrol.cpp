#include "rtkpacketcontrol.h"

RtkPacketControl::RtkPacketControl(inner_recv_manager* recv_manger)
{

        car_rtk_Packet = new Car_RtkPacket;
        car_rtk_Packet_Front = new Car_RtkPacket;

        gpspro_parser = new GpsProtocolParser();
        connect(this,SIGNAL(SendToCarFrontMsgCan(can_msg*)),recv_manger, SLOT(accept_inner_msg(can_msg*)));
        rtkPacket_Control_timer = new QTimer(this);
        connect((QObject *)rtkPacket_Control_timer, SIGNAL(timeout()), this, SLOT(RtkPackControlThread()));

        rtkPacket_Control_timer->start(50);
}

RtkPacketControl::~RtkPacketControl()
{
    if(rtkPacket_Control_timer){
        if(rtkPacket_Control_timer->isActive()){
            rtkPacket_Control_timer->stop();
        }
        delete rtkPacket_Control_timer;
        rtkPacket_Control_timer = NULL;
    }
    if(car_rtk_Packet){
        if(!car_rtk_Packet->empty()){
            car_rtk_Packet->clear();
        }
        delete car_rtk_Packet;
        car_rtk_Packet =  NULL;
    }
    if(car_rtk_Packet){
        if(!car_rtk_Packet->empty()){
            car_rtk_Packet->clear();
        }
        delete car_rtk_Packet;
        car_rtk_Packet =  NULL;
    }
}

void RtkPacketControl::RecvListInsert(char *buf,int len)
{
   // char* recvbuf = new char[len];
     //memcpy(recvbuf,buf,len);
   //  printf("RecvListInsert\n");
     if(car_rtk_Packet->empty())
     {
         car_rtk_Packet->enqueue(buf);
        //LOG_INFO("Function_%s,lineNum_%d,%s\n", "RtkPacketControl::RecvListInsert",59,buf);
     }
     else
     {
       // LOG_INFO("Function_%s,lineNum_%d,%s\n", "RtkPacketControl::RecvListInsert",63,buf);
         char *temp  =NULL;
          temp  = car_rtk_Packet->dequeue();
         delete temp ;
         temp =NULL;
         car_rtk_Packet->enqueue(buf);
     }

}

void RtkPacketControl::RecvListInsert_Front(char *buf)
{

    if(car_rtk_Packet_Front->empty())
    {
       // LOG_INFO("Function_%s,lineNum_%d,%s\n","RtkPacketControl::RecvListInsert_Front",74,buf);
       car_rtk_Packet_Front->enqueue(buf);
       //printf("%s\n","RecvListInsert_Front");
    }
    else
    {
       //  printf("%s\n","RecvListInsert_Front_else");
        char *temp  =NULL;
         temp  = car_rtk_Packet_Front->dequeue();
        delete temp ;
        temp =NULL;
       // LOG_INFO("Function_%s,lineNum_%d,%s\n","RtkPacketControl::RecvListInsert_Front",81,buf);
         car_rtk_Packet_Front->enqueue(buf);
    }

}

void  RtkPacketControl::RtkPackControlThread()
{
    Json::Reader reader;
    Json::Value  p_packet;
    Json::Value p_packet_front;
   // string msg_type;
    if(car_rtk_Packet->empty()||car_rtk_Packet_Front->empty())
        return;
    KSXT_STRU gps_self;
    KSXT_STRU gps_front ;
    //GPS_INFO gps_info;
    memset(&gps_self, 0, sizeof(KSXT_STRU));
    memset(&gps_front, 0, sizeof(KSXT_STRU));
    string CarSelf_data;
    string FrontCar_data;
    char buf_self[rtk_msg_Len];
     char buf_Front[rtk_msg_Len];
    //char* msg_rtk = new char[1024];
  //  if(reader.parse(car_rtk_Packet->front(),p_packet))

    if(!car_rtk_Packet->empty())
    {
     // printf("%s\n","car_rtk_Packet_RtkPackControlThread");
      CarSelf_data = car_rtk_Packet->front();
    //  LOG_INFO("CarSelf_data=%s,CarSelf_dataLen=%d\n",CarSelf_data.data(),CarSelf_data.length());
      reader.parse(car_rtk_Packet->front(),p_packet);
      CarSelf_data = p_packet["data"]["extra"].asString();
      LOG_INFO("CarSelf_data=%s,CarSelf_dataLen=%d\n",CarSelf_data.data(),CarSelf_data.length());
      memset(buf_self,0,rtk_msg_Len);
      memcpy(buf_self,CarSelf_data.data(),CarSelf_data.length());
     // LOG_INFO("BUFF_Self%s\n",buf_self);
      gpspro_parser->GPS_KSXT_Parse(buf_self,&gps_self);
      car_rtk_Packet->pop_front();
      //printf("car_rtk_Packet%s\n",CarSelf_data.c_str());
     // LOG_INFO("RtkPacketControl::RtkPackControlThread_lineNum_%d,%s\n",104,CarSelf_data.data());
      //Log::get_instance()->write_log(1,"RtkPacketControl::RtkPackControlThread()_118\n");

    }
    if(!car_rtk_Packet_Front->empty())
    {
      // printf("%s\n","car_rtk_Packet_Front_RtkPackControlThread");
       FrontCar_data = car_rtk_Packet_Front->front();
       LOG_INFO("CarFront_data=%s,CarFront_dataLen=%d\n",FrontCar_data.data(),FrontCar_data.length());
       reader.parse(car_rtk_Packet_Front->front(),p_packet_front);
       FrontCar_data = p_packet_front["data"]["extra"].asString();
       LOG_INFO("CarFront_data=%s,CarFront_dataLen=%d\n",FrontCar_data.data(),FrontCar_data.length());
       memset(buf_Front,0,rtk_msg_Len);
       memcpy(buf_Front,FrontCar_data.data(),FrontCar_data.length());
       gpspro_parser->GPS_KSXT_Parse(buf_Front,&gps_front);
       LOG_INFO("BUff_Front%s\n",buf_Front);
       car_rtk_Packet_Front->pop_front();
      // Log::get_instance()->write_log(1,"RtkPacketControl::RtkPackControlThread()_130\n");
       LOG_INFO("gps_front.latitude:%.8f,gps_front.longitude:%.8f,gps_self.latitude:%.8f,gps_self.longitude:%.8f ",gps_front.latitude,gps_front.longitude,gps_self.latitude,gps_self.longitude);

       double distance = gpspro_parser->GetDistance(gps_front.latitude,gps_front.longitude,gps_self.latitude,gps_self.longitude);
       printf("distance_result:=%.8f\n",distance);

       LOG_INFO("distance_%.8f ",distance);
        float  speed = fabs(gps_front.speed - gps_self.speed);
        printf("speed_result:=%.8f\n",distance);
        LOG_INFO("Speed_Front_%.8f,speed_self_%.8f\n",gps_front.speed,gps_self.speed);
       // LOG_INFO("Distasnce_%lf,speed_%f,%s\n",distance,speed,"PackThread");
        //char *buf = new char[1024];
        can_msg * can_carFront = new can_msg;
        ConstructJsonString(distance,speed,can_carFront->msg);
        printf("%s\n",can_carFront->msg);
        LOG_INFO("Function_%s,lineNum_%d,%s\n" ,"RtkPacketControl::RtkPackControlThread",141,can_carFront->msg);
       //Log::get_instance()->write_log(1,"RtkPacketControl::RtkPackControlThread()_138\n");
        emit SendToCarFrontMsgCan(can_carFront);
     // Log::get_instance()->write_log(1,"RtkPacketControl::RtkPackControlThread()_138\n");
    }
}

void  RtkPacketControl::ConstructJsonString( double distance,float speed,char *buf)
{
    Json::Value packet;
    Json::Value packet_data;
    packet["id"]  = Json::valueToString(condition);
    packet["type"] = Json::valueToString(Car_Front_Info);
    packet_data["distance"] = Json::valueToString(distance);
    packet_data["speed"] = Json::valueToString(speed);
    packet["data"] = packet_data;
    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    memset(buf,0,Distance_Speed_Len);
    memcpy(buf, str_packet.data(), str_packet.length());;
}
