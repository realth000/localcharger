#ifndef QMLHANDLER_H
#define QMLHANDLER_H

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtGui/QRegularExpressionValidator>
#include "core/webrecver.h"
#include "core/websender.h"
#include "defines.h"
#include "src/gui/mainui.h"

class QmlHandler : public QObject
{
    Q_OBJECT
public:
    explicit QmlHandler(QObject *parent = nullptr);

signals:
    void qmlMessageInfo(QString msg);
    void qmlUpdateSenderState(SenderState state);
    void qmlUpdateRecverState(RecverState state);
    void qmlUpdateLocalUrlLists(QStringList ipStringList);

public slots:
    void initHandler();

private:
    WebSender m_socketSender;
    WebRecver m_socketRecver;
    QString m_sockerSenderIp;
    port_t m_socketSenderPort;
    url_t m_socketRecverUrl;
    port_t m_socketRecverPort;
    SenderState m_socketSenderState;
    RecverState m_socketRecverState;
    QRegularExpressionValidator *m_ipTypeValidator;
    QIntValidator *m_portTypeValidator;
    const QString m_configFilePath;
    QString m_saveFileDirPath;

    void startSender(const port_t &port);
    void stopSender();
    void startRecver(const url_t &url);
    void stopRecver();
    void updateSenderState(SenderState state);
    void updateRecverState(RecverState state);
    void loadDefaultConfig();
    void loadConfig();
    void saveConfig();
    void getLocalIp();
};

#endif // QMLHANDLER_H
