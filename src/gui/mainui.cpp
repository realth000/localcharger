#include "mainui.h"
#include "ui_mainui.h"
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkInterface>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QScrollBar>
#include "defines.h"
#include "iconinstaller.h"
#include "qssinstaller.h"
#include "titlebar.h"
#include "utils/networkinfohelper.h"

MainUi::MainUi(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::MainUi),
      m_sockerSenderIp(QStringLiteral(WEBSOCKET_SENDER_IP_DEFAULT)),
      m_socketSenderPort(WEBSOCKET_SENDER_PORT_DEFAULT),
      m_socketRecverUrl(QStringLiteral(WEBSOCKET_RECVER_URL_DEFAULT)),
      m_socketRecverPort(WEBSOCKET_RECVER_PORT_DEFAULT),
      m_socketSenderState(SenderState::Disconnected),
      m_socketRecverState(RecverState::Disconnected),
      m_ipTypeValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral(VALIDATOR_TYPE_IP_EXPRESSION)))),
      m_portTypeValidator(new QIntValidator(VALIDATOR_TYPE_PORT_MIN, VALIDATOR_TYPE_PORT_MAX)),
      configFilePath(QCoreApplication::applicationDirPath() + QStringLiteral(NATIVE_PATH_SEP) + QStringLiteral(APP_CONFIGFILE_NAME)),
      m_pushButtonStyle(new PushButtonStyle),
      m_hScrollStyle(new HorizontalScrollBarStyle),
      m_vScrollStyle(new VerticalScrollBarStyle)
{
    ui->setupUi(this);
    loadConfig();
    initUi();
    initConnections();

    getLocalIp();

    m_socketRecver.setFileSavePath(QCoreApplication::applicationDirPath());

    updateWebSocketConfig();
    updateSenderState(SenderState::Disconnected);
    updateRecverState(RecverState::Disconnected);
}

void MainUi::initUi()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    this->setStyleSheet(QssInstaller::QssInstallFromFile(":/stylesheet/stylesheet.css").arg(this->objectName(), "rgb(55,85,100)", "rgb(51,51,51)"));

    IconInstaller::installPushButtonIcon(ui->startSenderPushButton, ":/pic/start_connect.png");
    IconInstaller::installPushButtonIcon(ui->startRecverPushButton, ":/pic/start_connect.png");
    IconInstaller::installPushButtonIcon(ui->updateWebConfigPushButton, ":/pic/reload.png");
    IconInstaller::installPushButtonIcon(ui->sendMsgPushButton, ":/pic/send.png");
    IconInstaller::installPushButtonIcon(ui->sendFilePushButton, ":/pic/send_file.png");
    IconInstaller::installPushButtonIcon(ui->saveConfigPushButton, ":/pic/save_config.png");
    ui->startSenderPushButton->setStyle(m_pushButtonStyle);
    ui->startRecverPushButton->setStyle(m_pushButtonStyle);
    ui->updateWebConfigPushButton->setStyle(m_pushButtonStyle);
    ui->sendMsgPushButton->setStyle(m_pushButtonStyle);
    ui->sendFilePushButton->setStyle(m_pushButtonStyle);
    ui->saveConfigPushButton->setStyle(m_pushButtonStyle);
    ui->startSenderPushButton->setFocusPolicy(Qt::NoFocus);
    ui->startRecverPushButton->setFocusPolicy(Qt::NoFocus);
    ui->updateWebConfigPushButton->setFocusPolicy(Qt::NoFocus);
    ui->sendMsgPushButton->setFocusPolicy(Qt::NoFocus);
    ui->sendFilePushButton->setFocusPolicy(Qt::NoFocus);
    ui->saveConfigPushButton->setFocusPolicy(Qt::NoFocus);

    // Title bar style
    ui->titleBar->setFixedWidth(this->width());
    ui->titleBar->setCloseIcon(QStringLiteral(TITLEBAR_CLOSEICON));
    ui->titleBar->setTitleText(QStringLiteral(TITLEBAR_TITLETEXT));
    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBarMode::NoMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(QStringLiteral(TITLEBAR_TITLEICON));

    ui->msgSendTextEdit->setReadOnly(true);
    ui->msgRecvTextEdit->setReadOnly(true);
    ui->senderUrlLineEdit->setValidator(m_ipTypeValidator);
    ui->senderPortLineEdit->setValidator(m_portTypeValidator);
    ui->recverPortLineEdit->setValidator(m_portTypeValidator);

    ui->msgSendTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->msgRecvTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->msgSendTextEdit->setFocusPolicy(Qt::NoFocus);
    ui->msgRecvTextEdit->setFocusPolicy(Qt::NoFocus);
    ui->msgReadyToSendTextEdit->setFocusPolicy(Qt::ClickFocus);
    ui->msgSendTextEdit->horizontalScrollBar()->setStyle(m_hScrollStyle);
    ui->msgSendTextEdit->verticalScrollBar()->setStyle(m_vScrollStyle);
    ui->msgRecvTextEdit->horizontalScrollBar()->setStyle(m_hScrollStyle);
    ui->msgRecvTextEdit->verticalScrollBar()->setStyle(m_vScrollStyle);
    ui->msgReadyToSendTextEdit->horizontalScrollBar()->setStyle(m_hScrollStyle);
    ui->msgReadyToSendTextEdit->verticalScrollBar()->setStyle(m_vScrollStyle);

    ui->senderUrlLineEdit->setText(m_sockerSenderIp);
    ui->senderPortLineEdit->setText(QString::number(m_socketSenderPort));
    ui->recverPortLineEdit->setText(QString::number(m_socketRecverPort));

    this->setTabOrder(ui->recverPortLineEdit, ui->senderUrlLineEdit);
    this->setTabOrder(ui->senderUrlLineEdit, ui->senderPortLineEdit);
    this->setFocus();
}

