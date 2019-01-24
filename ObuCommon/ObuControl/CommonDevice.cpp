#include "CommonDevice.h"

QList<IRecvable *> CommonDevice::RegRecvs;
QList<QString> CommonDevice::RegTypes;

bool CommonDevice::RegChannel(IRecvable *recv, QString channelType)
{
    if(RegRecvs.contains(recv))
    {
        return false;
    }

    RegRecvs.append(recv);
    RegTypes.append(channelType);

    return true;
}

bool CommonDevice::UnregChannel(IRecvable *recv)
{
    if(RegRecvs.contains(recv))
    {
        int index = RegRecvs.indexOf(recv);
        RegRecvs.removeAt(index);
        RegTypes.removeAt(index);

        return true;
    }

    return false;
}

void CommonDevice::CallRegRecvs(QByteArray *buf, int &length, QString recvType)
{
    for(int i = 0; i < RegRecvs.length(); i++)
    {
        if(RegTypes[i].contains(recvType))
        {
            RegRecvs[i]->RegRecvCall(buf, length);
        }
    }
}

void CommonDevice::CallRegRecvs(QByteArray *buf, int &length)
{
    CallRegRecvs(buf, length, GetDesc());
}
