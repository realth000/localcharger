#include <QtCore/QTranslator>
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

#ifdef Q_OS_ANDROID
#include <QtGui/QFontDatabase>
#endif

#ifdef ENABLE_VID
#include "vld.h"
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#ifdef ENABLE_QML
    QGuiApplication app(argc, argv);
#endif

#ifdef Q_OS_ANDROID
    QFontDatabase appFontDatabase;
    appFontDatabase.addApplicationFont(":/config/DejaVuSansMono-1.ttf");
#endif

    QFont appFont;
#if defined(Q_OS_WINDOWS) || defined(Q_OS_WIN)
    appFont.setFamily("Microsoft YaHei");
#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    appFont.setFamily("Dejavu Sans Mono");
#elif defined(Q_OS_ANDROID)
    appFont.setFamily("DejaVu Sans Mono");
    appFont.setWeight(QFont::Bold);
#endif
    appFont.setStyleStrategy(static_cast<QFont::StyleStrategy>(QFont::PreferAntialias | QFont::PreferQuality));

    // Setup translation
    QLocale locale = QLocale::system();
    QTranslator appTranslator;
    MainUi::AppLanguage appLanguage;

    switch (locale.script()) {
    case QLocale::SimplifiedChineseScript:
        appTranslator.load(QLatin1String(":/translation/localcharger_zh_CN.qm"));
        appLanguage = MainUi::AppLanguage::Zh_cn;
        break;
    default:
        appTranslator.load(QLatin1String(":/translation/localcharger_en.qm"));
        appLanguage = MainUi::AppLanguage::En;
    }

#ifdef ENABLE_QML
    QCoreApplication::setApplicationName(TITLEBAR_TITLETEXT);
    app.setFont(appFont);
    qmlRegisterType<QmlHandler>("TH.QmlHandler", 1, 0, "QmlHandler");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qmlfiles/MainUi.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    app.installTranslator(&appTranslator);
    engine.load(url);
    return app.exec();
#else
    QApplication a(argc, argv);

    a.installTranslator(&appTranslator);
    a.setFont(appFont);
    MainUi w(nullptr, appLanguage);
    w.show();
    return a.exec();
#endif
}
