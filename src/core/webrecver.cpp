#include "webrecver.h"
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include "core/jsonparser.h"

WebRecver::WebRecver(QObject *parent, QString fileSavePath) : QObject(parent), m_fileSavePath(fileSavePath)
{
    connect(&m_socket, &QWebSocket::connected, this, &WebRecver::onConnected, Qt::UniqueConnection);
    connect(&m_socket, &QWebSocket::disconnected, this, &WebRecver::onDisconnected, Qt::QueuedConnection);
    connect(&m_socket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors), this, &WebRecver::onSslErrors, Qt::UniqueConnection);
}

WebRecver::WebRecver(const url_t &url, QObject *parent, QString fileSavePath) : WebRecver(parent, fileSavePath)
{
    openUrl(url);
}

WebRecver::~WebRecver()
{
    m_socket.close();
}

QAbstractSocket::SocketState WebRecver::getRecverState() const noexcept
{
    return m_socket.state();
}

url_t WebRecver::recverUrl() const noexcept
{
    return m_socket.requestUrl();
}

port_t WebRecver::recverPort() const noexcept
{
    return m_socket.localPort();
}

bool WebRecver::start(const url_t &url)
{
    if(!url.isValid()){
        qDebug() << "Recver start failed: invalid url" << url;
        return false;
    }
    openUrl(url);
    return true;
}

void WebRecver::stop()
{
    if(m_socket.state() != QAbstractSocket::UnconnectedState && m_socket.state() != QAbstractSocket::ClosingState){
        m_socket.close();
    }
}

void WebRecver::setFileSavePath(QString path)
{
    m_fileSavePath = path;
}

void WebRecver::sendMessage(const QString &msg)
{
    qDebug() << "send";
    m_socket.sendTextMessage(msg);
}

void WebRecver::openUrl(const url_t &url)
{
    m_socket.open(url);
}

void WebRecver::onConnected()
{
    emit recverConnected();
    qDebug() << "WebSocket connected";
    // Without Qt::QueuedConnection, connection will become apart when transporting files divided into two frames or more
    // Without Qt::UniqueConnection, connection will be multiple when restart connecting without changing socket config
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &WebRecver::onTextMessageReceived, static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
    connect(&m_socket, &QWebSocket::binaryMessageReceived, this, &WebRecver::onBinaryMessageReceived, static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection));
}

void WebRecver::onDisconnected()
{
    emit recverDisconnected();
}

void WebRecver::onTextMessageReceived(const QString &message)
{
    emit recvedMessage(message);
}

void WebRecver::onBinaryMessageReceived(const QByteArray &message)
{
    qDebug() << "binary message received";
    const int messageType = QString::fromUtf8(message.left(10)).toInt();

    switch (messageType) {
    case WebSocketBinaryMessageType::SingleFile:
        saveSingleFileFrame(message);
        break;
    default:
        break;
    }
}

void WebRecver::onSslErrors(const QList<QSslError> &errors)
{
    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    foreach(QSslError error, errors){
        qDebug() << error.errorString();
    }
    m_socket.ignoreSslErrors();
}

WebSocketBinaryMessageType WebRecver::parseBinaryMessageType(const QByteArray &message)
{
    const int messageType = QString::fromUtf8(message.left(WEBSOCKET_MESSAGETYPE_LENGTH)).toInt();
    switch (messageType) {
    case WebSocketBinaryMessageType::SingleFile:
        return WebSocketBinaryMessageType::SingleFile;
    default:
        return WebSocketBinaryMessageType();
    }

}

/*
 * messageArray:
 * MessageType          10bytes
 * FileInfoArrayLength  8bytes (qint64 = qlonglong)
 * FileInfoArray        FileInfoArrayLength bytes
 * FileData             FileInfoArray.m_fileSize
 *
 */
void WebRecver::saveSingleFileFrame(const QByteArray &message)
{
    const int fileInfoArrayLength = message.mid(WEBSOCKET_MESSAGETYPE_LENGTH, WEBSOCKET_FILEINFO_ARRAYLENGTH_LENGTH).toInt();
    const QByteArray fileInfoArray = message.mid(WEBSOCKET_MESSAGETYPE_LENGTH + WEBSOCKET_FILEINFO_ARRAYLENGTH_LENGTH, fileInfoArrayLength);
    const WebSocketFileInfo fileInfo = JsonParser::parseFileInfoFromArray(fileInfoArray);
    const int fileFrameID = QString::fromUtf8(message.mid(WEBSOCKET_MESSAGETYPE_LENGTH + WEBSOCKET_FILEINFO_ARRAYLENGTH_LENGTH + fileInfoArrayLength, WEBSOCKET_FILEFRAME_ID_LENGTH)).toInt();
    qDebug() << fileInfo.m_fileID << fileInfo.m_fileName << fileInfo.m_fileSize << fileInfo.m_fileChkSum << fileInfo.m_fileFrameCount;
    qDebug() << "receive id =" << fileFrameID;
    QFile file(m_fileSavePath + NATIVE_PATH_SEP + fileInfo.m_fileName);
    if(fileFrameID == 0){
        emit recvFileStart(file.fileName(), file.size());
        if(file.exists()){
            file.remove();
        }
    }
    if(!file.open(QIODevice::Append)){
        qDebug() << "WebRecver can't save file not open" << QFileInfo(file).absoluteFilePath();
        return;
    }
    qint64 fileWriteSize = file.write(message.right(message.length() - WEBSOCKET_MESSAGETYPE_LENGTH - WEBSOCKET_FILEINFO_ARRAYLENGTH_LENGTH - fileInfoArrayLength - WEBSOCKET_FILEFRAME_ID_LENGTH));
    file.close();
    qDebug() << QString("WebSocket: write file %1(%2 bytes, fileFrameID=%3)").arg(QFileInfo(file).absoluteFilePath(), QString::number(fileWriteSize), QString::number(fileFrameID));
    if(fileFrameID == fileInfo.m_fileFrameCount -1){
        emit recvFileFinish(file.fileName(), file.size());
    }
}
