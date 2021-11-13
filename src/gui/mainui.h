﻿#ifndef MAINUI_H
#define MAINUI_H

#include <QtWidgets/QWidget>
#include <QtGui/QRegularExpressionValidator>
#include "src/core/websender.h"
#include "src/core/webrecver.h"
#include "proxystyle.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class MainUi; }
QT_END_NAMESPACE

class MainUi : public QWidget
{
Q_OBJECT

public:
    enum class SenderState{
        Disconnected = 0,
        Listening,
        Connected
    };
    enum class RecverState{
        Disconnected = 0,
        Connecting,
        Connected
    };

    Q_ENUM(SenderState)

    explicit MainUi(QWidget *parent = nullptr);
//    void initConfig();
    void initUi();
    void initConnections();
    ~MainUi();

private slots:
    void on_sendMsgPushButton_clicked();
    void recoredRecvedMsg(const QString &msg);
    void on_startSenderPushButton_clicked();
    void on_startRecverPushButton_clicked();
    void onSenderConnected();
    void onSenderDisconnected();
    void onRecverConnected();
    void onRecverDisconnected();
    void on_uptSenderWebConfigPushButton_clicked();
    void on_sendFilePushButton_clicked();
    bool updateWebSocketConfig();

    void on_saveConfigPushButton_clicked();

private:
    Ui::MainUi *ui;
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
    const QString configFilePath;

    // styles
    PushButtonStyle *m_pushButtonStyle;

    void startSender(const port_t &port);
    void updateSenderState(SenderState state);
    void updateRecverState(RecverState state);
    void loadConfig();
    void saveConfig();
};
#endif // MAINUI_H
