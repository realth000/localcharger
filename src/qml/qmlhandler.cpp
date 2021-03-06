#include "qmlhandler.h"
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QRandomGenerator>
#include <QtCore/QSettings>
#include <QtGui/QGuiApplication>
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QtAndroid>
#include <QtCore/QDir>
#endif
#include "src/utils/networkinfohelper.h"
#include "src/utils/filehelper.h"

using MsgType = WebSocketBinaryMessageType;

QmlHandler::QmlHandler(QObject *parent)
    : QObject(parent),
      m_socketSenderIp(QStringLiteral(WEBSOCKET_SENDER_IP_DEFAULT)),
      m_socketSenderPort(WEBSOCKET_SENDER_PORT_DEFAULT),
      m_socketRecverUrl(QStringLiteral(WEBSOCKET_RECVER_URL_DEFAULT)),
      m_socketRecverPort(WEBSOCKET_RECVER_PORT_DEFAULT),
      m_socketSenderState(QmlSenderState::SenderDisconnected),
      m_socketRecverState(QmlRecverState::RecverDisconnected),
      m_ipTypeValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral(VALIDATOR_TYPE_IP_EXPRESSION)))),
      m_portTypeValidator(new QIntValidator(VALIDATOR_TYPE_PORT_MIN, VALIDATOR_TYPE_PORT_MAX)),
#ifdef Q_OS_ANDROID
      m_configFilePath(APP_CONFIG_SAVE_PATH_ANDROID),
      m_saveFileDirPath(QStringLiteral(WEBSOCKET_RECVER_FILE_SAVE_PATH_ANDROID)),
#else
      m_configFilePath(QCoreApplication::applicationDirPath() + QStringLiteral(NATIVE_PATH_SEP) + QStringLiteral(APP_CONFIGFILE_NAME)),
      m_saveFileDirPath(QCoreApplication::applicationDirPath()),
      m_enableAutoConnect(false),
#endif
      m_clipBoard(QGuiApplication::clipboard()),
      m_localClientReadableName("default"),
      m_localClientId(QRandomGenerator::securelySeeded().bounded(1000, 10000)),
      m_localWorkingPort(WEBSOCKET_PORT_DEFAULT)
{

}

void QmlHandler::initHandler()
{
#ifdef Q_OS_ANDROID
    requestAndroidPermissions();

    QUrl fileSaveUrl(QString("file://%1").arg(WEBSOCKET_RECVER_FILE_SAVE_PATH_ANDROID));
    const QString workPath = fileSaveUrl.toLocalFile();
    QDir workDir(workPath);
    if(!workDir.exists()){
        emit qmlMessageInfo("mkdir1: "+QString::number(workDir.mkpath(workPath)));
    }
    emit qmlMessageInfo("exists?: "+QString::number(workDir.exists(workPath)));

#endif
    loadDefaultConfig();
    loadConfig();
    emit qmlUpdateSocketConfig(m_socketSenderIp, m_socketSenderPort, m_socketRecverPort);
    emit qmlUpdateFileSavePath(m_saveFileDirPath);
    emit qmlUpdateClientName(m_localClientReadableName);
    emit qmlUpdateClientId(m_localClientId);
    emit qmlUpdateClientAutoConnect(m_enableAutoConnect);
    m_identifier = new WebIdentifier(m_localClientReadableName, m_localClientId, m_localWorkingPort, this);
    initConnections();
    getLocalIp();
    updateSenderState(QmlSenderState::SenderDisconnected);
    updateRecverState(QmlRecverState::RecverDisconnected);
    m_identifier->boardcastIdentityMessage();
    qDebug() << "openssl lib state" << QSslSocket::sslLibraryBuildVersionNumber() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionNumber() << QSslSocket::sslLibraryVersionString();
}

void QmlHandler::startSender()
{
    stopSender();
    startSender(m_socketSenderPort);
}

void QmlHandler::startRecver()
{
    stopRecver();
    if(m_socketRecver.getRecverState() != QAbstractSocket::UnconnectedState){
        qDebug() << "Recver is already running" << m_socketRecver.recverUrl() << m_socketRecver.recverPort();
        return;
    }
    m_socketRecverUrl.setHost(m_socketSenderIp);
    m_socketRecverUrl.setPort(m_socketSenderPort);
    startRecver(m_socketRecverUrl);
}

void QmlHandler::onSenderConnected()
{
    updateSenderState(QmlSenderState::SenderConnected);
}

