#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include "getopt.h"
#include "clicontroller.h"

void version()
{
    qInfo().noquote() << QString("LocalCharger:\n"
                       "Verion: %1\n"
                       "use \"LocalChargerCli -h to show help message.\"").arg(LOCALCHARGER_VERSION_STRING);
}

void printUsage()
{
    qInfo() << "LocalChargerCli\n"
                "Usage:\n"
                "  -q, --query       query status\n"
                "  -l, --list        list all connections\n"
                "  -s, --send        send connection to remote(ip:port)\n"
                "  -r, --remove      remove connection\n"
                "  -m, --message     send message\n"
                "  -f, --file        send file\n"
                "  -d, --dir         send dir\n"
                "  -x, --exit        disconnect and exit\n"
                "  -v, --version     check version\n"
                "  -h, --help        print this help message";
}

bool checkRemotePath(const QString &remotePath)
{
    if(remotePath.isEmpty()){
        return false;
    }
    QRegularExpression re("^((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))"
                            ":(6553[0-5]|655[0-2]\\d|65[0-4]\\d{2}|6[0-4]\\d{4}|[1-5]\\d{4}|[1-9]\\d{1,3}|[0-9])");
    QRegularExpressionMatch match = re.match(remotePath);
    return match.hasMatch() ? true : false;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    int c = 0;
    int option_index = 0;
    int exitCode = 0;
    CliController cli;
    if(!cli.getDaemonConnectionStatus()){
        return -1;
    }

    static struct option long_options[] ={
        {"query",        no_argument, 0, 'q'},
        {"list",         no_argument, 0, 'l'},
        {"send",   required_argument, 0, 's'},
        {"remove", optional_argument, 0, 'r'},
        {"message",required_argument, 0, 'm'},
        {"file",   required_argument, 0, 'f'},
        {"dir",    required_argument, 0, 'd'},
        {"exit",         no_argument, 0, 'x'},
        {"version",      no_argument, 0, 'v'},
        {"help",         no_argument, 0, 'h'},
        {0,                        0, 0,   0}
    };
    if(argc < 2){
        printUsage();
        return 0;
    }
    while((c = getopt_long(argc, argv, "qls:r::m:f:d:xvh", long_options, &option_index)) != -1){
        switch (c) {
        case 'q':
            cli.getStatus();
            return exitCode;
        case 'l':
            cli.getSenderStatus();
            cli.getRecverStatus();
            return exitCode;
        case 's':
            if(!checkRemotePath(optarg)){
                qInfo() << "Invalid remote path.\n"
                            "e.g. 192.168.1.1:3080";
                exitCode = -1;
            }
            else{
                cli.connectRemote(optarg);
            }
            return exitCode;
        case 'r':
            break;
        case 'm':
            if(cli.getSenderStatusCode() != static_cast<int>(SenderState::Connected)) {
                qInfo() << "Sender not connected";
                exitCode = -1;
                return exitCode;
            }
            cli.sendMessage(optarg);
            return exitCode;
        case 'f':
            {
                if(cli.getSenderStatusCode() != static_cast<int>(SenderState::Connected)) {
                    qInfo() << "Sender not connected";
                    exitCode = -1;
                    return exitCode;
                }
                QFileInfo fileInfo(optarg);
                if(!fileInfo.exists()){
                    qInfo() << "File not exists:" << optarg;
                    exitCode = -1;
                    return exitCode;
                }
                if(!fileInfo.isFile()){
                    qInfo() << "Not a file:" << optarg;
                    exitCode = -1;
                    return exitCode;
                }
                QDBusConnection daemonConnection = QDBusConnection::sessionBus();
                if(!daemonConnection.registerService(CLI_SERVICE_NAME)){
                    qInfo() << "Can NOT connect to session DBus:" << daemonConnection.lastError().message();
                    exitCode = -1;
                    return exitCode;
                }
                daemonConnection.registerObject(CLI_SERVICE_PATH, CLI_SERVICE_NAME, &cli, QDBusConnection::ExportAllSlots);
                cli.sendFile(fileInfo.absoluteFilePath());
                return app.exec();
            }
        case 'd':
            {
                if(cli.getSenderStatusCode() != static_cast<int>(SenderState::Connected)) {
                    qInfo() << "Sender not connected";
                    exitCode = -1;
                    return exitCode;
                }
                QFileInfo dirInfo(optarg);
                if(!dirInfo.exists()){
                    qInfo() << "Directory not exists:" << optarg;
                    exitCode = -1;
                    return exitCode;
                }
                if(!dirInfo.isDir()){
                    qInfo() << "Not a directory:" << optarg;
                    exitCode = -1;
                    return exitCode;
                }
                QDBusConnection daemonConnection = QDBusConnection::sessionBus();
                if(!daemonConnection.registerService(CLI_SERVICE_NAME)){
                    qInfo() << "Can NOT connect to session DBus:" << daemonConnection.lastError().message();
                    exitCode = -1;
                    return exitCode;
                }
                daemonConnection.registerObject(CLI_SERVICE_PATH, CLI_SERVICE_NAME, &cli, QDBusConnection::ExportAllSlots);
                cli.sendDir(dirInfo.absoluteFilePath());
                return app.exec();
            }
            return exitCode;
        case 'x':
            cli.exitDaemon();
            return exitCode;
        case 'v':
            version();
            return exitCode;
        case 'h':
        default:
            printUsage();
            return exitCode;
        }
    }
    return exitCode;
}
