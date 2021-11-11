#include "src/gui/mainui.h"
#include <QtWidgets/QApplication>
#include <QFont>

#if QT_VERSION <= QT_VERSION_CHECK(5, 12, 4)
#define Q_OS_WINDOWS Q_OS_WIN
#endif

int main(int argc, char *argv[])
{
    QFont appFont;
#ifdef Q_OS_WINDOWS
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
