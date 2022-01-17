#include <QtCore/QCoreApplication>
#include "clicontroller.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    CliController cli;
    return a.exec();
}
