#ifndef WEBSENDER_H
#define WEBSENDER_H

#include <QtWebSockets/QWebSocket>
#include <QtCore/QObject>
#include <QtNetwork/QSslError>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include "defines.h"

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

signals:
    void senderConnected();
    void senderDisconnected();
    void sendFileStart(QString fileName, qint64 sendBytes);
    void sendFileFinish(QString fileName, qint64 sendBytes);
    void prepareRecvFile();

public slots:
    void sendMessage(const QString &msg);
    bool sendFile(const QString &filePath);

private slots:
    void onNewConnection();
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocketServer *m_socketServer;
    QWebSocket * m_currentSocket;
    bool startListenPort(const port_t &port);
    QByteArray generateFileInfoMessage(const QString &filePath);
};

#endif // WEBSENDER_H
