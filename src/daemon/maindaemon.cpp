#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QSharedMemory>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include "daemon.h"
#include "defines.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QSharedMemory sharedMemory("local_charger_daemon_shared_mem11");
    if(sharedMemory.attach()){
        QDBusInterface daemonInterface(DAEMON_SERVICE_NAME, DAEMON_SERVICE_PATH, DAEMON_SERVICE_NAME, QDBusConnection::sessionBus());
        if(daemonInterface.isValid()){
            qInfo() << "daemon already started";
            exit(-2);
        }
        sharedMemory.detach();
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
