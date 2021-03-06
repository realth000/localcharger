#include "daemon.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QRandomGenerator>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkInterface>
#include "filehelper.h"
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
      m_enableAutoConnect(true),
      m_cliInterface(CLI_SERVICE_NAME, CLI_SERVICE_PATH, CLI_SERVICE_NAME, QDBusConnection::sessionBus()),
      m_cliDBusConnected(m_cliInterface.isValid()),
      m_sendFileName(""),
      m_sendFileProcess(-1),
      m_localClientReadableName("default"),
      m_localClientId(QRandomGenerator::securelySeeded().bounded(1000, 10000)),
      m_localWorkingPort(WEBSOCKET_PORT_DEFAULT)
{
    if(!m_cliDBusConnected){
        qInfo() << "Warning: Can NOT connect to cli dbus service, running without connection" << QDBusConnection::sessionBus().lastError().message();
    }
    m_identifier = new WebIdentifier(m_localClientReadableName, m_localClientId, m_localWorkingPort, this);
    getLocalIp();
    initConnections();
    m_identifier->boardcastIdentityMessage();
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
    qInfo() << qPrintable(ret);
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

int LocalChargerDaemon::getSenderStatusCode()
{
    return static_cast<int>(m_socketSenderState);
}

int LocalChargerDaemon::getRecverStatusCode()
{
    return static_cast<int>(m_socketRecverState);
}

void LocalChargerDaemon::exitDaemon()
{
    exit(0);
}

void LocalChargerDaemon::connectRemote(const QString &remotePath)
{
    qInfo() << "connect to remote" << remotePath;
    qDebug() << "after split" << remotePath.split(":") << "all" << remotePath;
    // NOTE: remotePath here had been checked with pattern [ip:port].
    const url_t t(QString("wss://%1:%2").arg(remotePath.split(":")[0], QString::number(IDENTIFIER_UDP_LISTION_PORT)));
    const url_t testUrl(QString("wss://%1").arg(remotePath));
    if(!t.isValid()){
        qInfo() << "invalid url to identifier:" << t;
    }
    m_identifier->startAutoConnect(t);
    if(!testUrl.isValid()){
        qInfo() << "invalid url" << testUrl.toString();
        return;
    }
    m_sockerSenderIp = testUrl.url();
    m_socketSenderPort = testUrl.port();
    m_socketRecverUrl = testUrl;
    stopSender();
    startSender(m_socketSenderPort);
    stopRecver();
    if(m_socketRecver.getRecverState() != QAbstractSocket::UnconnectedState){
        qInfo() << "Recver is already running" << m_socketRecver.recverUrl() << m_socketRecver.recverPort();
        return;
    }
    else{
        startRecver(m_socketRecverUrl);
    }
}

void LocalChargerDaemon::sendMessage(const QString &msg)
{
    if(m_socketSenderState != SenderState::Connected){
        return;
    }
    if(msg.isEmpty()){
        return;
    }
    m_socketSender.sendMessage(msg);
}

void LocalChargerDaemon::sendFile(const QString &filePath)
{
    if(m_socketSenderState != SenderState::Connected){
        return;
    }
    m_socketSender.sendFile(filePath);
    m_cliInterface.call(CLI_METHOD_EXIT_CLI, 0);
}

void LocalChargerDaemon::sendDir(const QString &dirPath)
{
    if(m_socketSenderState != SenderState::Connected){
        return;
    }
    qint64 fileCount = 0;
    qint64 totalSize = 0;
    dir_lists dirVector;
    if(!FileHelper::checkDirectoryInfo(dirPath, fileCount, totalSize, dirVector)){
        qInfo() << "Error checking directory:" << dirPath;
        return;
    }
    // test
    qInfo("Check dir %s: fileCount=%lld, totalSize=%lld", dirPath.toStdString().c_str(), fileCount, totalSize);
    dirVector.prepend(QFileInfo(dirPath).fileName());
    qInfo() << "all dirs:" << dirVector << QFileInfo(dirPath).fileName();
    // Set local root path
    m_socketSender.setRootPath(QFileInfo(dirPath).absoluteDir().absolutePath());
    // Let remote make directory
    m_socketSender.makeDir(dirVector);
    m_socketSender.sendDir(dirPath);
    m_cliInterface.call(CLI_METHOD_EXIT_CLI, 0);
}

#ifndef DISABLE_UPDATE_PROGRESS_BY_TIMER
QString LocalChargerDaemon::getSendFileName()
{
    return m_sendFileName;
}

int LocalChargerDaemon::getSendFileProgress()
{
    return m_sendFileProcess;
}
#endif

void LocalChargerDaemon::initConnections()
{
    // passing sender state
    connect(&m_socketSender, &WebSender::senderConnected, this, &LocalChargerDaemon::onSenderConnected);
    connect(&m_socketSender, &WebSender::senderDisconnected, this, &LocalChargerDaemon::onSenderDisconnected);
    // passing sender message
    connect(&m_socketSender, &WebSender::sendFileStart, this, &LocalChargerDaemon::onSendFileStart);
//    connect(&m_socketSender, &WebSender::sendFileFinish, this, &LocalChargerDaemon::onSendFileFinish);
    connect(&m_socketSender, &WebSender::sendFileFrameFinish, this, &LocalChargerDaemon::onSendFileFrameFinish);

    // passing recver state
    connect(&m_socketRecver, &WebRecver::recverConnected, this, &LocalChargerDaemon::onRecverConnected);
    connect(&m_socketRecver, &WebRecver::recverDisconnected, this, &LocalChargerDaemon::onRecverDisconnected);
    // passing recver message
    connect(&m_socketRecver, &WebRecver::recvedMessage, this, &LocalChargerDaemon::recoredRecvedMsg);
    connect(&m_socketRecver, &WebRecver::recvFileStart, this, &LocalChargerDaemon::onRecvFileStart);
    connect(&m_socketRecver, &WebRecver::recvFileFinish, this, &LocalChargerDaemon::onRecvFileFinish);

    // clear info before send file
    connect(&m_socketSender, &WebSender::prepareRecvFile, &m_socketRecver, &WebRecver::onPrepareRecvFile);

    // WebIdentifier
    connect(m_identifier, &WebIdentifier::identityMessageParsed, this, &LocalChargerDaemon::onIdentityMessageParsed);
    connect(m_identifier, &WebIdentifier::getClientToConnect, this, &LocalChargerDaemon::autoConnectToClinet);
    connect(m_identifier, &WebIdentifier::getAutoConnectReply, this, &LocalChargerDaemon::onGetAutoConnectReply);
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

void LocalChargerDaemon::startSender(const port_t &port)
{
    if(m_socketSender.isSenderListening()){
        qInfo() << "Sender already listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort() << "stop first";
        stopSender();
    }

    if(m_socketSender.start(port)){
        qInfo() << "Sender start listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(SenderState::Listening);
    }
    else{
        qInfo() << "Sender failed to listen" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(SenderState::Disconnected);
    }
}

void LocalChargerDaemon::stopSender()
{
    m_socketSender.stop();
}

void LocalChargerDaemon::updateSenderState(SenderState state)
{
    switch (state) {
    case SenderState::Disconnected:
        m_socketSenderState = SenderState::Disconnected;
        break;
    case SenderState::Listening:
        m_socketSenderState = SenderState::Listening;
        break;
    case SenderState::Connected:
        m_socketSenderState = SenderState::Connected;
        break;
    default:
        break;
    }
}

void LocalChargerDaemon::startRecver(const url_t &url)
{
    m_socketRecver.stop();
    updateRecverState(RecverState::Connecting);
    m_socketRecver.start(url);
}

void LocalChargerDaemon::stopRecver()
{
    m_socketRecver.stop();
    updateRecverState(RecverState::Disconnected);
}

void LocalChargerDaemon::updateRecverState(RecverState state)
{
    switch (state) {
    case RecverState::Disconnected:
        m_socketRecverState = RecverState::Disconnected;
        break;
    case RecverState::Connecting:
        m_socketRecverState = RecverState::Connecting;
        break;
    case RecverState::Connected:
        m_socketRecverState = RecverState::Connected;
        break;
    default:
        break;
    }
}

void LocalChargerDaemon::autoConnectToClinet(const QString &ip, const QString &port)
{
    // TODO: autoconnect config check
    if(!m_enableAutoConnect){
        qInfo() << "auto connect isdisabled";
        return;
    }
    if(ip.isEmpty() || port.isEmpty()){
        qInfo() << "empty cient to auto connect, ip =" << ip << "port =" << port;
        return;
    }
    const url_t testUrl(QString("wss://%1:%2").arg(ip, port));
    if(!testUrl.isValid()){
        qInfo() << "invalid url" << testUrl.toString();
        return;
    }
    m_socketRecverUrl = testUrl;
    stopSender();
    startSender(m_socketSenderPort);
    stopRecver();
    if(m_socketRecver.getRecverState() != QAbstractSocket::UnconnectedState){
        qInfo() << "Recver is already running" << m_socketRecver.recverUrl() << m_socketRecver.recverPort();
    }
    else{
        startRecver(m_socketRecverUrl);
    }
    m_identifier->sendAutoConnectReply();
}

void LocalChargerDaemon::onIdentityMessageParsed(const QString &ip, const QString &port, const QString &readableName, const QString &id)
{
    qInfo() << ip << m_localIp << port << m_socketRecverPort;
    if(ip == m_localIp && port.toInt() == m_socketRecverPort){
        return;
    }
    if(!m_clientsMap.contains(id)){
//        addDetectedClients(ip, port, readableName, id);
        m_clientsMap.insert(id, port);
        return;
    }
    if(m_clientsMap[id] == readableName){
        // TODO: update
        return;
    }
//    addDetectedClients(ip, port, readableName, id);
    m_clientsMap.insert(id, port);
}

void LocalChargerDaemon::onGetAutoConnectReply()
{
    stopRecver();
    if(m_socketRecver.getRecverState() != QAbstractSocket::UnconnectedState){
        qInfo() << "Recver is already running" << m_socketRecver.recverUrl() << m_socketRecver.recverPort();
        return;
    }
    startRecver(m_socketRecverUrl);
}

void LocalChargerDaemon::onSenderConnected()
{
    qInfo() << "sender connected";
    updateSenderState(SenderState::Connected);
}

void LocalChargerDaemon::onSenderDisconnected()
{
    m_socketSender.isSenderListening() ? updateSenderState(SenderState::Listening) : updateSenderState(SenderState::Disconnected);
}

void LocalChargerDaemon::onRecverConnected()
{
    updateRecverState(RecverState::Connected);
}

void LocalChargerDaemon::onRecverDisconnected()
{
    updateRecverState(RecverState::Disconnected);
}

void LocalChargerDaemon::onSendFileStart(const QString &filePath, const qint64 &fileSize)
{
//    qInfo() << QString("Sending file:%1(%2)").arg(filePath, fileSize);
    m_sendFileName = filePath;
}

void LocalChargerDaemon::onSendFileFinish(const QString &filePath, const qint64 &fileSize)
{
    qInfo() << QString("File sended:%1(%2)").arg(filePath, fileSize);
}

void LocalChargerDaemon::onSendFileFrameFinish(const QString fileName, const qint64 frameID, const qint64 fileTotalFrameCount)
{
    m_sendFileName = fileName;
    m_sendFileProcess = 100*frameID/fileTotalFrameCount;
    qInfo() << "Update send file progress" << m_sendFileName << m_sendFileProcess;
    m_cliInterface.call(CLI_METHOD_UPDATE_SEND_FILE_PROGRESS, m_sendFileName, m_sendFileProcess);
}

void LocalChargerDaemon::onRecvFileStart(const QString &filePath, const qint64 &fileSize)
{
    qInfo() << QString("Recving file:%1(%2)").arg(filePath, fileSize);
}

void LocalChargerDaemon::onRecvFileFinish(const QString &filePath, const qint64 &fileSize)
{
    qInfo() << QString("File recvied:%1(%2)").arg(filePath, fileSize);
}

void LocalChargerDaemon::recoredRecvedMsg(const QString &msg)
{
    qInfo() << QString("Message recvied: %1").arg(msg);
}
