﻿#include "src/gui/mainui.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainUi w;
    w.show();
    return a.exec();
}
