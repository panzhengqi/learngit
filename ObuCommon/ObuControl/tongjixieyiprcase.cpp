#include "tongjixieyiprcase.h"


TongjiXieyiPrcase::TongjiXieyiPrcase(QObject *parent) :
    QObject(parent)
{

}

char *  TongjiXieyiPrcase:: InCodePacket(char * buf ,int * len, uchar msgType)
{
    ushort datalen =0;
    if(buf ==NULL)
        return NULL;
    char * temp  =  new char[BUFFMATH];
    memset(temp ,0,BUFFMATH);
     temp[0] = 0x7E;
     temp[1]= msgType;
     if(msgType==0||msgType==1)
     {
          datalen = strlen(buf);
     }
     else
     {
         datalen = (ushort)*len;
     }
     UshortSmallDuan(datalen,temp+2);
     if(msgType == 3 || msgType == 4){
         memcpy(temp+4, buf, *len);
     }
     else{
         memcpy(temp+4,buf,strlen(buf));
     }

    ushort  checkSum = crc16_x25((uchar*)temp,datalen+4);
    UshortSmallDuan(checkSum, temp+datalen+4);
    temp[datalen+4+2]=0xFF;
    *len = datalen+7;
     return temp;

           /* if(buf ==NULL)
                return NULL;
            ushort datalen =0;
            char * temp  =  new char[BUFFMATH];
            memset(temp ,0,BUFFMATH);
             temp[0] = 0x7E;
             temp[1]= msgType;
             if(len>0)
             {
                 datalen = *len;
             }
             else
             {
                 datalen = strlen(buf);
             }
            UshortSmallDuan(datalen,temp+2);
            if(len>0)
            {
                memcpy(temp+4,buf,datalen);
            }
            else
            {
                memcpy(temp+4,buf,strlen(buf));
            }
            ushort  checkSum = crc16_x25((uchar*)temp,datalen+4);
            UshortSmallDuan(checkSum, temp+datalen+4);
            temp[datalen+4+2]=0xFF;
            printf("%s\n%d\n",buf,datalen);
            *len = datalen+7;
             return temp;*/
}

ushort TongjiXieyiPrcase::crc16_x25(uchar *buf, ushort buflen)
{
  unsigned short crc = CRC16_INITIAL_SEED;

  while( buflen-- ) {
    crc = ( crc >> 4 ) ^ crc16_x25_table[(crc & 0xf) ^ (*buf & 0xf)];
    crc = ( crc >> 4 ) ^ crc16_x25_table[(crc & 0xf) ^ (*buf++ >> 4)];
  }
  return (~crc);
}

void  TongjiXieyiPrcase::IntBigDuan(int value,char* temp)
{
       *temp= value>>24&0x000000FF;
       *(temp+1)= value>>16&0x000000FF;
      *(temp+1)= value>>8&0x000000FF;
       *(temp+1)= value&0x000000FF;
}

void   TongjiXieyiPrcase::UshortBigDuan(ushort value,char* temp)
{
//       temp= value>>24&0x000000FF;
//       temp++= value>>16&0x000000FF;
       *(temp)= value>>8&0x00FF;
       *(temp+1)= value&0x00FF;
}

void  TongjiXieyiPrcase::IntSmallDuan(int value,char* temp)
{
         *(temp)= value&0x000000FF;
          *(temp+1)= value>>8&0x000000FF;
          *(temp+1)= value>>16&0x000000FF;
          *(temp+1)= value>>24&0x000000FF;

}

void   TongjiXieyiPrcase::UshortSmallDuan(ushort value,char* temp)
{

        *temp= value&0x00FF;
       *( temp+1)= value>>8&0x00FF;


}

