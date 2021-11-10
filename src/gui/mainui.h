#ifndef MAINUI_H
#define MAINUI_H

#include <QtWidgets/QWidget>
#include "src/core/websender.h"
#include "src/core/webrecver.h"

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
    void initUi();
    void initConnections();
    ~MainUi();

private slots:
    void on_sendMsgButton_clicked();
    void recoredRecvedMsg(const QString &msg);
    void on_startSenderPushButton_clicked();
    void on_pushButton_clicked();
    void onSenderConnected();
    void onSenderDisconnected();
    void onRecverConnected();
    void onRecverDisconnected();

private:
    Ui::MainUi *ui;
    WebSender m_socketSender;
    WebRecver m_socketRecver;
    port_t m_socketSenderPort;
    QUrl m_socketRecverUrl;

    void startSender(const port_t &port);
    void updateSenderState(SenderState state);
    void updateRecverState(RecverState state);
};
#endif // MAINUI_H
