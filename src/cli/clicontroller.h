﻿#ifndef CLICONTROLLER_H
#define CLICONTROLLER_H
#include <QtCore/QObject>
#include "defines.h"
#include "core/webidentifier.h"

class CliController : public QObject
{
    Q_OBJECT
public:
    CliController(QObject *parent = nullptr);
    void getCliStatus() const;
    QString getSenderStateStr() const;
    QString getRecverStateStr() const;

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

#endif // CLICONTROLLER_H
