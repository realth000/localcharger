#ifndef WEBIDENTIFIER_H
#define WEBIDENTIFIER_H

#include <QtCore/QObject>
#include <QtNetwork/QUdpSocket>
#include "defines.h"

class WebIdentifier : public QObject
{
    Q_OBJECT
public:
    explicit WebIdentifier(const QString &identityReadableName = "default", const port_t &socketWorkingPort = WEBSOCKET_PORT_DEFAULT, QObject *parent = nullptr);
    void setIdentityReadableName(const QString &readableName);
    void setIdentityId(const int &id);
    void setIdentityIp(QString ip);
    void setWorkingPort(const port_t &port);

signals:
    void identityMessageParsed(QString ip, QString port, QString readableName, QString id);

public slots:
    void boardcastIdentityMessage();

private slots:
    void onIdenReadReady();

private:
    QUdpSocket m_identifierSocket;
    QString m_identityReadableName;
    int m_identityId;
    QString m_identityIp;
    port_t m_socketWorkingPort;

    QByteArray generateIdentidyData(const QString &identityReadableName, const port_t &socketWorkingPort);
};

#endif // WEBIDENTIFIER_H
