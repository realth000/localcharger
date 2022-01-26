#ifndef LOCALCHARGERDAEMON_H
#define LOCALCHARGERDAEMON_H
#include <QtCore/QObject>
#include "defines.h"
#include "core/webidentifier.h"
#include "core/webrecver.h"
#include "core/websender.h"

// test HTTPSERVER
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"

class Test : public QObject
{
    Q_OBJECT
public:
    Test(){
        QHttpServer *server = new QHttpServer(this);
        connect(server, &QHttpServer::newRequest, this, &Test::handleRequest);
        server->listen(QHostAddress::Any, 8080);
    }
private slots:
    void handleRequest(QHttpRequest *req, QHttpResponse *resp){
        Q_UNUSED(req)
#if 0
        QByteArray body = "Hello world";
        resp->setHeader("Content-Length", QString::number(body.length()));
        resp->writeHead(200);
        resp->end(body);
#else
        qInfo() << "resp->path =" << req->path();
        QRegExp exp("^/user/([a-z]+)$");
        if( exp.indexIn(req->path()) != -1 )
        {
            resp->setHeader("Content-Type", "text/html");
            resp->writeHead(200);

            QString name = exp.capturedTexts()[1];
            QString body = tr("<html><head><title>Greeting App</title></head><body><h1>Hello %1!</h1></body></html>");
            resp->end(body.arg(name).toUtf8());
        }
        else
        {
            resp->writeHead(403);
            resp->end(QByteArray("You aren't allowed here!"));
        }
#endif
    }
};

class LocalChargerDaemon : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", DAEMON_SERVICE_NAME)

public:
    LocalChargerDaemon(QObject *parent = nullptr);

public slots:
    QString getStatus();
    QString getSenderStatus();
    QString getRecverStatus();
    int getSenderStatusCode();
    int getRecverStatusCode();
    void exitDaemon();
    void connectRemote(const QString &remotePath);
    void sendMessage(const QString &msg);

private:
    QString m_sockerSenderIp;
    port_t m_socketSenderPort;
    url_t m_socketRecverUrl;
    port_t m_socketRecverPort;
    SenderState m_socketSenderState;
    RecverState m_socketRecverState;
    const QString m_configFilePath;
    QString m_saveFileDirPath;
    bool m_enableAutoConnect;

    // for WebIdentifier
    QMap<QString, QString> m_clientsMap;
    QString m_localClientReadableName;
    int m_localClientId;
    port_t m_localWorkingPort;
    WebIdentifier *m_identifier;
    QString m_localIp;
    WebSender m_socketSender;
    WebRecver m_socketRecver;

    void initConnections();
    void getLocalIp();
    void startSender(const port_t &port);
    void stopSender();
    void updateSenderState(SenderState state);
    void startRecver(const url_t &url);
    void stopRecver();
    void updateRecverState(RecverState state);

private slots:
    void autoConnectToClinet(const QString &ip, const QString &port);
    void onIdentityMessageParsed(const QString &ip, const QString &port, const QString &readableName, const QString &id);
    void onGetAutoConnectReply();
    void onSenderConnected();
    void onSenderDisconnected();
    void onRecverConnected();
    void onRecverDisconnected();
    void onSendFileStart(const QString &filePath, const qint64 &fileSize);
    void onSendFileFinish(const QString &filePath, const qint64 &fileSize);
    void onRecvFileStart(const QString &filePath, const qint64 &fileSize);
    void onRecvFileFinish(const QString &filePath, const qint64 &fileSize);
    void recoredRecvedMsg(const QString &msg);
};

#endif // LOCALCHARGERDAEMON_H
