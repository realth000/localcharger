#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QSharedMemory>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include "daemon.h"
#include "defines.h"

#if 0
// test
#include "qhttpserver.hpp"
#include "qhttpserverconnection.hpp"
#include "qhttpserverrequest.hpp"
#include "qhttpserverresponse.hpp"
#include "unixcatcher.hpp"
#endif
int main(int argc, char *argv[])
{
#if 1
    QCoreApplication a2(argc, argv);
    Test t;
    return a2.exec();
#else
    QCoreApplication a3(argc, argv);
    using namespace qhttp::server;
    QHttpServer server(&a3);
    server.listen(QHostAddress::Any, 8080,
                  [](QHttpRequest *req, QHttpResponse *res){
                    res->setStatusCode(qhttp::ESTATUS_OK);
    });
    if(!server.isListening()){
        qInfo() << "failed to listen";
        return -1;
    }
    return a3.exec();
#endif


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
