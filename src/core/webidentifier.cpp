#include "webidentifier.h"
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QRandomGenerator>

#define IDENTITY_HEADER 0x4B9ACA00
#define IDENTITY_AUTOCONNECT_HEADER 0x6A027C31
#define IDENTITY_AUTOCONNECT_MESSAGE_TYPE_LENGTH 1
#define IDENTITY_HEADER_LENGTH 10
#define IDENTITY_ID_LENGTH 4
#define IDENTITY_IP_LENGTH 16
#define IDENTITY_WORKINGPORT_LENGTH 5
#define IDENTITY_READABLENAME_LENGTH 16

WebIdentifier::WebIdentifier(const QString &identityReadableName, const int &identityId, const port_t &socketWorkingPort, QObject *parent) :
    QObject(parent),
    m_identityReadableName(identityReadableName),
    m_identityId(identityId),
    m_socketWorkingPort(socketWorkingPort),
    m_socketServer(new QWebSocketServer("identifyServer", QWebSocketServer::SecureMode, this)),
    m_inSocket(nullptr)
{
    m_identifierSocket.bind(IDENTIFIER_UDP_PORT, QUdpSocket::ShareAddress);
    connect(&m_identifierSocket, &QUdpSocket::readyRead, this, &WebIdentifier::onIdenReadReady);

    // Setup socket server
    QSslConfiguration sslConfigure;
    QFile certFile(WEBSOCKET_CONFIG_CERTFILT_PATH);
    QFile keyFile(WEBSOCKET_CONFIG_KEYFILE_PATH);
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfigure.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfigure.setLocalCertificate(certificate);
    sslConfigure.setPrivateKey(sslKey);
    sslConfigure.setProtocol(QSsl::TlsV1SslV3);
    m_socketServer->setSslConfiguration(sslConfigure);
    connect(m_socketServer, &QWebSocketServer::newConnection, this, &WebIdentifier::onNewConnection, Qt::UniqueConnection);
    startListenPort(IDENTIFIER_UDP_LISTION_PORT) ? qInfo() << "WebIdentifier: started listening" << IDENTIFIER_UDP_LISTION_PORT
                          : qInfo() << "WebIdentifier: start failed";

    // Setup m_outSocket
    connect(&m_outSocket, &QWebSocket::connected, this, &WebIdentifier::onOutSocketConnected, Qt::UniqueConnection);
    connect(&m_outSocket, &QWebSocket::disconnected, this, &WebIdentifier::onOutSocketDisconnected, Qt::UniqueConnection);
    connect(&m_outSocket, &QWebSocket::sslErrors, this, &WebIdentifier::onOutSocketSslErrors, Qt::UniqueConnection);
    connect(&m_outSocket, &QWebSocket::binaryMessageReceived, this, &WebIdentifier::onOutSocketBinaryMessageReceived, Qt::UniqueConnection);
}

WebIdentifier::~WebIdentifier()
{
    m_socketServer->close();
    m_outSocket.close();
}

void WebIdentifier::setIdentityReadableName(const QString &readableName)
{
    m_identityReadableName = readableName;
}

void WebIdentifier::setIdentityId(const int &id)
{
    m_identityId = id;
}

void WebIdentifier::setIdentityIp(QString ip)
{
    m_identityIp = ip;
}

void WebIdentifier::setWorkingPort(const port_t &port)
{
    m_socketWorkingPort = port;
}

void WebIdentifier::sendAutoConnectReply()
{
    QByteArray message;
    message.append(QString(IDENTITY_AUTOCONNECT_HEADER).toUtf8(), IDENTITY_HEADER_LENGTH);
    message.append(QString::number(static_cast<int>(AutoConnectMessageType::AcknowledgeToConnect)).toUtf8(), IDENTITY_AUTOCONNECT_MESSAGE_TYPE_LENGTH);
    m_inSocket->sendBinaryMessage(message);
}

void WebIdentifier::boardcastIdentityMessage()
{
    QByteArray datagram = generateIdentidyData(m_identityReadableName, m_socketWorkingPort);
    qInfo() << "boardcast: name" << m_identityReadableName << "port" << m_socketWorkingPort;
    m_identifierSocket.writeDatagram(datagram, QHostAddress::Broadcast, IDENTIFIER_UDP_PORT);
}

void WebIdentifier::openUrl(const url_t &url)
{
    m_outSocket.open(url);
}