void QmlHandler::onSenderDisconnected()
{
    updateSenderState(QmlSenderState::SenderDisconnected);
}

void QmlHandler::onRecverConnected()
{
    updateRecverState(QmlRecverState::RecverConnected);
}

void QmlHandler::onRecverDisconnected()
{
    updateRecverState(QmlRecverState::RecverDisconnected);
}

void QmlHandler::sendMessage(const QString &msg)
{
    if(m_socketSenderState != QmlSenderState::SenderConnected){
        return;
    }
    if(msg.isEmpty()){
        return;
    }
    m_socketSender.sendMessage(msg);
    emit qmlClearToSendMsg();
}

void QmlHandler::sendFile(const QString &filePath)
{
    const QFileInfo fileInfo(filePath);
    if(!fileInfo.exists()){
        qDebug() << "send file failed:" << filePath << "not exists";
        return;
    }
    if(!fileInfo.isFile()){
        qDebug() << "send file failed:" << filePath << "is not a file";
        return;
    }
    resetProgressRecord();
    m_socketSender.notifyStart();
    m_socketSender.sendFile(filePath) ? qDebug() << "send file finish:" << filePath : qDebug() << "error sending file:" << filePath;
}

void QmlHandler::setSenderUrl(const QString &url)
{
    m_socketSenderIp = url;
}

void QmlHandler::setSenderPort(const QString &port)
{
    m_socketSenderPort = port.toInt();
}

void QmlHandler::setRecverPort(const QString &port)
{
    m_socketRecverPort = port.toInt();
}

void QmlHandler::setClientReadableName(const QString &name)
{
    m_localClientReadableName = name;
    m_identifier->setIdentityReadableName(m_localClientReadableName);
}

void QmlHandler::setFileSavePath(const QString &filePath)
{
    m_saveFileDirPath = filePath;
    m_socketRecver.setFileSavePath(filePath);
}

void QmlHandler::initConnections()
{
    // passing sender state
    connect(&m_socketSender, &WebSender::senderConnected, this, &QmlHandler::onSenderConnected);
    connect(&m_socketSender, &WebSender::senderDisconnected, this, &QmlHandler::onSenderDisconnected);

    // TOOD: transport files
    // passing sender message
    connect(&m_socketSender, &WebSender::sendFileStart, this, &QmlHandler::onSendFileStart);
    connect(&m_socketSender, &WebSender::sendFileFinish, this, &QmlHandler::onSendFileFinish);
    connect(&m_socketSender, &WebSender::sendFileFinish, this, &QmlHandler::updateTotalProgress);
    connect(&m_socketSender, &WebSender::sendFileFrameFinish, this, &QmlHandler::onSendFileFrameFinish);

    // passing recver state
    connect(&m_socketRecver, &WebRecver::recverConnected, this, &QmlHandler::onRecverConnected);
    connect(&m_socketRecver, &WebRecver::recverDisconnected, this, &QmlHandler::onRecverDisconnected);
    connect(&m_socketRecver, &WebRecver::recvFileFrameFinish, this, &QmlHandler::onRecvFileFrameFinish);

    // TODO: transport files
    // passing recver message
    connect(&m_socketRecver, &WebRecver::recvedMessage, this, &QmlHandler::onRecoredRecvedMsg);
    connect(&m_socketRecver, &WebRecver::recvFileStart, this, &QmlHandler::onRecvFileStart);
    connect(&m_socketRecver, &WebRecver::recvFileFinish, this, &QmlHandler::onRecvFileFinish);
    connect(&m_socketRecver, &WebRecver::recvFileFinish, this, &QmlHandler::updateTotalProgress);
    connect(&m_socketRecver, &WebRecver::resetProgress, this, &QmlHandler::resetProgressRecord);

    // clear info before send file
    connect(&m_socketSender, &WebSender::prepareRecvFile, &m_socketRecver, &WebRecver::onPrepareRecvFile);

    // WebIdentifier
    connect(m_identifier, &WebIdentifier::identityMessageParsed, this, &QmlHandler::onIdentityMessageParsed);
    connect(m_identifier, &WebIdentifier::getClientToConnect, this, &QmlHandler::autoConnectToClinet);
    connect(m_identifier, &WebIdentifier::getAutoConnectReply, this, &QmlHandler::onGetAutoConnectReply);

    // Close all sockets before app quit is need on Android
    connect(qApp, &QCoreApplication::aboutToQuit, this, &QmlHandler::closeAllSocket);
}

