#ifndef LOCALCHARGERDAEMON_H
#define LOCALCHARGERDAEMON_H
#include <QtCore/QObject>
#include "defines.h"
#include "core/webidentifier.h"
#include "core/webrecver.h"
#include "core/websender.h"

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
    void sendFile(const QString &filePath);
    void getSendFileProcess(QString &fileName, int &sendProcess);

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
    QString m_sendFileName;
    int m_sendFileProcess;

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
    void onSendFileFrameFinish(const QString fileName, const qint64 frameID, const qint64 fileTotalFrameCount);
    void onRecvFileStart(const QString &filePath, const qint64 &fileSize);
    void onRecvFileFinish(const QString &filePath, const qint64 &fileSize);
    void recoredRecvedMsg(const QString &msg);
};

#endif // LOCALCHARGERDAEMON_H
