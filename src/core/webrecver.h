#ifndef WEBRECVER_H
#define WEBRECVER_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include "defines.h"

using MsgType = WebSocketBinaryMessageType;

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
    void stop();
    void setFileSavePath(QString path);

signals:
    void recvedMessage(const QString msg);
    void recverConnected();
    void recverDisconnected();
    void recvFileStart(QString filePath, qint64 recvBytes);
    void recvFileFinish(QString filePath, qint64 recvBytes);
    void recvFileFrameFinish(QString filePath, qint64 frameID, qint64 fileTotalFrameCount);
    void resetProgress(int fileCount);

public slots:
    void sendMessage(const QString &msg);
    void onPrepareRecvFile();
    void closeAllSocket();

private slots:
    void openUrl(const url_t &url);
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocket m_socket;
    QMap<QString , WebSocketFileInfo > m_fileInfoMap;
    QString m_fileSavePath;
    qint64 m_fileSavedSize;
    WebSocketBinaryMessageType parseBinaryMessageType(const QByteArray &message);
    void saveSingleFileFrame(const QByteArray &message);
    void makeDir(const QByteArray &dirListsArrary);
    void parseStartMessage(const QByteArray &message);
};

#endif // WEBRECVER_H
