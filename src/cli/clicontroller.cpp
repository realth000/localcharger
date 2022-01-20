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
        qInfo() << "Can not connect to service:" << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    m_daemonConnectionStatus = true;
}

QString CliController::getStatus() const
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return QString();
    }
    QDBusReply<QString> reply = m_daemonInterface.call(DAEMON_METHOD_GET_STATUS);
    if(!reply.isValid()){
        qInfo() << "Failed to get status: invalid reply";
        return QString();
    }
    qInfo("%s", qPrintable(reply.value()));
    return reply.value();
}

QString CliController::getSenderStatus() const
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return QString();
    }
    QDBusReply<QString> reply = m_daemonInterface.call(DAEMON_METHOD_GET_SENDER_STATUS);
    if(!reply.isValid()){
        qInfo() << "Failed to get sender status: invalid reply";
        return QString();
    }
    qInfo("Sender status: %s", qPrintable(reply.value()));
    return reply.value();
}

QString CliController::getRecverStatus() const
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return QString();
    }
    QDBusReply<QString> reply = m_daemonInterface.call(DAEMON_METHOD_GET_RECVER_STATUS);
    if(!reply.isValid()){
        qInfo() << "Failed to get recver status: invalid reply";
        return QString();
    }
    qInfo("Recver status: %s", qPrintable(reply.value()));
    return reply.value();
}

void CliController::exitDaemon() const
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return;
    }
    m_daemonInterface.call(DAEMON_METHOD_EXIT_DAEMON);
}

bool CliController::getDaemonConnectionStatus() const
{
    return m_daemonConnectionStatus;
}

void CliController::connectRemote(const QString &remotePath)
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return;
    }
    m_daemonInterface.call(DAEMON_METHOD_CONNECT_REMOTE, remotePath);
}

void CliController::sendMessage(const QString &msg)
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return;
    }
    m_daemonInterface.call(DAEMON_METHOS_SEND_MESSAGE, msg);
}
