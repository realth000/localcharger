#ifndef CLICONTROLLER_H
#define CLICONTROLLER_H
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtDBus/QDBusInterface>
#include "defines.h"
#include "core/webidentifier.h"

class CliController : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", CLI_SERVICE_NAME)

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
    void sendDir(const QString &dirPath);

public slots:
    void updateSendProgress(const QString &fileName, const int &fileProgress);
    void exitCli(const int &exitCode);

private:
    // FIXME: Can m_daemonInterface use as non-static?
    static QDBusInterface m_daemonInterface;
    bool m_daemonConnectionStatus;
    QString m_taskName;
    int m_process;
#ifndef DISABLE_UPDATE_PROGRESS_BY_TIMER
    QTimer m_processTimer;
#endif

    void printProcess(const QString &taskName, const int &process);
};

#endif // CLICONTROLLER_H
