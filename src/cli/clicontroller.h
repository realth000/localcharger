#ifndef CLICONTROLLER_H
#define CLICONTROLLER_H
#include <QtCore/QObject>
#include <QtDBus/QDBusInterface>
#include "defines.h"
#include "core/webidentifier.h"

class CliController : public QObject
{
    Q_OBJECT
public:
    CliController(QObject *parent = nullptr);
    QString getStatus() const;
    QString getSenderStatus() const;
    QString getRecverStatus() const;
    int getSenderStatusCode() const;
    int getRecverStatusCode() const;
    void exitDaemon() const;
    bool getDaemonConnectionStatus() const;
    void connectRemote(const QString &remotePath);
    void sendMessage(const QString &msg);
    void sendFile(const QString &filePath);

private:
    // FIXME: Can m_daemonInterface use as non-static?
    static QDBusInterface m_daemonInterface;
    bool m_daemonConnectionStatus;
};

#endif // CLICONTROLLER_H