void WebIdentifier::startAutoConnect(const url_t &url)
{
    if(m_outSocket.state() != QAbstractSocket::UnconnectedState && m_outSocket.state() != QAbstractSocket::ClosingState){
        m_outSocket.abort();
    }
    m_outSocket.open(url);
    qInfo() << "connecting" << url;
}

void WebIdentifier::onIdenReadReady()
{
    QByteArray datagram;
    QHostAddress senderAddress;
    while (m_identifierSocket.hasPendingDatagrams()) {
        datagram.resize(int(m_identifierSocket.pendingDatagramSize()));
        m_identifierSocket.readDatagram(datagram.data(), datagram.size(), &senderAddress);
        qint64 head = QString::fromUtf8(datagram.left(IDENTITY_HEADER_LENGTH)).toInt();
        if(head != IDENTITY_HEADER){
            qInfo() << "WebIdentifier: unreadable udp datagram head" << head;
            continue;
        }
        qint64 offset = IDENTITY_HEADER_LENGTH;
        QString senderId = QString::fromUtf8(datagram.mid(offset, IDENTITY_ID_LENGTH));
        offset += IDENTITY_ID_LENGTH;
        QString senderIp = QString::fromUtf8(datagram.mid(offset, IDENTITY_IP_LENGTH));
        offset += IDENTITY_IP_LENGTH;
        QString senderWorkingPort = QString::fromUtf8(datagram.mid(offset, IDENTITY_WORKINGPORT_LENGTH));
        offset += IDENTITY_WORKINGPORT_LENGTH;
        QString senderReadableName = QString::fromUtf8(datagram.mid(offset));
        // filt datagram from itself
        if(senderId == m_identityId && senderReadableName == m_identityReadableName){
            continue;
        }
        emit identityMessageParsed(senderIp, senderWorkingPort, senderReadableName, senderId);
        qInfo() << QString("ientify client: ip=%1, workingPort=%2, name=%3, id=%4")
                    .arg(senderIp, senderWorkingPort, senderReadableName, senderId);
    }
}

void WebIdentifier::onNewConnection()
{
    QWebSocket *pSocket = m_socketServer->nextPendingConnection();
    pSocket->setPauseMode(QAbstractSocket::PauseOnSslErrors);
    connect(pSocket, &QWebSocket::disconnected, this, &WebIdentifier::socketDisconnected, Qt::UniqueConnection);
    if(m_inSocket != nullptr){
        delete m_inSocket;
    }
    m_inSocket = pSocket;
    qInfo() << "identifier setup in socket";
    // Setup m_inSocket
    connect(m_inSocket, &QWebSocket::connected, this, &WebIdentifier::onInSocketConnected, Qt::UniqueConnection);
    connect(m_inSocket, &QWebSocket::sslErrors, this, &WebIdentifier::onInSocketSslErrors, Qt::UniqueConnection);
    // Without Qt::QueuedConnection, connection will become apart when transporting files divided into two frames or more
    // Without Qt::UniqueConnection, connection will be multiple when restart connecting without changing socket config
    connect(m_inSocket, &QWebSocket::binaryMessageReceived, this, &WebIdentifier::onInSocketBinaryMessageReceived, static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
}

void WebIdentifier::socketDisconnected()
{
    qInfo() << "Client disconnected";
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient != nullptr){
        m_inSocket = nullptr;
        pClient->deleteLater();
    }
}

void WebIdentifier::onSslErrors(const QList<QSslError> &errors)
{
    qInfo() << "Ssl errors occurred in WebIdentifier";
    foreach(QSslError error, errors){
        qInfo() << error.errorString();
    }
}

void WebIdentifier::onInSocketConnected()
{
    qInfo() << "identifier in socket connected";
}

