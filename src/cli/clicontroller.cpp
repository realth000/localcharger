#include "clicontroller.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QRandomGenerator>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtNetwork/QNetworkInterface>
#include "utils/networkinfohelper.h"

QDBusInterface CliController::m_daemonInterface(DAEMON_SERVICE_NAME, DAEMON_SERVICE_PATH, DAEMON_SERVICE_NAME, QDBusConnection::sessionBus());

CliController::CliController(QObject *parent)
    : QObject(parent),
      m_daemonConnectionStatus(false)
{
    if(!m_daemonInterface.isValid()){
        qDebug() << "Can not connect to service:" << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    m_daemonConnectionStatus = true;
}

void CliController::getLocalChargerStatus() const
{
//    qDebug()
//       << qPrintable(QString("Sender status: %1\n"
//                              "Recver status: %2\n"
//                              "Local  IP:   %3\n"
//                              "Local  Port: %4\n"
//                              "Remote Ip:   %5\n"
//                              "Remote Port: %6\n")
//                          .arg(getSenderStateStr(), getRecverStateStr(),
//                               m_localIp, QString::number(m_socketRecverPort),
//                               m_sockerSenderIp, QString::number(m_socketSenderPort))
//                    );
}

QString CliController::getSenderStatus() const
{
    if(!m_daemonConnectionStatus){
        qDebug() << "Daemon not connected";
        return QString();
    }
    QDBusReply<QString> reply = m_daemonInterface.call("getSenderStatus");
    if(!reply.isValid()){
        qDebug() << "Failed to get sender state: invalid reply";
        return QString();
    }
    qDebug("Sender status: %s", qPrintable(reply.value()));
    return reply.value();
}

QString CliController::getRecverStatus() const
{
    if(!m_daemonConnectionStatus){
        qDebug() << "Daemon not connected";
        return QString();
    }
    QDBusReply<QString> reply = m_daemonInterface.call("getRecverStatus");
    if(!reply.isValid()){
        qDebug() << "Failed to get recver state: invalid reply";
        return QString();
    }
    qDebug("Recver status: %s", qPrintable(reply.value()));
    return reply.value();
}

bool CliController::getDaemonConnectionStatus() const
{
    return m_daemonConnectionStatus;
}
