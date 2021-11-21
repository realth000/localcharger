#include <QtGui/QFont>

#ifdef ENABLE_QML
#include <QtCore/QCoreApplication>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include "src/qml/qmlhandler.h"
#else
#include "src/gui/mainui.h"
#include <QtWidgets/QApplication>
#endif

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

#ifdef ENABLE_QML
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    app.setFont(appFont);
    qmlRegisterType<QmlHandler>("TH.QmlHandler", 1, 0, "QmlHandler");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qmlfiles/MainUi.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
#else
    QApplication a(argc, argv);
    a.setFont(appFont);
    MainUi w;
    w.show();
    return a.exec();
#endif
}
