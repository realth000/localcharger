#ifndef MAINUI_H
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
    explicit MainUi(QWidget *parent = nullptr);
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
    void on_updateWebConfigPushButton_clicked();
    void on_sendFilePushButton_clicked();
    bool updateWebSocketConfig();
    void on_saveConfigPushButton_clicked();
    void onSendFileStart(const QString &fielPath, const qint64 &fileSize);
    void onSendFileFinish(const QString &fielPath, const qint64 &sendBytes);
    void onRecvFileStart(const QString &fielPath, const qint64 &fileSize);
    void onRecvFileFinish(const QString &fielPath, const qint64 &recvBytes);
    void on_selectSaveFilePathPushButton_clicked();
    void on_saveFilePathLineEdit_textChanged(const QString &arg1);

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
    const QString m_configFilePath;
    QString m_saveFileDirPath;

    // styles
    PushButtonStyle *m_pushButtonStyle;
    HorizontalScrollBarStyle *m_hScrollStyle;
    VerticalScrollBarStyle *m_vScrollStyle;
    ComboBoxStyle *m_comboBoxStyle;

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
#endif // MAINUI_H
