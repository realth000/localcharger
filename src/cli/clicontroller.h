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
    bool getDaemonConnectionStatus() const;
    void connectRemote(const QString &remotePath);

private:
    // FIXME: Can m_daemonInterface use as non-static?
    static QDBusInterface m_daemonInterface;
    bool m_daemonConnectionStatus;
};

#endif // CLICONTROLLER_H
