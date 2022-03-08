#ifndef WEBSENDER_H
#define WEBSENDER_H

#include <QtWebSockets/QWebSocket>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QObject>
#include <QtNetwork/QSslError>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include "defines.h"

using MsgType = WebSocketBinaryMessageType;

class WebSender : public QObject
{
    Q_OBJECT
public:
    explicit WebSender(QObject *parent = nullptr);
    explicit WebSender(const port_t &port, QObject *parent = nullptr);
    ~WebSender() override;
    bool isSenderListening() const noexcept;
    url_t senderUrl() const noexcept;
    port_t senderPort() const noexcept;
    bool start(const port_t &port);
    void stop();
    void setRootPath(const QString &rootPath);

signals:
    void senderConnected();
    void senderDisconnected();
    void sendFileStart(QString fileName, qint64 sendBytes);
    void sendFileFinish(QString fileName, qint64 sendBytes);
    void sendFileFrameFinish(QString fileName, qint64 frameID, qint64 fileTotalFrameCount);
    void prepareRecvFile();

public slots:
    void sendMessage(const QString &msg);
    bool sendFile(const QString &filePath, const MsgType &msgType = MsgType::SingleFile);
    void sendDir(const QString &dirPath);
    void makeDir(const dir_lists &dirs);
    void closeAllSocket();

private slots:
    void onNewConnection();
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocketServer *m_socketServer;
    QWebSocket * m_currentSocket;
    bool startListenPort(const port_t &port);
    QByteArray generateFileInfoMessage(const QString &filePath, const MsgType &msgType = MsgType::SingleFile);
    QString m_rootPath; // Use when sending directory, replace() and get the relative file path.
    QEventLoop m_sendIntervalLoop;
};

#endif // WEBSENDER_H
