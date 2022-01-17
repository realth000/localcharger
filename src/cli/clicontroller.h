#ifndef CLICONTROLLER_H
#define CLICONTROLLER_H
#include <QtCore/QObject>
#include "defines.h"

class CliController : public QObject
{
    Q_OBJECT
public:
    CliController(QObject *parent = nullptr);

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
};

#endif // CLICONTROLLER_H
