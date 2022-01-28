#ifndef LOCALCHARGERDAEMON_H
#define LOCALCHARGERDAEMON_H
#include <QtCore/QObject>
#include "defines.h"
#include "core/webidentifier.h"
#include "core/webrecver.h"
#include "core/websender.h"

#if 1
// test HTTPSERVER
#include "qhttprequest.h"
#include "qhttpresponse.h"
#include "qhttpserver.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

class Test : public QObject
{
    Q_OBJECT
public:
    Test()
    {
        QHttpServer *server = new QHttpServer(this);
        connect(server, &QHttpServer::newRequest, this, &Test::handleRequest);
        server->listen(QHostAddress::Any, 8080);
    }

private:
    bool exists(const QString &filePath) const
    {
        return QFileInfo::exists(filePath);
    }

    bool isDir(const QString &filePath) const
    {
        return QFileInfo(filePath).isDir();
    }

    bool isFile(const QString &filePath) const
    {
        return QFileInfo(filePath).isFile();
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
        const QString accessPath = req->path();
        qInfo() << "resp->path =" << accessPath;
        if(!exists(accessPath)){
            resp->writeHead(403);
            resp->end(QByteArray("Accessing empty path"));
        }
        QString body = "";
        QByteArray bodyArray;
        // Is a file
        if(isFile(accessPath)){
            QFile file(accessPath);
            if(!file.open(QIODevice::ReadOnly)){
                resp->setHeader("Content-Type", "text/html");
                resp->writeHead(200);
                qInfo() << "can not read file" << accessPath;
                body = "<html><head>"
                       "<title>Greeting App</title>"
                       "</head>"
                       "<body>"
                       "<h1>Can not read file %1</h1>"
                       "<hr>"
                       "</body></html>";
                body.arg(accessPath);
                resp->end(body.toUtf8());
            }
            else{
                resp->setHeader("Content-Type", "application/octet-stream");
                resp->writeHead(200);
                bodyArray = file.readAll();
                file.close();
                resp->end(bodyArray);
            }
            return;
        }
        // Not a file
        QRegExp exp("^/user/([a-z]+)$");
        if( exp.indexIn(req->path()) != -1 || true )
        {
            resp->setHeader("Content-Type", "text/html");
            resp->writeHead(200);
            QString name = exp.capturedTexts()[1];
            QString body = tr("<html><head>"
                              "<title>Greeting App</title>"
                              "</head>"
                              "<body>"
                              "<h1>Hello %1!</h1>"
                              "<hr>"
                              "<ul>"
                              "<li><a href=\".git/\">.git/</a></li>"
                              "<li><a href=\"Makefile\">Makefile</a></li>"
                              "<li><a href=\"123\">123</a></li>"
                              "</ul>"
                              "<hr>"
                              "</body></html>");
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
#endif

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