void WebIdentifier::onInSocketBinaryMessageReceived(const QByteArray &message)
{
    int offset = 0;
    QString autoConnectHeader = QString::fromUtf8(message.mid(0, IDENTITY_HEADER_LENGTH));
    if(autoConnectHeader != IDENTITY_AUTOCONNECT_HEADER){
        qInfo() << "WebIdentifier in socket: not a autoconnect header:" << autoConnectHeader;
        return;
    }
    offset += IDENTITY_HEADER_LENGTH;
    const AutoConnectMessageType messageType = static_cast<AutoConnectMessageType>(QString::fromUtf8(message.mid(offset, IDENTITY_AUTOCONNECT_MESSAGE_TYPE_LENGTH)).toInt());
    if(messageType != AutoConnectMessageType::RequestToConnect){
        qInfo() << "WebIdentifier: in socket: message type incorret value =" << messageType;
        return;
    }
    offset += IDENTITY_AUTOCONNECT_MESSAGE_TYPE_LENGTH;
    QString ip = QString::fromUtf8(message.mid(offset, IDENTITY_IP_LENGTH));
    offset += IDENTITY_IP_LENGTH;
    QString port = QString::fromUtf8(message.mid(offset, IDENTITY_WORKINGPORT_LENGTH));
    qInfo() << "WebIdentifier in socket: get clinet to auto connect" << ip << port;
    emit getClientToConnect(ip, port);
}

void WebIdentifier::onInSocketSslErrors(const QList<QSslError> &errors)
{
    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    foreach(QSslError error, errors){
        qInfo() << "WebIdentifier: out socket ssl error:" << error.errorString();
    }
    m_inSocket->ignoreSslErrors();
}

void WebIdentifier::onOutSocketConnected()
{
    qInfo() << "identifier out socket connected" << m_outSocket.localAddress() << m_outSocket.localPort();
    QByteArray message;
    message.append(QString(IDENTITY_AUTOCONNECT_HEADER).toUtf8(), IDENTITY_HEADER_LENGTH);
    message.append(QString::number(static_cast<int>(AutoConnectMessageType::RequestToConnect)).toUtf8(), IDENTITY_AUTOCONNECT_MESSAGE_TYPE_LENGTH);
    message.append(m_identityIp.toUtf8(), IDENTITY_IP_LENGTH);
    message.append(QString::number(m_socketWorkingPort).toUtf8(), IDENTITY_WORKINGPORT_LENGTH);
    m_outSocket.sendBinaryMessage(message);
}

void WebIdentifier::onOutSocketDisconnected()
{

}

void WebIdentifier::onOutSocketBinaryMessageReceived(const QByteArray &message)
{
    qInfo() << "1222222222222";
    int offset = 0;
    QString autoConnectHeader = QString::fromUtf8(message.mid(0, IDENTITY_HEADER_LENGTH));
    if(autoConnectHeader != IDENTITY_AUTOCONNECT_HEADER){
        qInfo() << "WebIdentifier in socket: not a autoconnect header:" << autoConnectHeader;
        return;
    }
    offset += IDENTITY_HEADER_LENGTH;
    const AutoConnectMessageType messageType = static_cast<AutoConnectMessageType>(QString::fromUtf8(message.mid(offset, IDENTITY_AUTOCONNECT_MESSAGE_TYPE_LENGTH)).toInt());
    if(messageType == AutoConnectMessageType::AcknowledgeToConnect){
        emit getAutoConnectReply();
        return;
    }
    else{
        qInfo() << "WebIdentifier: out socket: unknown message type when pharsing auto-connect-reply" << messageType;
        return;
    }
}

void WebIdentifier::onOutSocketSslErrors(const QList<QSslError> &errors)
{
    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    foreach(QSslError error, errors){
        qInfo() << "WebIdentifier: out socket ssl error:" << error.errorString();
    }
    m_outSocket.ignoreSslErrors();
}

QByteArray WebIdentifier::generateIdentidyData(const QString &identityReadableName, const port_t &socketWorkingPort)
{
    QByteArray identityByteArray;
    identityByteArray.append(QString::number(IDENTITY_HEADER).toUtf8(), IDENTITY_HEADER_LENGTH);
    identityByteArray.append(QString::number(m_identityId).toUtf8(), IDENTITY_ID_LENGTH);
    identityByteArray.append(m_identityIp.toUtf8(), IDENTITY_IP_LENGTH);
    identityByteArray.append(QString::number(socketWorkingPort).toUtf8(), IDENTITY_WORKINGPORT_LENGTH);
    identityByteArray.append(identityReadableName.toUtf8());
    return identityByteArray;
}

bool WebIdentifier::startListenPort(port_t port)
{
    if(!m_socketServer->listen(QHostAddress::Any, port)){
        return false;
    }
    connect(m_socketServer, &QWebSocketServer::newConnection, this, &WebIdentifier::onNewConnection, Qt::UniqueConnection);
    connect(m_socketServer, &QWebSocketServer::sslErrors, this, &WebIdentifier::onSslErrors, Qt::UniqueConnection);
    return true;
}
