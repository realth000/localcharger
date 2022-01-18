#include "daemon.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QRandomGenerator>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkInterface>
#include "utils/networkinfohelper.h"

LocalChargerDaemon::LocalChargerDaemon(QObject *parent)
    : QObject(parent),
      m_sockerSenderIp(QStringLiteral(WEBSOCKET_SENDER_IP_DEFAULT)),
      m_socketSenderPort(WEBSOCKET_SENDER_PORT_DEFAULT),
      m_socketRecverUrl(QStringLiteral(WEBSOCKET_RECVER_URL_DEFAULT)),
      m_socketRecverPort(WEBSOCKET_RECVER_PORT_DEFAULT),
      m_socketSenderState(SenderState::Disconnected),
      m_socketRecverState(RecverState::Disconnected),
      m_configFilePath(QCoreApplication::applicationDirPath() + QStringLiteral(NATIVE_PATH_SEP) + QStringLiteral(APP_CONFIGFILE_NAME)),
      m_saveFileDirPath(QCoreApplication::applicationDirPath()),
      m_enableAutoConnect(false),
      m_localClientReadableName("default"),
      m_localClientId(QRandomGenerator::securelySeeded().bounded(1000, 10000)),
      m_localWorkingPort(WEBSOCKET_PORT_DEFAULT)
{
    m_identifier = new WebIdentifier(m_localClientReadableName, m_localClientId, m_localWorkingPort, this);
    getLocalIp();
}

QString LocalChargerDaemon::getStatus()
{
    const QString ret = QString( "Sender status: %1\n"
                                 "Recver status: %2\n"
                                 "Local  IP:     %3\n"
                                 "Local  Port:   %4\n"
                                 "Remote Ip:     %5\n"
                                 "Remote Port:   %6\n")
                             .arg(getSenderStatus(), getRecverStatus(),
                                  m_localIp, QString::number(m_socketRecverPort),
                                  m_sockerSenderIp, QString::number(m_socketSenderPort));
    qDebug() << qPrintable(ret);
    return ret;
}

QString LocalChargerDaemon::getSenderStatus()
{
    switch (m_socketSenderState) {
    case SenderState::Disconnected:
        return QStringLiteral("Disconnected");
        break;
    case SenderState::Listening:
        return QStringLiteral("Listening");
        break;
    case SenderState::Connected:
        return QStringLiteral("Connected");
        break;
    default:
        return QStringLiteral("Unknown");
        break;
    }
}

QString LocalChargerDaemon::getRecverStatus()
{
    switch (m_socketRecverState) {
    case RecverState::Disconnected:
        return QStringLiteral("Disconnected");
        break;
    case RecverState::Connecting:
        return QStringLiteral("Connecting");
        break;
    case RecverState::Connected:
        return QStringLiteral("Connected");
        break;
    default:
        return QStringLiteral("Unknown");
        break;
    }
}

void LocalChargerDaemon::connectRemote(const QString &remotePath)
{
    qDebug() << "connect to remote" << remotePath;
    m_identifier->startAutoConnect(remotePath);
}

void LocalChargerDaemon::getLocalIp()
{
    const QList<IpInfo> ipList = NetworkInfoHelper::getLocalIpAddress();
    QStringList ipStringList;
    foreach(IpInfo ip, ipList){
        ipStringList << QString("%1/%2").arg(ip.ipV4Address, QString::number(ip.prefixLength));
    }
    // TODO: when have more than one ip
    if(ipList.length() > 0){
        m_identifier->setIdentityIp(ipList[0].ipV4Address);
        m_localIp = ipList[0].ipV4Address;
    }
}
