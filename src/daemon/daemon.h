#ifndef LOCALCHARGERDAEMON_H
#define LOCALCHARGERDAEMON_H
#include <QtCore/QObject>
#include "defines.h"
#include "core/webidentifier.h"

class LocalChargerDaemon : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "th000.localcharger.daemon")

public:
    LocalChargerDaemon(QObject *parent = nullptr);

public slots:
    QString getSenderStateStr();
    QString getRecverStateStr();

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
    QString m_localClientReadableName;
    int m_localClientId;
    port_t m_localWorkingPort;
    WebIdentifier *m_identifier;
    QString m_localIp;

    void getLocalIp();
};

#endif // LOCALCHARGERDAEMON_H