int  TongjiXieyiPrcase:: DeCodePacket(RecvMSgPacket *recvmsg)
{
  /*printf("DeCodePacket_in\n");
     QString str = recvmsg->p_mcu_msg;
     str.remove(QRegExp("\\s"));
     volatile int data_len = str.length();
     printf("DeCodePacket_str%d\n",data_len);
    QByteArray temp;

    ushort crc=0,  old_crc=0 , old_crc1 = 0;
   //ushort  len;
    if((data_len % 2) == 0)

    {
        StringToHex(str, temp);

        if(temp[0] == 0xA7)
        {
            if(temp[1]==2)
            {
             crc =crc16_x25((uchar*)temp.data(), data_len/2-3);

             old_crc =  (temp[data_len/2-2] << 8)|(temp[data_len/2-3]&0xFF);
 //            old_crc =old_crc | temp[data_len/2-2];
 //            old_crc = old_crc << 8;
 //            old_crc1 = temp[data_len/2-3] &0xFF;
 //            old_crc = old_crc | old_crc1;

 //            old_crc  = (old_crc )|(temp[data_len/2-3]);
 //            old_crc =  (temp[data_len/2-2]<<8);
             if(crc == old_crc)
             {
                         recvmsg->type =2;
                         recvmsg->p_mcu_msg[0]=(uchar)temp[5];
                         return 1;
             }

            }
            if(temp[1]==1)
            {
                crc =crc16_x25((uchar*)temp.data(), data_len/2-3);

                old_crc =  (temp[data_len/2-2] << 8)|(temp[data_len/2-3]&0xFF);
                if(crc == old_crc)
                {
                            recvmsg->type =1;
                            recvmsg->p_mcu_msg[0]=(uchar)temp[4];
                            return 1;
                }

            }
            return -1;

      */

   ushort crc=0,  old_crc=0  ;
  volatile int data_len =recvmsg->len;
  char temp = (char)recvmsg->p_mcu_msg[0];
  if(temp == (char)0xA7)
  {
      temp = (char)recvmsg->p_mcu_msg[1];
      if(temp == (char)0x02)
      {
          crc =crc16_x25((uchar*)recvmsg->p_mcu_msg, data_len-3);
          old_crc =  ((recvmsg->p_mcu_msg[data_len-2] << 8)&0xFF00)|(recvmsg->p_mcu_msg[data_len-3]&0xFF);
          if(crc == old_crc)
          {
                      recvmsg->type =2;
                      recvmsg->p_mcu_msg[0]=recvmsg->p_mcu_msg[5];
                      return 1;
          }
      }
      if(temp == (char)0x01)
      {
          crc =crc16_x25((uchar*)recvmsg->p_mcu_msg, data_len-3);
          old_crc =  ((recvmsg->p_mcu_msg[data_len-2] << 8)&0xFF00)|(recvmsg->p_mcu_msg[data_len-3]&0xFF);
          if(crc == old_crc)
          {
                      recvmsg->type =1;
                      recvmsg->p_mcu_msg[0]=recvmsg->p_mcu_msg[4];
                      recvmsg->p_mcu_msg[1] = recvmsg->p_mcu_msg[5];
                      return 1;
          }
      }
  }
  return 1;
}

int  TongjiXieyiPrcase:: DeCodePacket(char * buff ,int len)
{
    ushort datalen =0;
     ushort crc=0;
   //printf("Recv_TongjixieyiPacket:=%s\n",buff);
    char buf[256]={0};
    // sprintf(buf,"%s\n",buff);

    for(int i=0;i<len;i++)
    {
        if(buff[i]==0xA7)
        {
            if(buff[i+1]==2)
            {
                memcpy(&datalen,buff+i+2,2);
                memcpy(&crc,buff+i+4+datalen,2);

              //  printf("%RecvCrc_Vale=u:\n",crc);
              //  ushort crc1=  crc16_x25((uchar*)buff+i,4+datalen);
              //  printf("crc16_x25_res=%u:\n",crc1);

                if(crc ==crc16_x25((uchar*)buff+i,4+datalen))
                {
                    //IMC2OBU_MSG_EHNCRTL*  msgControl = new IMC2OBU_MSG_EHNCRTL;

                    //memset(msgControl,0,sizeof(IMC2OBU_MSG_EHNCRTL));
                    //memcpy(msgControl,buff+i+4,2);
                  //  msgControl.EHB_CTRL_Enablel =(uchar)buff[i+4];
                     //msgControl.EHB_Ctrl_value =(uchar)buff[i+5];
                   // printf("EHB_CTRL_Enablel:=%u,EHB_Ctrl_value:=%u\n",msgControl.EHB_CTRL_Enablel,msgControl.EHB_Ctrl_value);

                    i=i+7;
                }               
            }
             if(buff[i+1]==1)
             {
                     memcpy(&datalen,buff+i+2,2);
                     memcpy(&crc,buff+i+4+datalen,2);
                     printf("Recv_crc=%u:\n",crc);
                    ushort crc2=  crc16_x25((uchar*)buff+i,4+datalen);
                     printf("crc16_x25=%u:\n",crc2);
                     if(crc ==crc16_x25((uchar*)buff+i,4+datalen))
                     {
                             int len =0; char *temp=NULL;
                             temp = ConstructJsonString((char)buff[i+4],&len);
                             printf("Voice_packet%s\n",temp);
                              i=i+7;
                     }
             }

        }


    }


}
char TongjiXieyiPrcase::CovertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else
        return -1;
}

void TongjiXieyiPrcase::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;

    for(int i=0; i<len;)
    {
        hstr = str[i].toAscii();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toAscii();
        hexdata = CovertHexChar(hstr);
        lowhexdata = CovertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
        {
            break;
        }
        else
        {
            hexdata = hexdata*16+lowhexdata;
        }
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }

    senddata.resize(hexdatalen);
}
char* TongjiXieyiPrcase::ConstructJsonString(char data,int* len)
{
    Json::Value packet;
    Json::Value packet_data;
    int condition =0;
    packet["id"]  = Json::valueToString(condition);
    packet["type"] = Json::valueToString(Car_IsSelf);
    packet_data["voiceguid"] = Json::Value(data);
    printf("Voice_IdVale:%d\n",data);
    packet["data"] = packet_data;
    Json::StyledWriter strwriter;
    string str_packet = strwriter.write(packet);
    *len = (int)str_packet.length();
    char * buf = new char[256];
    memset(buf,0,256);
    memcpy(buf, str_packet.data(), str_packet.length());;

}
