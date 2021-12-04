#ifndef NETWORKINFOHELPER_H
#define NETWORKINFOHELPER_H
#include <QtCore/QList>
#include <QtNetwork/QNetworkInterface>

struct IpInfo{
    IpInfo(QString ipV4Address, int prefixLength):
          ipV4Address(ipV4Address),
          prefixLength(prefixLength){};
    QString ipV4Address;
    int prefixLength;
};

class NetworkInfoHelper
{
public:
    NetworkInfoHelper();
    static int netmaskToPrefixLength(const QString &netmask);
    static QList<IpInfo> getLocalIpAddress();
    static QList<IpInfo> getLocalIpAddressEx();
};

#endif // NETWORKINFOHELPER_H
