#include "websender.h"
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

WebSender::WebSender(QObject *parent): QObject(parent),
      m_socketServer(nullptr)
{
    m_socketServer = new QWebSocketServer(QStringLiteral(WEBSOCKET_CONFIG_SENDER_SOCKET_NAME), QWebSocketServer::SecureMode, this);
    QSslConfiguration sslConfigure;
    QFile certFile(QStringLiteral(WEBSOCKET_CONFIG_CERTFILT_PATH));
    QFile keyFile(QStringLiteral(WEBSOCKET_CONFIG_KEYFILE_PATH));
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
    startListenPort(port) ? qDebug() << "WebSender: started listening" << port
                          : qDebug() << "WebSender: start failed";
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

url_t WebSender::senderUrl() const noexcept
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

void WebSender::sendMessage(const QString &msg)
{
    // TODO: NOW only communicate in the lateset socket connection.
    m_clientsList.constLast()->sendTextMessage(msg);
}

void WebSender::onNewConnection()
{
    emit senderConnected();
    qDebug() << "in new connection";
    QWebSocket *pSocket = m_socketServer->nextPendingConnection();
    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();
    connect(pSocket, &QWebSocket::disconnected, this, &WebSender::socketDisconnected);
    m_clientsList << pSocket;
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
    if(!m_socketServer->listen(QHostAddress::Any, port)){
        return false;
    }
    connect(m_socketServer, &QWebSocketServer::newConnection, this, &WebSender::onNewConnection, Qt::UniqueConnection);
    connect(m_socketServer, &QWebSocketServer::sslErrors, this, &WebSender::onSslErrors, Qt::UniqueConnection);
    return true;
}
