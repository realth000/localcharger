#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QSharedMemory>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include "daemon.h"
#include "defines.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QSharedMemory sharedMemory("local_charger_daemon_shared_mem11");
    if(sharedMemory.attach()){
        qInfo() << "daemon already started";
        // FIXME: Normally, there should NOT be detch().
        // In fact, the attached sharedMemory will not free after exit, so
        // detach() is needed, which performs different in KeyContainer.
        // In this case, detach() will free sharedMemory so the latter
        // start will success if the former one exited.
        sharedMemory.detach();
        exit(-2);
    }
    sharedMemory.create(1);
    LocalChargerDaemon d;
    QDBusConnection daemonConnection = QDBusConnection::sessionBus();
    if(!daemonConnection.registerService(DAEMON_SERVICE_NAME)){
        qInfo() << "Can NOT connect to session DBus:" << daemonConnection.lastError().message();
        exit(-1);
    }
    daemonConnection.registerObject(DAEMON_SERVICE_PATH, DAEMON_SERVICE_NAME, &d, QDBusConnection::ExportAllSlots);

    return app.exec();
}
