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

QString CliController::getStatus() const
{
    if(!m_daemonConnectionStatus){
        qDebug() << "Daemon not connected";
        return QString();
    }
    QDBusReply<QString> reply = m_daemonInterface.call(DAEMON_METHOD_GET_STATUS);
    if(!reply.isValid()){
        qDebug() << "Failed to get status: invalid reply";
        return QString();
    }
    qDebug("%s", qPrintable(reply.value()));
    return reply.value();
}

QString CliController::getSenderStatus() const
{
    if(!m_daemonConnectionStatus){
        qDebug() << "Daemon not connected";
        return QString();
    }
    QDBusReply<QString> reply = m_daemonInterface.call(DAEMON_METHOD_GET_SENDER_STATUS);
    if(!reply.isValid()){
        qDebug() << "Failed to get sender status: invalid reply";
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
    QDBusReply<QString> reply = m_daemonInterface.call(DAEMON_METHOD_GET_RECVER_STATUS);
    if(!reply.isValid()){
        qDebug() << "Failed to get recver status: invalid reply";
        return QString();
    }
    qDebug("Recver status: %s", qPrintable(reply.value()));
    return reply.value();
}

bool CliController::getDaemonConnectionStatus() const
{
    return m_daemonConnectionStatus;
}
