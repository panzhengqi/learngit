#ifndef OBUDEVICE_H
#define OBUDEVICE_H
#include <QString>
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QHostAddress>
#include "CommonDevice.h"

class ObuDevice: public CommonDevice
{
    Q_OBJECT
public:
    explicit ObuDevice(QObject *parent = 0);

private:
   QString m_loaclIP = "127.0.0.1";
   quint16 m_localPort = 11000;
   QString m_remoteIP = "127.0.0.1";
   quint16 m_remotePort = 11000;
   QUdpSocket *udp_socket = NULL;
   bool _isConnect = false;
   bool _isInit = false;

private :
    int readMessage( QByteArray *buf);

public:
    bool IsConnected();

    int Init(QVector<QString> params);
    int Open(QVector<QString> params);

    void Stop();

    int Send(QByteArray buf, QVector<QString> params);
    QList<QString> GetProperties();
    QString GetChannelType(){return "UdpChannel";}

    int Recv( QByteArray* buf, QVector<QString> params);
    int Close(QVector<QString> params);
    QString GetDesc();
    void Set_loaclIP(QString localIP) { m_loaclIP = localIP; }
    QString Get_loaclIP() const { return m_loaclIP; }


    void Set_loaclPort(quint16 localPort) { m_localPort = localPort; }
    quint16 Get_loaclPort() const { return m_localPort; }

    void Set_remoteIP(QString remoteIP) { m_remoteIP = remoteIP; }
    QString Get_remoteIP() const { return m_remoteIP; }

    void Set_remotePort(quint16 localIP) { m_remotePort = localIP; }
    quint16 Get_remotePort() const { return m_remotePort; }


};

#endif // OBUDEVICE_H
