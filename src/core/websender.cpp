#include "websender.h"
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include "core/jsonparser.h"
#include "utils/randomgenerator.h"
#include <QCoreApplication>

WebSender::WebSender(QObject *parent): QObject(parent),
      m_socketServer(nullptr),
      m_currentSocket(nullptr)
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
    if(m_currentSocket != nullptr){
        delete m_currentSocket;
    }
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

void WebSender::stop()
{
    if(m_currentSocket != nullptr){
        m_currentSocket->close();
        delete m_currentSocket;
        m_currentSocket = nullptr;
    }
    m_socketServer->close();
}

void WebSender::sendMessage(const QString &msg)
{
    // TODO: NOW only communicate in the lateset socket connection.
    m_currentSocket->sendTextMessage(msg);
}

void WebSender::sendFile(const QString &filePath)
{
    QFile fileToSend(filePath);
    QFileInfo fileInfo(filePath);
    if(!fileToSend.exists()){
        qDebug() << "file not exists:" << filePath;
        return;
    }
    if(!fileInfo.isFile()){
        qDebug() << filePath << "is not a file";
        return;
    }
    if(!fileToSend.open(QIODevice::ReadOnly)){
        qDebug() << "can not open file" << filePath;
        return;
    }
    /*
     * messageArray:
     * MessageType          10bytes
     * FileInfoArrayLength  8bytes (qint64 = qlonglong)
     * FileInfoArray        FileInfoArrayLength bytes
     * FileData             FileInfoArray.m_fileSize
     *
     */
    QByteArray messageArray;
    const QByteArray fileInfoArray = generateFileInfoMessage(filePath);
    QByteArray fileDataArray = fileToSend.readAll();
    fileToSend.close();
    // length = 10 bytes
    messageArray.append(QString::number(WebSocketBinaryMessageType::SingleFile).toUtf8());
    messageArray.append(QString::number(qint64(fileInfoArray.length())).toStdString().c_str(), WEBSOCKET_FILEINFO_ARRAYLENGTH_LENGTH);
    messageArray.append(fileInfoArray);
    messageArray.append(fileDataArray);
    qDebug() << "WebSender: about to send file, array total length" << messageArray.length();
    m_currentSocket->sendBinaryMessage(messageArray);
}

void WebSender::onNewConnection()
{
    emit senderConnected();
    qDebug() << "in new connection";
    QWebSocket *pSocket = m_socketServer->nextPendingConnection();
    qDebug() << "Client connected:" << pSocket->peerName() << pSocket->origin();
    connect(pSocket, &QWebSocket::disconnected, this, &WebSender::socketDisconnected, Qt::QueuedConnection);
    if(m_currentSocket != nullptr){
        delete m_currentSocket;
    }
    m_currentSocket = pSocket;
}

void WebSender::socketDisconnected()
{
    emit senderDisconnected();
    qDebug() << "Client disconnected";
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient != nullptr){
        m_currentSocket = nullptr;
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

QByteArray WebSender::generateFileInfoMessage(const QString &filePath)
{
    QFile file(filePath);
    if(!file.exists() || !file.open(QIODevice::ReadOnly)){
        qDebug() << "can not read file" << filePath;
        return QByteArray();
    }
    QFileInfo fileInfo(filePath);
    return JsonParser::genFileInfoFromString(WebSocketFileInfo(fileInfo.fileName(),fileInfo.size(),
                                                               "", RandomGenerator::generateFromString(WEBSOCKET_FILEINFO_FILEID_LENGTH)));
}
