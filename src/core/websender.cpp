#include "websender.h"
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

WebSender::WebSender(QObject *parent): QObject(parent),
      m_socketServer(nullptr)
{
    m_socketServer = new QWebSocketServer(QStringLiteral("sada"), QWebSocketServer::SecureMode, this);
    QSslConfiguration sslConfigure;
    QFile certFile(QStringLiteral(":/config/localhost.cert"));
    QFile keyFile(QStringLiteral(":/config/localhost.key"));
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
}

WebSender::WebSender(const port_t &port, QObject *parent): WebSender(parent)
{
    if(startListenPort(port)){
        qDebug() << "WebSender: started listening" << port;
    }
    else{
        qDebug() << "WebSender: start failed";
    }
}

WebSender::~WebSender()
{
    m_socketServer->close();
    qDeleteAll(m_clientsList);
}

bool WebSender::isSenderListening() const noexcept
{
    return m_socketServer->isListening();
}

QUrl WebSender::senderUrl() const noexcept
{
    return m_socketServer->serverUrl();
}

port_t WebSender::senderPort() const noexcept
{
    return m_socketServer->serverPort();
}

bool WebSender::start(const port_t &port)
{
    return startListenPort(port);
}

void WebSender::onNewConnection()
{
    emit senderConnected();
    qDebug() << "in new connection";
    QWebSocket *pSocket = m_socketServer->nextPendingConnection();
    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();
    connect(pSocket, &QWebSocket::textMessageReceived, this, &WebSender::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebSender::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebSender::socketDisconnected);
    m_clientsList << pSocket;
}

void WebSender::processTextMessage(const QString &message) const
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient != nullptr){
        pClient->sendTextMessage(message);
    }

}

void WebSender::processBinaryMessage(const QByteArray &message) const
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient != nullptr){
        pClient->sendBinaryMessage(message);
    }
}

void WebSender::socketDisconnected()
{
    emit senderDisconnected();
    qDebug() << "Client disconnected";
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient != nullptr){
        m_clientsList.removeAll(pClient);
        pClient->deleteLater();
    }
}

void WebSender::onSslErrors(const QList<QSslError> &errors)
{
    qDebug() << "Ssl errors occurred";
    foreach(QSslError error, errors){
        qDebug() << error.errorString();
    }
}

bool WebSender::startListenPort(const port_t &port)
{
    if(m_socketServer->listen(QHostAddress::Any, port)){
        connect(m_socketServer, &QWebSocketServer::newConnection, this, &WebSender::onNewConnection, Qt::UniqueConnection);
        connect(m_socketServer, &QWebSocketServer::sslErrors, this, &WebSender::onSslErrors, Qt::UniqueConnection);
        return true;
    }
    return false;
}
