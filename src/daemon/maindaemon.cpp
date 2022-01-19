#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include "daemon.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QDBusConnection daemonConnection = QDBusConnection::sessionBus();
    if(!daemonConnection.registerService("th000.localcharger")){
        qDebug() << "Can NOT connect to session DBus:" << daemonConnection.lastError().message();
        exit(-1);
    }
    LocalChargerDaemon d;
    daemonConnection.registerObject("/th000/LocalChargerDaemon", &d, QDBusConnection::ExportAllSlots);

    return app.exec();
}
