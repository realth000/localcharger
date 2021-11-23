#include "qmlhandler.h"
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include "src/utils/networkinfohelper.h"

QmlHandler::QmlHandler(QObject *parent)
    : QObject(parent),
      m_sockerSenderIp(QStringLiteral(WEBSOCKET_SENDER_IP_DEFAULT)),
      m_socketSenderPort(WEBSOCKET_SENDER_PORT_DEFAULT),
      m_socketRecverUrl(QStringLiteral(WEBSOCKET_RECVER_URL_DEFAULT)),
      m_socketRecverPort(WEBSOCKET_RECVER_PORT_DEFAULT),
      m_socketSenderState(QmlSenderState::SenderDisconnected),
      m_socketRecverState(QmlRecverState::RecverDisconnected),
      m_ipTypeValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral(VALIDATOR_TYPE_IP_EXPRESSION)))),
      m_portTypeValidator(new QIntValidator(VALIDATOR_TYPE_PORT_MIN, VALIDATOR_TYPE_PORT_MAX)),
      m_configFilePath(QCoreApplication::applicationDirPath() + QStringLiteral(NATIVE_PATH_SEP) + QStringLiteral(APP_CONFIGFILE_NAME)),
      m_saveFileDirPath(QCoreApplication::applicationDirPath())
{

}

void QmlHandler::initHandler()
{
    loadDefaultConfig();
    loadConfig();
    initConnections();

    getLocalIp();

    updateSenderState(QmlSenderState::SenderDisconnected);
    updateRecverState(QmlRecverState::RecverDisconnected);
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
    m_socketRecverUrl.setHost(m_sockerSenderIp);
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

void QmlHandler::initConnections()
{
    // passing sender state
    connect(&m_socketSender, &WebSender::senderConnected, this, &QmlHandler::onSenderConnected);
    connect(&m_socketSender, &WebSender::senderDisconnected, this, &QmlHandler::onSenderDisconnected);

    // TOOD: transport files
    // passing sender message
//    connect(&m_socketSender, &WebSender::sendFileStart, this, &QmlHandler::onSendFileStart);
//    connect(&m_socketSender, &WebSender::sendFileFinish, this, &QmlHandler::onSendFileFinish);

    // passing recver state
    connect(&m_socketRecver, &WebRecver::recverConnected, this, &QmlHandler::onRecverConnected);
    connect(&m_socketRecver, &WebRecver::recverDisconnected, this, &QmlHandler::onRecverDisconnected);

    // TODO: transport files
    // passing recver message
//    connect(&m_socketRecver, &WebRecver::recvedMessage, this, &QmlHandler::recoredRecvedMsg);
//    connect(&m_socketRecver, &WebRecver::recvFileStart, this, &QmlHandler::onRecvFileStart);
//    connect(&m_socketRecver, &WebRecver::recvFileFinish, this, &QmlHandler::onRecvFileFinish);

    // clear info before send file
    connect(&m_socketSender, &WebSender::prepareRecvFile, &m_socketRecver, &WebRecver::onPrepareRecvFile);
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
        qDebug() << "config file not found, load default config";
        return;
    }
    const QSettings *configIni = new QSettings(m_configFilePath, QSettings::IniFormat);
    m_sockerSenderIp = configIni->value(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH)).toString();
    m_socketSenderPort = configIni->value(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH)).toInt();
    m_socketRecverPort = configIni->value(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH)).toInt();
    m_saveFileDirPath = configIni->value(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_RECVER_FILE_SAVE_PATH)).toString();
    delete configIni;
    emit qmlUpdateSocketConfig(m_sockerSenderIp, m_socketSenderPort, m_socketRecverPort);
}

void QmlHandler::saveConfig()
{
    QSettings *configIni = new QSettings(m_configFilePath, QSettings::IniFormat);
    configIni->setValue(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH), m_sockerSenderIp);
    configIni->setValue(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH), m_socketSenderPort);
    configIni->setValue(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH), m_socketRecverPort);
    configIni->setValue(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_RECVER_FILE_SAVE_PATH), m_saveFileDirPath);
    delete configIni;
}

void QmlHandler::getLocalIp()
{
    // Get local ip address and netmask
    // TODO: Support more than one ip and IPV6
    const QList<IpInfo> ipList = NetworkInfoHelper::getLocalIpAddress();
    QStringList ipStringList;
    foreach(IpInfo ip, ipList){
        ipStringList << QString("%1/%2").arg(ip.ipV4Address, QString::number(ip.prefixLength));
    }
    emit qmlUpdateLocalUrlLists(ipStringList);
}