void MainUi::initConnections()
{
    // passing sender state
    connect(&m_socketSender, &WebSender::senderConnected, this, &MainUi::onSenderConnected);
    connect(&m_socketSender, &WebSender::senderDisconnected, this, &MainUi::onSenderDisconnected);
    // passing sender message
    connect(&m_socketSender, &WebSender::sendFileStart, this, &MainUi::onSendFileStart);
    connect(&m_socketSender, &WebSender::sendFileFinish, this, &MainUi::onSendFileFinish);

    // passing recver state
    connect(&m_socketRecver, &WebRecver::recverConnected, this, &MainUi::onRecverConnected);
    connect(&m_socketRecver, &WebRecver::recverDisconnected, this, &MainUi::onRecverDisconnected);
    // passing recver message
    connect(&m_socketRecver, &WebRecver::recvedMessage, this, &MainUi::recoredRecvedMsg);
    connect(&m_socketRecver, &WebRecver::recvFileStart, this, &MainUi::onRecvFileStart);
    connect(&m_socketRecver, &WebRecver::recvFileFinish, this, &MainUi::onRecvFileFinish);
}

MainUi::~MainUi()
{
    delete ui;
    delete m_ipTypeValidator;
    delete m_portTypeValidator;

    // delete styles
    delete m_pushButtonStyle;
    delete m_hScrollStyle;
    delete m_vScrollStyle;
}


void MainUi::on_sendMsgPushButton_clicked()
{
    if(m_socketSenderState != SenderState::Connected){
        return;
    }
#if MSG_SEND_TYPE_HTML
    const QString &msg(ui->msgReadyToSendTextEdit->toHtml());
#else
    const QString &msg(ui->msgReadyToSendTextEdit->toPlainText());
#endif
    if(msg.isEmpty()){
        return;
    }
    m_socketSender.sendMessage(msg);
    ui->msgReadyToSendTextEdit->clear();
    ui->msgSendTextEdit->append(msg);
}

void MainUi::recoredRecvedMsg(const QString &msg)
{
    ui->msgRecvTextEdit->append(msg);
}

void MainUi::startSender(const port_t &port)
{
    if(m_socketSender.isSenderListening()){
        qDebug() << "Sender already listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort() << "stop first";
        stopSender();
    }

    if(m_socketSender.start(port)){
        qDebug() << "Sender start listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(SenderState::Listening);
    }
    else{
        qDebug() << "Sender failed to listen" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(SenderState::Disconnected);
    }
}

void MainUi::stopSender()
{
    m_socketSender.stop();
    updateSenderState(SenderState::Disconnected);
}

void MainUi::startRecver(const url_t &url)
{
    m_socketRecver.stop();
    updateRecverState(RecverState::Connecting);
    m_socketRecver.start(url);
}

void MainUi::stopRecver()
{
    m_socketRecver.stop();
    updateRecverState(RecverState::Disconnected);
}

