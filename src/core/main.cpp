#include "src/gui/mainui.h"
#include <QtWidgets/QApplication>
#include <QFont>

#ifdef ENABLE_VID
#include "vld.h"
#endif

int main(int argc, char *argv[])
{
    QFont appFont;
#if defined(Q_OS_WINDOWS) || defined(Q_OS_WIN)
    appFont.setFamily("Microsoft YaHei");
#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    appFont.setFamily("Dejavu Sans Mono");
#endif
    appFont.setStyleStrategy(QFont::PreferAntialias);

    QApplication a(argc, argv);
    a.setFont(appFont);
    MainUi w;
    w.show();
    return a.exec();
}
