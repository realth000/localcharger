#ifndef QMLHANDLER_H
#define QMLHANDLER_H

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtGui/QClipboard>
#include <QtGui/QRegularExpressionValidator>
#include "core/webrecver.h"
#include "core/websender.h"
#include "defines.h"
#include "src/core/webidentifier.h"

class QmlHandler : public QObject
{
    Q_OBJECT

public:
    explicit QmlHandler(QObject *parent = nullptr);
    enum QmlSenderState{
        SenderDisconnected = static_cast<int>(SenderState::Disconnected),
        SenderListening = static_cast<int>(SenderState::Listening),
        SenderConnected = static_cast<int>(SenderState::Connected)
    };
    Q_ENUM(QmlSenderState)
    enum class QmlRecverState{
        RecverDisconnected = static_cast<int>(RecverState::Disconnected),
        RecverConnecting = static_cast<int>(RecverState::Connecting),
        RecverConnected = static_cast<int>(RecverState::Connected)
    };
    Q_ENUM(QmlRecverState)

signals:
    void qmlMessageInfo(QString msg);
    void qmlUpdateSenderState(QmlHandler::QmlSenderState state);
    void qmlUpdateRecverState(QmlHandler::QmlRecverState state);
    void qmlUpdateLocalUrlLists(QStringList ipStringList);
    void qmlUpdateSocketConfig(QString senderIp, int senderPort, int recverPort);
    void qmlUpdateSenderIp(QString senderIp);
    void qmlUpdateSenderPort(int senderPort);
    void qmlUpdateRecverPort(int recverPort);
    void qmlUpdateClientName(QString name);
    void qmlUpdateClientId(int id);
    void qmlAppendSendedMessage(QString msg);
    void qmlAppendRecvedMessage(QString msg);
    void qmlClearToSendMsg();
    void qmlUpdateFileSavePath(QString path);
    void qmlAddClient(QString ip, QString port, QString readableName, QString id);
    void qmlUpdateClientAutoConnect(bool isEnabled);

public slots:
    void initHandler();
    void startSender();
    void startRecver();
    void onSenderConnected();
    void onSenderDisconnected();
    void onRecverConnected();
    void onRecverDisconnected();
    void sendMessage(const QString &msg);
    void sendFile(const QString &filePath);
    void setSenderUrl(const QString & url);
    void setSenderPort(const QString &port);
    void setRecverPort(const QString &port);
    void setClientReadableName(const QString &name);
    void setFileSavePath(const QString &filePath);
    void saveConfig();
    void updateWebConfig();
    void boardcastIdentityMessage();
    void connectSelectedClient(const QString &name, const QString &id, const QString &ip, const QString &port);
    void setClipBoardText(const QString text);
    void setAutoConnect(const bool &isEnabled);
    void sendDir(const QString &dirPath);

private:
    WebSender m_socketSender;
    WebRecver m_socketRecver;
    WebIdentifier *m_identifier;
    QString m_socketSenderIp;
    port_t m_socketSenderPort;
    url_t m_socketRecverUrl;
    port_t m_socketRecverPort;
    QmlSenderState m_socketSenderState;
    QmlRecverState m_socketRecverState;
    QRegularExpressionValidator *m_ipTypeValidator;
    QIntValidator *m_portTypeValidator;
    const QString m_configFilePath;
    QString m_saveFileDirPath;
    bool m_enableAutoConnect;
    QClipboard *m_clipBoard;

    // for WebIdentifier
    QMap<QString, QString> m_clientsMap;
    QString m_localClientReadableName;
    int m_localClientId;
    port_t m_localWorkingPort;
    QString m_localIp;

    void initConnections();
    void startSender(const port_t &port);
    void stopSender();
    void startRecver(const url_t &url);
    void stopRecver();
    void updateSenderState(QmlSenderState state);
    void updateRecverState(QmlRecverState state);
    void loadDefaultConfig();
    void loadConfig();
    void getLocalIp();
    void addDetectedClients(const QString &ip, const QString &port, const QString &readableName, const QString &id);
#ifdef Q_OS_ANDROID
    void requestAndroidPermissions();
    void callAndroidToast(const QString &message);
#endif

private slots:
    void onRecoredRecvedMsg(const QString &msg);
    void onSendFileStart(const QString &fielPath, const qint64 &fileSize);
    void onSendFileFinish(const QString &fielPath, const qint64 &sendBytes);
    void onRecvFileStart(const QString &fielPath, const qint64 &fileSize);
    void onRecvFileFinish(const QString &fielPath, const qint64 &recvBytes);
    void onIdentityMessageParsed(const QString &ip, const QString &port, const QString &readableName, const QString &id);
    void autoConnectToClinet(const QString &ip, const QString &port);
    void onGetAutoConnectReply();
};

#endif // QMLHANDLER_H
