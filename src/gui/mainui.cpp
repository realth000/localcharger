#include "mainui.h"
#include "ui_mainui.h"
#include <QThread>
#include "defines.h"
#include "iconinstaller.h"
#include "qssinstaller.h"
#include "titlebar.h"

MainUi::MainUi(QWidget *parent)
    : QWidget(parent)
      , ui(new Ui::MainUi)
      , m_socketSenderPort(WEBSOCKET_SENDER_PORT_DEFAULT)
      , m_socketRecverUrl(WEBSOCKET_RECVER_URL_DEFAULT)
      , m_socketSenderState(SenderState::Disconnected)
      , m_socketRecverState(RecverState::Disconnected)
      , m_ipTypeValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral(VALIDATOR_TYPE_IP_EXPRESSION))))
      , m_portTypeValidator(new QIntValidator(VALIDATOR_TYPE_PORT_MIN, VALIDATOR_TYPE_PORT_MAX))
      , m_pushButtonStyle(new PushButtonStyle)
{
    ui->setupUi(this);
    initUi();
    initConnections();
}

void MainUi::initUi()
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setFixedSize(this->width(), this->height());
    this->setStyleSheet(QssInstaller::QssInstallFromFile(":/stylesheet/stylesheet.css").arg(this->objectName(), "rgb(55,85,100)", "rgb(51,51,51)"));

    IconInstaller::installPushButtonIcon(ui->startSenderPushButton, ":/pic/start_connect.png");
    IconInstaller::installPushButtonIcon(ui->startRecverPushButton, ":/pic/start_connect.png");
    IconInstaller::installPushButtonIcon(ui->uptSenderWebConfigPushButton, ":/pic/reload.png");
    IconInstaller::installPushButtonIcon(ui->sendMsgButton, ":/pic/send.png");
    ui->startSenderPushButton->setStyle(m_pushButtonStyle);
    ui->startRecverPushButton->setStyle(m_pushButtonStyle);
    ui->uptSenderWebConfigPushButton->setStyle(m_pushButtonStyle);
    ui->sendMsgButton->setStyle(m_pushButtonStyle);

    // Title bar style
    ui->titleBar->setFixedWidth(this->width());
    ui->titleBar->setCloseIcon(TITLEBAR_CLOSEICON);
    ui->titleBar->setTitleText(TITLEBAR_TITLETEXT);
    ui->titleBar->setUseGradient(true);
    ui->titleBar->initUi(TitleBarMode::NoMaxButton, "rgb(240,255,255)", "rgb(93,94,95)",
                         "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(18,18,18), stop: 1 rgb(21,21,21))", "rgb(240,255,255)");
    ui->titleBar->setTitleIcon(TITLEBAR_TITLEICON);

    ui->msgSendTextEdit->setReadOnly(true);
    ui->msgRecvTextEdit->setReadOnly(true);
    ui->senderUrlLineEdit->setValidator(m_ipTypeValidator);
    ui->senderPortLineEdit->setValidator(m_portTypeValidator);

    updateSenderState(SenderState::Disconnected);
    updateRecverState(RecverState::Disconnected);
}

void MainUi::initConnections()
{
    // passing message
    connect(&m_socketRecver, &WebRecver::recvedMessage, this, &MainUi::recoredRecvedMsg);

    // passing sender state
    connect(&m_socketSender, &WebSender::senderConnected, this, &MainUi::onSenderConnected);
    connect(&m_socketSender, &WebSender::senderDisconnected, this, &MainUi::onSenderDisconnected);

    // passing recver state
    connect(&m_socketRecver, &WebRecver::recverConnected, this, &MainUi::onRecverConnected);
    connect(&m_socketRecver, &WebRecver::recverDisconnected, this, &MainUi::onRecverDisconnected);
}

MainUi::~MainUi()
{
    delete ui;
    delete m_ipTypeValidator;
    delete m_portTypeValidator;
    delete m_pushButtonStyle;
}


void MainUi::on_sendMsgButton_clicked()
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
        qDebug() << "Sender already listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        return;
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

void MainUi::on_startSenderPushButton_clicked()
{
    startSender(m_socketSenderPort);
}


void MainUi::on_startRecverPushButton_clicked()
{
    if(m_socketRecver.getRecverState() != QAbstractSocket::UnconnectedState){
        qDebug() << "Recver is already running" << m_socketRecver.recverUrl() << m_socketRecver.recverPort();
        return;
    }
    updateRecverState(RecverState::Connecting);
    m_socketRecver.start(m_socketRecverUrl);
    updateRecverState(RecverState::Connecting);
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


void MainUi::on_uptSenderWebConfigPushButton_clicked()
{
    url_t testUrl(QString("wss://%1:%2").arg(ui->senderUrlLineEdit->text(), ui->senderPortLineEdit->text()));
    if(!testUrl.isValid()){
        qDebug() << "invalid url" << testUrl.toString();
        return;
    }
    qDebug() << "update server url to" << testUrl.toString();
    m_socketRecverUrl = testUrl;
    m_socketRecver.start(m_socketRecverUrl);
}

