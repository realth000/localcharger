#include "websender.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QtMath>
#include <QtCore/QThread>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include "core/jsonparser.h"
#include "core/threadworker.h"
#include "utils/randomgenerator.h"


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
    startListenPort(port) ? qInfo() << "WebSender: started listening" << port
                          : qInfo() << "WebSender: start failed";
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
        // Usually actions below will handle in SLOT socketDisconnected.
        // FIXME: Be careful, non-connect socket may exists.
        // delete m_currentSocket;
        // m_currentSocket = nullptr;
    }
    m_socketServer->close();
}

void WebSender::setRootPath(const QString &rootPath)
{
    m_rootPath = rootPath;
}

void WebSender::sendMessage(const QString &msg)
{
    // TODO: NOW only communicate in the lateset socket connection.
    m_currentSocket->sendTextMessage(msg);
}

bool WebSender::sendFile(const QString &filePath, const MsgType &msgType)
{
    QFile fileToSend(filePath);
    QFileInfo fileInfo(filePath);
    if(!fileToSend.exists()){
        qInfo() << "file not exists:" << filePath;
        return false;
    }
    if(!fileInfo.isFile()){
        qInfo() << filePath << "is not a file";
        return false;
    }
    if(!fileToSend.open(QIODevice::ReadOnly)){
        qInfo() << "can not open file" << filePath;
        return false;
    }
    emit prepareRecvFile();
    /*
     * messageArray:
     * MessageType          10bytes
     * FileInfoArrayLength  8bytes (qint64 = qlonglong)
     * FileInfoArray        FileInfoArrayLength bytes
     * FileData             FileInfoArray.m_fileSize
     *
     */
    QByteArray messageArray;
    const QByteArray fileInfoArray = generateFileInfoMessage(filePath, msgType);
    QByteArray fileDataArray = fileToSend.read(WEBSOCKET_FILEFRAME_FRAME_LENGTH);
    const qint64 fileTotalFrameCount = qCeil(fileInfo.size()/qreal(WEBSOCKET_FILEFRAME_FRAME_LENGTH));
    qint64 fileFrameID = 0;
    qint64 fileSendBytes = 0;
    emit sendFileStart(fileInfo.fileName(), fileToSend.size());
    while(!fileDataArray.isEmpty()){
        // TODO: processEvents can control speed but controls too much
        // length = 10 bytes
        QCoreApplication::processEvents();
        messageArray.append(QString::number(msgType).toUtf8());
        messageArray.append(QString::number(qint64(fileInfoArray.length())).toUtf8(), WEBSOCKET_FILEINFO_ARRAYLENGTH_LENGTH);
        messageArray.append(fileInfoArray);
        messageArray.append(QString::number(fileFrameID).toUtf8(), WEBSOCKET_FILEFRAME_ID_LENGTH);
        messageArray.append(fileDataArray);
        m_currentSocket->sendBinaryMessage(messageArray);
        emit sendFileFrameFinish(fileInfo.fileName(), fileFrameID + 1, fileTotalFrameCount);
        /* frame size, time
         * 10485760,   1000
         * 1048576,    100
         */
        QTimer::singleShot(100, &m_sendIntervalLoop, &QEventLoop::quit);
        m_sendIntervalLoop.exec();
        messageArray.clear();
        fileSendBytes += fileDataArray.length();
        fileDataArray = fileToSend.read(WEBSOCKET_FILEFRAME_FRAME_LENGTH);
        fileFrameID ++;
    }
    fileToSend.close();
    qInfo() << "WebSender: about to send file, array total length" << fileSendBytes;
    emit sendFileFinish(fileInfo.fileName(), fileSendBytes);
    return true;
}

void WebSender::sendDir(const QString &dirPath)
{
    QDir sourceDir(dirPath);
    if(!sourceDir.exists()){
        qInfo() << "source directory not exists:" << dirPath;
        return;
    }
    const QFileInfoList sourceDirEntryList = sourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden, QDir::Name);
    for(const QFileInfo &sourceInfo : sourceDirEntryList){
        /*
         * TODO: now only handle on symbol link, file and directory,
         * other types should also be in concern.
         */
        if(sourceInfo.isSymLink()){
            continue;
        }
        else if(sourceInfo.isFile()){
            sendFile(sourceInfo.absoluteFilePath(), MsgType::SingleFileWithPath);
        }
        else if(sourceInfo.isDir()){
            sendDir(sourceInfo.absoluteFilePath());
        }
    }
}

void WebSender::makeDir(const dir_lists &dirs)
{
    QByteArray messageArray;
    messageArray.append(QString::number(MsgType::MakeDir).toUtf8());
    messageArray.append(JsonParser::genDirListsFromVector(dirs));
    m_currentSocket->sendBinaryMessage(messageArray);
}

void WebSender::closeAllSocket()
{
    if (m_socketServer->isListening()) {
        m_socketServer->close();
    }
    if (m_currentSocket != nullptr && m_currentSocket->state() != QAbstractSocket::UnconnectedState) {
        m_currentSocket->abort();
    }
}

void WebSender::notifyStart(const int &fileCount)
{
    QByteArray messageArray;
    messageArray.append(QString::number(MsgType::StartProgress).toUtf8());
    messageArray.append(QString::number(fileCount).toUtf8());
    m_currentSocket->sendBinaryMessage(messageArray);
}

void WebSender::onNewConnection()
{
    emit senderConnected();
    qInfo() << "in new connection";
    QWebSocket *pSocket = m_socketServer->nextPendingConnection();
    qInfo() << "Client connected:" << pSocket->peerName() << pSocket->origin();
    pSocket->setPauseMode(QAbstractSocket::PauseOnSslErrors);
    connect(pSocket, &QWebSocket::disconnected, this, &WebSender::socketDisconnected, Qt::QueuedConnection);
    if(m_currentSocket != nullptr){
        delete m_currentSocket;
    }
    m_currentSocket = pSocket;
    m_currentSocket->setOutgoingFrameSize(1048576);
}

void WebSender::socketDisconnected()
{
    emit senderDisconnected();
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient != nullptr){
        qInfo() << "WebSender: Client disconnected" << pClient->closeCode() << pClient->closeReason();
        m_currentSocket = nullptr;
        pClient->deleteLater();
    }
}

void WebSender::onSslErrors(const QList<QSslError> &errors)
{
    qInfo() << "Ssl errors occurred";
    foreach(QSslError error, errors){
        qInfo() << error.errorString();
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

QByteArray WebSender::generateFileInfoMessage(const QString &filePath, const MsgType &msgType)
{
    QFile file(filePath);
    if(!file.exists() || !file.open(QIODevice::ReadOnly)){
        qInfo() << "can not read file" << filePath;
        return QByteArray();
    }
    QFileInfo fileInfo(filePath);
    QString fileName;
    switch (msgType) {
    case MsgType::SingleFile:
        fileName = fileInfo.fileName();
        break;
    case MsgType::SingleFileWithPath:
        fileName = fileInfo.absoluteFilePath().replace(m_rootPath + "/", "");
        break;
    default:
        break;
    }
    return JsonParser::genFileInfoFromString(WebSocketFileInfo(
                                                 fileName,
                                                 fileInfo.size(),
                                                 "",
                                                 RandomGenerator::generateFromString(WEBSOCKET_FILEINFO_FILEID_LENGTH),
                                                 qCeil(qreal(fileInfo.size())/WEBSOCKET_FILEFRAME_FRAME_LENGTH)));
}