void QmlHandler::startSender(const port_t &port)
{
    if(m_socketSender.isSenderListening()){
        qDebug() << "Sender already listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort() << "stop first";
        stopSender();
    }

    if(m_socketSender.start(port)){
        qDebug() << "Sender start listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(QmlSenderState::SenderListening);
    }
    else{
        qDebug() << "Sender failed to listen" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(QmlSenderState::SenderDisconnected);
    }
}

void QmlHandler::stopSender()
{
    m_socketSender.stop();
    updateSenderState(QmlSenderState::SenderDisconnected);
}

void QmlHandler::startRecver(const url_t &url)
{
    m_socketRecver.stop();
    updateRecverState(QmlRecverState::RecverConnecting);
    m_socketRecver.start(url);
}

void QmlHandler::stopRecver()
{
    m_socketRecver.stop();
    updateRecverState(QmlRecverState::RecverDisconnected);
}

void QmlHandler::updateSenderState(QmlSenderState state)
{
    switch (state) {
    case QmlSenderState::SenderDisconnected:
        m_socketSenderState = QmlSenderState::SenderDisconnected;

        break;
    case QmlSenderState::SenderListening:
        m_socketSenderState = QmlSenderState::SenderListening;
        break;
    case QmlSenderState::SenderConnected:
        m_socketSenderState = QmlSenderState::SenderConnected;
        break;
    default:
        break;
    }
    emit qmlUpdateSenderState(m_socketSenderState);
}

void QmlHandler::updateRecverState(QmlRecverState state)
{
    switch (state) {
    case QmlRecverState::RecverDisconnected:
        m_socketRecverState = QmlRecverState::RecverDisconnected;
        break;
    case QmlRecverState::RecverConnecting:
        m_socketRecverState = QmlRecverState::RecverConnecting;
        break;
    case QmlRecverState::RecverConnected:
        m_socketRecverState = QmlRecverState::RecverConnected;
        break;
    default:
        break;
    }
    emit qmlUpdateRecverState(m_socketRecverState);
}

void QmlHandler::loadDefaultConfig()
{
    m_socketRecver.setFileSavePath(m_saveFileDirPath);
}

void QmlHandler::loadConfig()
{
    if(!QFileInfo::exists(m_configFilePath)){
        qDebug() << m_socketSenderIp << m_socketSenderPort << m_socketRecverPort;
        emit qmlUpdateSocketConfig(m_socketSenderIp, m_socketSenderPort, m_socketRecverPort);
        return;
    }
    const QSettings *configIni = new QSettings(m_configFilePath, QSettings::IniFormat);
    m_socketSenderIp = configIni->value(APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH).toString();
    m_socketSenderPort = configIni->value(APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH).toInt();
    m_socketRecverPort = configIni->value(APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH).toInt();
    m_saveFileDirPath = configIni->value(APP_CONFIGFILE_WEBSOCKET_RECVER_FILE_SAVE_PATH).toString();
    m_localClientReadableName = configIni->value(APP_CONFIGFILE_CLIENT_READABLENAME_PATH).toString();
    m_enableAutoConnect = configIni->value(APP_CONFIGFILE_CLINET_AUTOCONNECT_PATH).toBool();
    delete configIni;
    m_socketRecver.setFileSavePath(m_saveFileDirPath);
    m_localWorkingPort = m_socketRecverPort;
}

void QmlHandler::saveConfig()
{
    QSettings *configIni = new QSettings(m_configFilePath, QSettings::IniFormat);
    configIni->setValue(APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH, m_socketSenderIp);
    configIni->setValue(APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH, m_socketSenderPort);
    configIni->setValue(APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH, m_socketRecverPort);
    configIni->setValue(APP_CONFIGFILE_WEBSOCKET_RECVER_FILE_SAVE_PATH, m_saveFileDirPath);
    configIni->setValue(APP_CONFIGFILE_CLIENT_READABLENAME_PATH, m_localClientReadableName);
    configIni->setValue(APP_CONFIGFILE_CLINET_AUTOCONNECT_PATH, m_enableAutoConnect);
#ifdef Q_OS_ANDROID
    configIni->sync();
    emit qmlMessageInfo(QString::number(configIni->status()));
#endif
    delete configIni;
}

void QmlHandler::updateWebConfig()
{
    startSender();
    startRecver();
}

