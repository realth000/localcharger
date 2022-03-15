#ifndef WEBIDENTIFIER_H
#define WEBIDENTIFIER_H

#include <QtCore/QObject>
#include <QtNetwork/QUdpSocket>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include "defines.h"

#define IDENTIFIER_UDP_PORT 12335
#define IDENTIFIER_UDP_LISTION_PORT IDENTIFIER_UDP_PORT+1

class WebIdentifier : public QObject
{
    Q_OBJECT
public:
    enum class AutoConnectMessageType{
        RequestToConnect = 0,
        AcknowledgeToConnect = 1,
    };
    Q_ENUM(AutoConnectMessageType)
    explicit WebIdentifier(const QString &identityReadableName = "default", const int &identityId = 0000, const port_t &socketWorkingPort = WEBSOCKET_PORT_DEFAULT, QObject *parent = nullptr);
    ~WebIdentifier();
    void setIdentityReadableName(const QString &readableName);
    void setIdentityId(const int &id);
    void setIdentityIp(QString ip);
    void setWorkingPort(const port_t &port);
    void sendAutoConnectReply();

signals:
    void identityMessageParsed(QString ip, QString port, QString readableName, QString id);
    void getClientToConnect(QString ip, QString port);
    void getAutoConnectReply();
    void autoConnectStarted();

public slots:
    void boardcastIdentityMessage();
    void openUrl(const url_t &url);
    void startAutoConnect(const url_t &url);

private slots:
    void onIdenReadReady();
    void onNewConnection();
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);
    void onInSocketConnected();
    void onInSocketBinaryMessageReceived(const QByteArray &message);
    void onInSocketSslErrors(const QList<QSslError> &errors);
    void onOutSocketConnected();
    void onOutSocketDisconnected();
    void onOutSocketBinaryMessageReceived(const QByteArray &message);
    void onOutSocketSslErrors(const QList<QSslError> &errors);

private:
    QUdpSocket m_identifierSocket;
    QString m_identityReadableName;
    int m_identityId;
    QString m_identityIp;
    port_t m_socketWorkingPort;
    QWebSocketServer *m_socketServer;
    QWebSocket *m_inSocket;
    QWebSocket m_outSocket;

    QByteArray generateIdentidyData(const QString &identityReadableName, const port_t &socketWorkingPort);
    bool startListenPort(port_t port);
};

#endif // WEBIDENTIFIER_H
