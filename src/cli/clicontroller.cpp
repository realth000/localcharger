#include "clicontroller.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QRandomGenerator>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtNetwork/QNetworkInterface>
#include "utils/networkinfohelper.h"

QDBusInterface CliController::m_daemonInterface(DAEMON_SERVICE_NAME, DAEMON_SERVICE_PATH, DAEMON_SERVICE_NAME, QDBusConnection::sessionBus());

CliController::CliController(QObject *parent)
    : QObject(parent),
      m_daemonConnectionStatus(false),
      m_taskName(""),
      m_process(-3),
#ifndef DISABLE_UPDATE_PROGRESS_BY_TIMER
      m_processTimer(),
#endif
     m_posChar("|/-\\"),
     m_posTimes(0)
{
    if(!m_daemonInterface.isValid()){
        qInfo() << "Can not connect to service:" << QDBusConnection::sessionBus().lastError().message();
        return;
    }
    m_daemonConnectionStatus = true;
#ifndef DISABLE_UPDATE_PROGRESS_BY_TIMER
    m_processTimer.setSingleShot(false);
    connect(&m_processTimer, &QTimer::timeout, this,
            [this](){
                // FIXME: Property here not works
                // FIXME: valiation check always fails
#if 1
                QDBusReply<QString> nameReply = m_daemonInterface.call("getSendFileName");
                m_taskName = nameReply.value();
                QDBusReply<int> progressReply = m_daemonInterface.call("getSendFileProgress");
                m_process = progressReply.value();
#else
                QVariant nameReply = m_daemonInterface.property("sendFileName");
                m_taskName = nameReply.toString();
                QVariant progressReply = m_daemonInterface.property("sendFileProgress");
                m_process = progressReply.toInt();
#endif
                qDebug() << m_taskName << m_process << nameReply << progressReply;
                printProcess(m_taskName, m_process);
                if(m_process >= 100){
                    m_processTimer.stop();
                    QCoreApplication::exit(0);
                }
            });
#endif
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

int CliController::getSenderStatusCode() const
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return static_cast<int>(SenderState::Disconnected);
    }
    QDBusReply<int> reply = m_daemonInterface.call(DAEMON_METHOD_GET_SENDER_STATUS_CODE);
    if(!reply.isValid()){
        qInfo() << "Failed to get recver status: invalid reply";
        return reply.value();
    }
    return static_cast<int>(reply.value());
}

int CliController::getRecverStatusCode() const
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return static_cast<int>(RecverState::Disconnected);
    }
    QDBusReply<int> reply = m_daemonInterface.call(DAEMON_METHOD_GET_RECVER_STATUS_CODE);
    if(!reply.isValid()){
        qInfo() << "Failed to get recver status: invalid reply";
        return reply.value();
    }
    return static_cast<int>(RecverState::Disconnected);
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

void CliController::sendFile(const QString &filePath)
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return;
    }
//    m_processTimer.start(1000);
    m_daemonInterface.asyncCall(DAEMON_METHOD_SEND_FILE, filePath);
}

void CliController::sendDir(const QString &dirPath)
{
    if(!m_daemonConnectionStatus){
        qInfo() << "Daemon not connected";
        return;
    }
    m_daemonInterface.asyncCall(DAEMON_METHOD_SEND_DIR, dirPath);
}

void CliController::updateSendProgress(const QString &fileName, const int &fileProgress)
{
    printf("\33[?25l[%s %3d%%] %s\r", getProcessBarChunk(fileProgress).toStdString().c_str(), fileProgress, fileName.toStdString().c_str());
    fflush(stdout);
    if(fileProgress >= 100){
        printf("\n");
    }
}

void CliController::exitCli(const int &exitCode)
{
    QCoreApplication::exit(exitCode);
}

void CliController::insertChunkCursor(QString &chunk, const int &pos)
{
    if(m_posTimes > 3){
        m_posTimes = 0;
    }
    chunk[pos-1] = m_posChar[m_posTimes];
    m_posTimes++;
}

QString CliController::getProcessBarChunk(const int &process)
{
    QString ret(10, ' ');
    if(process < 10){
        insertChunkCursor(ret, 1);
        return ret;
    }
    int pos = 1;
    for(; pos <= process/10; pos++){
        ret[pos-1] = '=';
    }
    if(process < 100){
        insertChunkCursor(ret, pos);
    }
    return ret;
}

#ifndef DISABLE_UPDATE_PROGRESS_BY_TIMER
void CliController::printProcess(const QString &taskName, const int &process)
{
    printf("%s  %d\r", taskName.toStdString().c_str(), process);
    fflush(stdout);
}
#endif
