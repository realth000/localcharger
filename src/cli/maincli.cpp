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
        case 'l':
            cli.getSenderStatus();
            cli.getRecverStatus();
            break;
        case 's':
            break;
        case 'r':
            break;
        case 'x':
            break;
        case 'h':
        default:
            printUsage();
            break;
        }
    }
    return exitCode;
}
