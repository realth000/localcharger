#include "networkinfohelper.h"

NetworkInfoHelper::NetworkInfoHelper()
{

}

QList<IpInfo> NetworkInfoHelper::getLocalIpAddress()
{
    QList<IpInfo> ret;
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface i, list){
        const QNetworkInterface::InterfaceFlags flags = i.flags();
        if(!(flags & i.IsUp) || !(flags & i.IsRunning) || (flags & i.IsLoopBack)){
            continue;
        }
        foreach(QNetworkAddressEntry entry, i.addressEntries()){
            if(entry.ip().toIPv4Address() != 0 && !entry.ip().toString().endsWith(".1")){
                ret.append(IpInfo(entry.ip().toString(), entry.prefixLength()));
            }
        }
    }
    return ret;
}