void QmlHandler::boardcastIdentityMessage()
{
    m_identifier->boardcastIdentityMessage();
}

void QmlHandler::connectSelectedClient(const QString &name, const QString &id, const QString &ip, const QString &port)
{
    Q_UNUSED(name)
    if(id.isEmpty() || ip.isEmpty() || port.isEmpty()){
        qDebug() << "empty client";
        return;
    }
    updateWebConfig();
    const url_t t(QString("wss://%1:%2").arg(ip, QString::number(12336)));
    if(!t.isValid()){
        qDebug() << "invalid url to identifier:" << t;
    }
    m_identifier->startAutoConnect(t);
}

void QmlHandler::setClipBoardText(const QString text)
{
    qDebug() << "clip board test" << text;
#ifdef Q_OS_ANDROID
    callAndroidToast("已复制到剪切板");
#endif
    m_clipBoard->setText(text);
}

void QmlHandler::setAutoConnect(const bool &isEnabled)
{
    m_enableAutoConnect = isEnabled;
    saveConfig();
}

void QmlHandler::sendDir(const QString &dirPath)
{
    if(dirPath.isEmpty()){
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
    resetProgressRecord();
    if(fileCount > 0) {
        m_fileTotalCount = fileCount;
    }
    dirVector.prepend(QFileInfo(dirPath).fileName());
    qInfo() << "all dirs:" << dirVector;
    m_socketSender.setRootPath(QFileInfo(dirPath).absoluteDir().absolutePath());
    m_socketSender.notifyStart(fileCount);
    m_socketSender.makeDir(dirVector);
    m_socketSender.sendDir(dirPath);
}

int QmlHandler::getSenderState()
{
    return m_socketSenderState;
}

void QmlHandler::closeAllSocket()
{
    m_socketSender.closeAllSocket();
    m_socketRecver.closeAllSocket();
}

void QmlHandler::getLocalIp()
{
    // Get local ip address and netmask
    // TODO: Support more than one ip and IPV6
#if defined(Q_OS_WINDOWS) || defined(Q_OS_WIN)
    const QList<IpInfo> ipList = NetworkInfoHelper::getLocalIpAddressEx();
#else
    const QList<IpInfo> ipList = NetworkInfoHelper::getLocalIpAddress();
#endif
    QStringList ipStringList;
    foreach(IpInfo ip, ipList){
        ipStringList << QString("%1/%2").arg(ip.ipV4Address, QString::number(ip.prefixLength));
    }
    emit qmlUpdateLocalUrlLists(ipStringList);
    // TODO: when have more than one ip
    if(ipList.length() > 0){
        m_identifier->setIdentityIp(ipList[0].ipV4Address);
        m_localIp = ipList[0].ipV4Address;
    }
}

void QmlHandler::addDetectedClients(const QString &ip, const QString &port, const QString &readableName, const QString &id)
{
    emit qmlAddClient(ip, port, readableName, id);
}

void QmlHandler::resetProgressRecord(const int fileCount)
{
    m_fileFinishedCount = 0;
    m_fileTotalCount = fileCount;
    emit qmlClearTransportProgress();
}

#ifdef Q_OS_ANDROID
void QmlHandler::requestAndroidPermissions()
{
    QtAndroid::PermissionResult READ_EXTERNAL_STORAGE = QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE");
    if(READ_EXTERNAL_STORAGE == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync(QStringList()<<"android.permission.READ_EXTERNAL_STORAGE");
    }
    QtAndroid::PermissionResult MANAGE_EXTERNAL_STORAGE = QtAndroid::checkPermission("android.permission.MANAGE_EXTERNAL_STORAGE");
    if(MANAGE_EXTERNAL_STORAGE == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync(QStringList()<<"android.permission.MANAGE_EXTERNAL_STORAGE");
    }
    QtAndroid::PermissionResult WRITE_EXTERNAL_STORAGE = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    if(WRITE_EXTERNAL_STORAGE == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync(QStringList()<<"android.permission.WRITE_EXTERNAL_STORAGE");
    }
}

void QmlHandler::callAndroidToast(const QString &message) {
    // all the magic must happen on Android UI thread
    QtAndroid::runOnAndroidThread([message] {
        QAndroidJniObject javaString = QAndroidJniObject::fromString(message);
        QAndroidJniObject toast = QAndroidJniObject::callStaticObjectMethod("android/widget/Toast", "makeText",
                                                                            "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
                                                                            QtAndroid::androidActivity().object(),
                                                                            javaString.object(),
                                                                            jint(0));
        toast.callMethod<void>("show");
    });
}
#endif

void QmlHandler::onRecoredRecvedMsg(const QString &msg)
{
    emit qmlAppendRecvedMessage(msg);
}

void QmlHandler::onSendFileStart(const QString &fielPath, const qint64 &fileSize)
{
    // TODO: send message here
    emit qmlAppendSendedMessage(QString("<font color=\"%3\">Sending file:</font>"
                                        " %1 "
                                        "<font color=\"%4\">(%2 bytes)</font>").
                                arg(fielPath, QString::number(fileSize), MSGSEND_TEXTEDIT_SENDING_HEAD_COLOR, MSGSEND_TEXTEDIT_SENDING_TAIL_COLOR));
}

void QmlHandler::onSendFileFinish(const QString &fielPath, const qint64 &sendBytes)
{
    // TODO: send message here
    emit qmlAppendSendedMessage(QString("<font color=\"%3\">File sended:</font>"
                                        " %1 "
                                        "<font color=\"%4\">(%2 bytes)</font>").
                                arg(fielPath, QString::number(sendBytes), MSGSEND_TEXTEDIT_SENDED_HEAD_COLOR, MSGSEND_TEXTEDIT_SENDED_TAIL_COLOR));

}

void QmlHandler::onSendFileFrameFinish(const QString &fileName, const qint64 &frameID, const qint64 &fileTotalFrameCount)
{
    emit qmlUpdateProgress(fileName, frameID, fileTotalFrameCount);
}

void QmlHandler::onRecvFileStart(const QString &fielPath, const qint64 &fileSize)
{
    emit qmlAppendRecvedMessage(QString("<font color=\"%3\">Recving file:</font>"
                                        " %1 "
                                        "<font color=\"%4\">(%2 bytes)</font>").
                                arg(fielPath, QString::number(fileSize), MSGRECV_TEXTEDIT_RECVING_HEAD_COLOR, MSGRECV_TEXTEDIT_RECVING_TAIL_COLOR));
}

void QmlHandler::onRecvFileFinish(const QString &fielPath, const qint64 &recvBytes)
{
    emit qmlAppendRecvedMessage(QString("<font color=\"%3\">File recvied:</font>"
                                        " %1 "
                                        "<font color=\"%4\">(%2 bytes)</font>").
                                arg(fielPath, QString::number(recvBytes), MSGRECV_TEXTEDIT_RECVED_HEAD_COLOR, MSGRECV_TEXTEDIT_RECVED_TAIL_COLOR));
}

void QmlHandler::onRecvFileFrameFinish(const QString &filePath, const qint64 &frameID, const qint64 &fileTotalFrameCount)
{
    emit qmlUpdateProgress(filePath, frameID, fileTotalFrameCount);
}

void QmlHandler::onIdentityMessageParsed(const QString &ip, const QString &port, const QString &readableName, const QString &id)
{
    qDebug() << ip << m_localIp << port << m_socketRecverPort;
    if(ip == m_localIp && port.toInt() == m_socketRecverPort){
        return;
    }
    if(!m_clientsMap.contains(id)){
        addDetectedClients(ip, port, readableName, id);
        m_clientsMap.insert(id, port);
        return;
    }
    if(m_clientsMap[id] == readableName){
        // TODO: update
        return;
    }
    addDetectedClients(ip, port, readableName, id);
    m_clientsMap.insert(id, port);
}

void QmlHandler::autoConnectToClinet(const QString &ip, const QString &port)
{
    // TODO: autoconnect config check
    if(!m_enableAutoConnect){
        qDebug() << "auto connect isdisabled";
        return;
    }
    if(ip.isEmpty() || port.isEmpty()){
        qDebug() << "empty cient to auto connect, ip =" << ip << "port =" << port;
        return;
    }
    emit qmlUpdateSenderIp(ip);
    m_socketSenderIp = ip;
    emit qmlUpdateSenderPort(port.toInt());
    m_socketSenderPort = port.toInt();
    updateWebConfig();
    m_identifier->sendAutoConnectReply();
}

void QmlHandler::onGetAutoConnectReply()
{
    startRecver();
}

void QmlHandler::updateTotalProgress()
{
    m_fileFinishedCount++;
    emit qmlUpdateTotalProgress(m_fileFinishedCount, m_fileTotalCount);
}
