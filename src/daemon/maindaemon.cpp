#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include "daemon.h"
#include "defines.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    LocalChargerDaemon d;
    QDBusConnection daemonConnection = QDBusConnection::sessionBus();
    if(!daemonConnection.registerService(DAEMON_SERVICE_NAME)){
        qInfo() << "Can NOT connect to session DBus:" << daemonConnection.lastError().message();
        exit(-1);
    }
    daemonConnection.registerObject(DAEMON_SERVICE_PATH, DAEMON_SERVICE_NAME, &d, QDBusConnection::ExportAllSlots);

    return app.exec();
}
