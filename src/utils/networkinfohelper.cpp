#include "networkinfohelper.h"
#if defined(Q_OS_WINDOWS) || defined(Q_OS_WIN)
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#endif

NetworkInfoHelper::NetworkInfoHelper()
{

}

int NetworkInfoHelper::netmaskToPrefixLength(const QString &netmask)
{
    int prefixLength = 0;
    const QStringList list = netmask.split(".");
    foreach(const auto &l, list){
        int t = l.toInt();
        while(t > 0){
            if(t & 0x1){
                prefixLength++;
            }
            t = t >> 1;
        }
    }
    return prefixLength;
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

#if defined(Q_OS_WINDOWS) || defined(Q_OS_WIN)
QList<IpInfo> NetworkInfoHelper::getLocalIpAddressEx()
{
    QList<IpInfo> ipAddressList;

    PIP_ADAPTER_INFO pAdapterInfo;
    pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
    ULONG buflen = sizeof(IP_ADAPTER_INFO);

    if(GetAdaptersInfo(pAdapterInfo, &buflen) == ERROR_BUFFER_OVERFLOW){
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) malloc(buflen);
    }

    if(GetAdaptersInfo(pAdapterInfo, &buflen) == NO_ERROR){
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while (pAdapter){
            if(QString(pAdapter->IpAddressList.IpAddress.String) != "0.0.0.0" && QString(pAdapter->GatewayList.IpAddress.String) != "0.0.0.0"){
                ipAddressList.append(IpInfo(pAdapter->IpAddressList.IpAddress.String, netmaskToPrefixLength(pAdapter->IpAddressList.IpMask.String)));
            }
            pAdapter = pAdapter->Next;
        }
    }
    free(pAdapterInfo);
    return ipAddressList;
}
#endif
