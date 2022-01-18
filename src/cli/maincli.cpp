#include <QtCore/QRegularExpression>
#include "getopt.h"
#include "clicontroller.h"

void printUsage()
{
    qDebug() << "LocalChargerCli\n"
                "Usage:\n"
                "-q, --query       query status\n"
                "-l, --list        list all connections\n"
                "-s, --send        send connection to remote(ip:port)\n"
                "-r, --remove      remove connection\n"
                "-x, --exit        disconnect and exit\n"
                "-h, --help        print this help message";
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
        {"exit",         no_argument, 0, 'x'},
        {"help",         no_argument, 0, 'h'},
        {0,                        0, 0,   0}
    };
    if(argc < 2){
        printUsage();
        return 0;
    }
    while((c = getopt_long(argc, argv, "qls:r::xh", long_options, &option_index)) != -1){
        switch (c) {
        case 'q':
            cli.getStatus();
            break;
        case 'l':
            cli.getSenderStatus();
            cli.getRecverStatus();
            break;
        case 's':
            if(!checkRemotePath(optarg)){
                qDebug() << "Invalid remote path.\n"
                            "e.g. 192.168.1.1:8080";
                break;
            }
            cli.connectRemote(optarg);
            break;
        case 'r':
            break;
        case 'x':
            cli.exitDaemon();
            break;
        case 'h':
        default:
            printUsage();
            break;
        }
    }
    return exitCode;
}
