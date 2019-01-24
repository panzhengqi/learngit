#include "ObuDevice.h"

ObuDevice::ObuDevice(QObject *parent)
{

}

bool ObuDevice::IsConnected()
{
    if(udp_socket == NULL)
        return false;
    return _isConnect;
}

int ObuDevice::Init(QVector<QString> params)
{
    if(params.isEmpty())
    {
        return -1;
    }
    Set_loaclIP(params[0]);
    Set_loaclPort(qint16(params[1].toInt()));
    Set_remoteIP(params[2]);
    Set_remotePort(qint16(params[3].toInt()));
    udp_socket = new QUdpSocket();
    _isInit =true;
    return 0;
}

int ObuDevice::Open(QVector<QString> params)
{
   if(!_isInit)
       return 1;
   if(udp_socket ==NULL)
       return -1;
   if(udp_socket->bind(QHostAddress::Any, m_localPort, QUdpSocket::ShareAddress))
   {
       _isConnect = true;
       return 0;
   }
   else
   {
       return -1;
   }
}

int ObuDevice::Send(QByteArray buf,QVector<QString> params)
{
    if(udp_socket == NULL)
        return -1;
    return udp_socket->writeDatagram(buf,buf.size(), QHostAddress(m_remoteIP), m_remotePort);
}

int ObuDevice::readMessage( QByteArray *buf)
{

    while(udp_socket->hasPendingDatagrams())
    {
        QHostAddress sender;
        quint16 senderPort;
        buf->resize(udp_socket->pendingDatagramSize());
        udp_socket->readDatagram(buf->data(), buf->size(), &sender, &senderPort);
        int len = buf->length();
        //CallRegRecvs(buf, len);
        return len;
    }
}

int ObuDevice::Recv( QByteArray *buf, QVector<QString> params)
{
    if(udp_socket == NULL)
        return -1;
    return readMessage(buf);
}

int ObuDevice::Close(QVector<QString> params)
{
    if(udp_socket == NULL)
    {
        return -1;
    }
    udp_socket->close();
    delete udp_socket;
    udp_socket = NULL;
    return 0;
}

QString ObuDevice::GetDesc()
{
    QString desc = "UdpClient";
    if (IsConnected())
    {
        desc = desc + "_" + m_remoteIP + ": " + QString::number(m_remotePort);
    }

    return desc;
}

QList<QString> ObuDevice::GetProperties()
{
    QList<QString> propertyNames;
    propertyNames << "LocalIP";
    propertyNames << "LocalPort";
    propertyNames << "RemoteIP";
    propertyNames << "RemotePort";

    return propertyNames;
}
