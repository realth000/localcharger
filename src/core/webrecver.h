#ifndef WEBRECVER_H
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
    explicit WebRecver(QObject *parent = nullptr, QString fileSavePath = "../");
    explicit WebRecver(const url_t &url, QObject *parent = nullptr, QString fileSavePath = "../");
    ~WebRecver() override;
    QAbstractSocket::SocketState getRecverState() const noexcept;
    url_t recverUrl() const noexcept;
    port_t recverPort() const noexcept;
    bool start(const url_t &url);
    void setFileSavePath(QString path);

signals:
    void recvedMessage(const QString msg);
    void recverConnected();
    void recverDisconnected();

public slots:
    void sendMessage(const QString &msg);

private slots:
    void openUrl(const url_t &url);
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(QString message);
    void onBinaryMessageReceived(QByteArray message);
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocket m_socket;
    QMap<QString , WebSocketFileInfo > m_fileInfoMap;
    QString m_fileSavePath;
    WebSocketBinaryMessageType parseBinaryMessageType(const QByteArray &message);
    void saveSingleFile(const QByteArray &message);
};

#endif // WEBRECVER_H
