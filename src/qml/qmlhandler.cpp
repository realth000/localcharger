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
      m_socketSenderState(SenderState::Disconnected),
      m_socketRecverState(RecverState::Disconnected),
      m_ipTypeValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral(VALIDATOR_TYPE_IP_EXPRESSION)))),
      m_portTypeValidator(new QIntValidator(VALIDATOR_TYPE_PORT_MIN, VALIDATOR_TYPE_PORT_MAX)),
      m_configFilePath(QCoreApplication::applicationDirPath() + QStringLiteral(NATIVE_PATH_SEP) + QStringLiteral(APP_CONFIGFILE_NAME)),
      m_saveFileDirPath(QCoreApplication::applicationDirPath())
{

}

void QmlHandler::initHandler()
{
    emit qmlMessageInfo("123");

    loadDefaultConfig();
    loadConfig();
    getLocalIp();
    updateSenderState(SenderState::Disconnected);
    updateRecverState(RecverState::Disconnected);
}

void QmlHandler::startSender(const port_t &port)
{
    if(m_socketSender.isSenderListening()){
        qDebug() << "Sender already listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort() << "stop first";
        stopSender();
    }

    if(m_socketSender.start(port)){
        qDebug() << "Sender start listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(SenderState::Listening);
    }
    else{
        qDebug() << "Sender failed to listen" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(SenderState::Disconnected);
    }
}

void QmlHandler::stopSender()
{
    m_socketSender.stop();
    updateSenderState(SenderState::Disconnected);
}

void QmlHandler::startRecver(const url_t &url)
{
    m_socketRecver.stop();
    updateRecverState(RecverState::Connecting);
    m_socketRecver.start(url);
}

void QmlHandler::stopRecver()
{
    m_socketRecver.stop();
    updateRecverState(RecverState::Disconnected);
}

void QmlHandler::updateSenderState(SenderState state)
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
    emit qmlUpdateSenderState(m_socketSenderState);
}

void QmlHandler::updateRecverState(RecverState state)
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
