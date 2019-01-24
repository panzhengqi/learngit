#ifndef COMMONDEVICE_H
#define COMMONDEVICE_H
#include <QVector>
#include <QList>
#include <QObject>

class CommonDevice : public QObject
{   
public:
    virtual ~CommonDevice(){}
    virtual bool IsConnected() = 0;
    virtual int Init(QVector<QString> params) = 0;
    virtual int Open(QVector<QString> params) = 0;
    virtual int Send(QByteArray buf, QVector<QString> params) = 0;
    virtual int Recv(QByteArray *buf, QVector<QString> params) = 0;
    virtual int Close(QVector<QString> params) = 0;
    virtual QString GetDesc() = 0;
    virtual QList<QString> GetProperties() = 0;
    virtual QString GetChannelType() = 0;

};

#endif // COMMONDEVICE_H

