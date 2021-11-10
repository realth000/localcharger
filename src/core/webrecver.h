﻿#ifndef WEBRECVER_H
#define WEBRECVER_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include "defines.h"

class WebRecver : public QObject
{
    Q_OBJECT
public:
    explicit WebRecver(QObject *parent = nullptr);
    explicit WebRecver(const QUrl &url, QObject *parent = nullptr);
    ~WebRecver() override;
    QAbstractSocket::SocketState getRecverState() const noexcept;
    QUrl recverUrl() const noexcept;
    port_t recverPort() const noexcept;
    bool start(const QUrl &url);

signals:
    void recvedMessage(const QString msg);
    void recverConnected();
    void recverDisconnected();

public slots:
    void sendMessage(const QString &msg);

private slots:
    void openUrl(const QUrl &url);
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(QString message);
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocket m_socket;
};

#endif // WEBRECVER_H