void MainUi::updateSenderState(SenderState state)
{
    switch (state) {
    case SenderState::Disconnected:
        ui->senderStateHintLabel->setText("closed");
        ui->senderStateHintPicLabel->setPixmap(QPixmap(":/pic/disconnected.png").scaled(ui->senderStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_socketSenderState = SenderState::Disconnected;
        break;
    case SenderState::Listening:
        ui->senderStateHintLabel->setText("listening");
        ui->senderStateHintPicLabel->setPixmap(QPixmap(":/pic/connecting.png").scaled(ui->senderStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_socketSenderState = SenderState::Listening;
        break;
    case SenderState::Connected:
        ui->senderStateHintLabel->setText("connected");
        ui->senderStateHintPicLabel->setPixmap(QPixmap(":/pic/connected.png").scaled(ui->senderStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_socketSenderState = SenderState::Connected;
        break;
    default:
        break;
    }
}

void MainUi::updateRecverState(RecverState state)
{
    switch (state) {
    case RecverState::Disconnected:
        ui->recverStateHintLabel->setText("closed");
        ui->recverStateHintPicLabel->setPixmap(QPixmap(":/pic/disconnected.png").scaled(ui->recverStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        break;
    case RecverState::Connecting:
        ui->recverStateHintLabel->setText("connecting");
        ui->recverStateHintPicLabel->setPixmap(QPixmap(":/pic/connecting.png").scaled(ui->recverStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        break;
    case RecverState::Connected:
        ui->recverStateHintLabel->setText("connected");
        ui->recverStateHintPicLabel->setPixmap(QPixmap(":/pic/connected.png").scaled(ui->recverStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        break;
    default:
        break;
    }
}

void MainUi::loadConfig()
{
    if(!QFileInfo::exists(configFilePath)){
        qDebug() << "config file not found, load default config";
        return;
    }
    const QSettings *configIni = new QSettings(configFilePath, QSettings::IniFormat);
    m_sockerSenderIp = configIni->value(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH)).toString();
    m_socketSenderPort = configIni->value(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH)).toInt();
    m_socketRecverPort = configIni->value(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH)).toInt();
    delete configIni;
}

void MainUi::saveConfig()
{
    QSettings *configIni = new QSettings(configFilePath, QSettings::IniFormat);
    configIni->setValue(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH), m_sockerSenderIp);
    configIni->setValue(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH), m_socketSenderPort);
    configIni->setValue(QStringLiteral(APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH), m_socketRecverPort);
    delete configIni;
}

void MainUi::getLocalIp()
{
    // Get local ip address and netmask
    // TODO: Support more than one ip and IPV6
    const QList<IpInfo> ipList = NetworkInfoHelper::getLocalIpAddress();
    if(ipList.length() == 1){
        ui->recverUrlHintLabel->setText(QString("%1/%2").arg(ipList[0].ipV4Address, QString::number(ipList[0].prefixLength)));
    }
    else{
        ui->recverUrlHintLabel->setText(QStringLiteral("获取失败，需要自行查看"));
        qDebug() << "需要自行查看";
    }
}

void MainUi::on_startSenderPushButton_clicked()
{
    stopSender();
    startSender(m_socketSenderPort);
}


void MainUi::on_startRecverPushButton_clicked()
{
    stopRecver();
    if(m_socketRecver.getRecverState() != QAbstractSocket::UnconnectedState){
        qDebug() << "Recver is already running" << m_socketRecver.recverUrl() << m_socketRecver.recverPort();
        return;
    }
    m_socketRecverUrl.setHost(ui->senderUrlLineEdit->text());
    m_socketRecverUrl.setPort(ui->senderPortLineEdit->text().toInt());
    startRecver(m_socketRecverUrl);
}

void MainUi::onSenderConnected()
{
    updateSenderState(SenderState::Connected);
}

void MainUi::onSenderDisconnected()
{
    updateSenderState(SenderState::Disconnected);
}

void MainUi::onRecverConnected()
{
    updateRecverState(RecverState::Connected);
}

void MainUi::onRecverDisconnected()
{
    updateRecverState(RecverState::Disconnected);
}


void MainUi::on_updateWebConfigPushButton_clicked()
{
    if(!updateWebSocketConfig()){
        return;
    }
    const url_t testUrl(QString("wss://%1:%2").arg(ui->senderUrlLineEdit->text(), ui->senderPortLineEdit->text()));
    if(!testUrl.isValid()){
        qDebug() << "invalid url" << testUrl.toString();
        return;
    }
    m_socketRecverUrl = testUrl;
    on_startSenderPushButton_clicked();
    on_startRecverPushButton_clicked();
}


void MainUi::on_sendFilePushButton_clicked()
{
    if(m_socketSenderState != SenderState::Connected){
        return;
    }
    QString filePath = QFileDialog::getOpenFileName(this, "发送文件", QCoreApplication::applicationFilePath());
    if(filePath.isEmpty()){
        return;
    }
    QFile fileToSend(filePath);
    if(!fileToSend.exists()){
        qDebug() << "file not exists:" << filePath;
        return;
    }
    if(!fileToSend.open(QIODevice::ReadOnly)){
        qDebug() << "can not open file" << filePath;
        return;
    }
    m_socketSender.sendFile(filePath);
    fileToSend.close();
    qDebug() << "send file finish:" << filePath;
}

bool MainUi::updateWebSocketConfig()
{
    bool ret = true;
    int pos = 0;
    QString test = ui->senderUrlLineEdit->text();
    style()->unpolish(ui->senderUrlLineEdit);
    style()->unpolish(ui->senderPortLineEdit);
    style()->unpolish(ui->recverPortLineEdit);
    if(m_ipTypeValidator->validate(test, pos) == QValidator::Acceptable){
        m_sockerSenderIp = ui->senderUrlLineEdit->text();
        ui->senderUrlLineEdit->setProperty(LINEEDIT_PROPERTY_TEXTVALID_NAME, QStringLiteral(LINEEDIT_PROPERTY_TEXTVALID_VALID));
    }
    else{
        qDebug() << "Invalid sender ip set in config";
        ui->senderUrlLineEdit->setProperty(LINEEDIT_PROPERTY_TEXTVALID_NAME, QStringLiteral(LINEEDIT_PROPERTY_TEXTVALID_INVALID));
        ret = false;
    }
    if(!ui->senderPortLineEdit->text().isEmpty()){
        m_socketSenderPort = ui->senderPortLineEdit->text().toInt();
        ui->senderPortLineEdit->setProperty(LINEEDIT_PROPERTY_TEXTVALID_NAME, QStringLiteral(LINEEDIT_PROPERTY_TEXTVALID_VALID));
    }
    else{
        ui->senderPortLineEdit->setProperty(LINEEDIT_PROPERTY_TEXTVALID_NAME, QStringLiteral(LINEEDIT_PROPERTY_TEXTVALID_INVALID));
        ret = false;
    }
    if(!ui->recverPortLineEdit->text().isEmpty()){
        m_socketRecverPort = ui->recverPortLineEdit->text().toInt();
        ui->recverPortLineEdit->setProperty(LINEEDIT_PROPERTY_TEXTVALID_NAME, QStringLiteral(LINEEDIT_PROPERTY_TEXTVALID_VALID));
    }
    else{
        ui->recverPortLineEdit->setProperty(LINEEDIT_PROPERTY_TEXTVALID_NAME, QStringLiteral(LINEEDIT_PROPERTY_TEXTVALID_INVALID));
        ret = false;
    }
    style()->polish(ui->senderUrlLineEdit);
    style()->polish(ui->senderPortLineEdit);
    style()->polish(ui->recverPortLineEdit);
    qDebug() << "update" << m_sockerSenderIp << m_socketSenderPort << m_socketRecverPort;
    return ret;
}


void MainUi::on_saveConfigPushButton_clicked()
{
    if(updateWebSocketConfig()){
        saveConfig();
    }
}

void MainUi::onSendFileStart(const QString &fielPath, const qint64 &fileSize)
{
    ui->msgSendTextEdit->append(QString("<font color=\"%3\">Sending file:"
                                        "</font> %1 "
                                        "<font color=\"%4\">(%2 bytes)"
                                        "</font>").
                                arg(fielPath, QString::number(fileSize), MSGSEND_TEXTEDIT_SENDING_HEAD_COLOR, MSGSEND_TEXTEDIT_SENDING_TAIL_COLOR));
}

void MainUi::onSendFileFinish(const QString &fielPath, const qint64 &sendBytes)
{
    ui->msgSendTextEdit->append(QString("<font color=\"%3\">File sended:"
                                        "</font> %1 "
                                        "<font color=\"%4\">(%2 bytes)"
                                        "</font>").
                                arg(fielPath, QString::number(sendBytes), MSGSEND_TEXTEDIT_SENDED_HEAD_COLOR, MSGSEND_TEXTEDIT_SENDED_TAIL_COLOR));
}

void MainUi::onRecvFileStart(const QString &fielPath, const qint64 &fileSize)
{
    ui->msgRecvTextEdit->append(QString("<font color=\"%3\">Recving file:"
                                        "</font> %1 "
                                        "<font color=\"%4\">(%2 bytes)"
                                        "</font>").
                                arg(fielPath, QString::number(fileSize), MSGRECV_TEXTEDIT_RECVING_HEAD_COLOR, MSGRECV_TEXTEDIT_RECVING_TAIL_COLOR));
}

void MainUi::onRecvFileFinish(const QString &fielPath, const qint64 &recvBytes)
{
    ui->msgRecvTextEdit->append(QString("<font color=\"%3\">File recvied:"
                                        "</font> %1 "
                                        "<font color=\"%4\">(%2 bytes)"
                                        "</font>").
                                arg(fielPath, QString::number(recvBytes), MSGRECV_TEXTEDIT_RECVED_HEAD_COLOR, MSGRECV_TEXTEDIT_RECVED_TAIL_COLOR));
}
