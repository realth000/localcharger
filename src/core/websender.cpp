#include "websender.h"
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>

WebSender::WebSender(quint16 port, QObject *parent): QObject(parent),
      m_socketServer(nullptr)
{
    m_socketServer = new QWebSocketServer(QStringLiteral("sad"), QWebSocketServer::SecureMode, this);
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
    if(m_socketServer->listen(QHostAddress::Any, port)){
        qDebug() << "SSL Echo Server listening on port" << port;
        connect(m_socketServer, &QWebSocketServer::newConnection,
                this, &WebSender::onNewConnection);
        connect(m_socketServer, &QWebSocketServer::sslErrors,
                this, &WebSender::onSslErrors);
    }

}

WebSender::~WebSender()
{
    m_socketServer->close();
    qDeleteAll(m_clientsList);
}

void WebSender::onNewConnection()
{
    qDebug() << "in new connection";
    QWebSocket *pSocket = m_socketServer->nextPendingConnection();
    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();
    connect(pSocket, &QWebSocket::textMessageReceived, this, &WebSender::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &WebSender::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &WebSender::socketDisconnected);
    m_clientsList << pSocket;
}

void WebSender::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient != nullptr){
        pClient->sendTextMessage(message);
    }

}

void WebSender::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient != nullptr){
        pClient->sendBinaryMessage(message);
    }
}

void WebSender::socketDisconnected()
{
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
