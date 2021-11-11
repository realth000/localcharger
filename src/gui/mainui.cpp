#include "mainui.h"
#include "ui_mainui.h"
#include <QThread>
#include "defines.h"

MainUi::MainUi(QWidget *parent)
    : QWidget(parent)
      , ui(new Ui::MainUi)
      , m_socketSenderPort(WEBSOCKET_SENDER_PORT_DEFAULT)
      , m_socketRecverUrl(WEBSOCKET_RECVER_URL_DEFAULT)
      , m_socketSenderState(SenderState::Disconnected)
      , m_socketRecverState(RecverState::Disconnected)
{
    ui->setupUi(this);
    initUi();
    initConnections();
}

void MainUi::initUi()
{
    ui->msgSendTextEdit->setReadOnly(true);
    ui->msgRecvTextEdit->setReadOnly(true);
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
        m_socketSenderState = SenderState::Disconnected;
        break;
    case SenderState::Listening:
        ui->senderStateHintLabel->setText("listening");
        m_socketSenderState = SenderState::Listening;
        break;
    case SenderState::Connected:
        ui->senderStateHintLabel->setText("connected");
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
        break;
    case RecverState::Connecting:
        ui->recverStateHintLabel->setText("connecting");
        break;
    case RecverState::Connected:
        ui->recverStateHintLabel->setText("connected");
        break;
    default:
        break;
    }
}

void MainUi::on_startSenderPushButton_clicked()
{
    startSender(m_socketSenderPort);
}


void MainUi::on_pushButton_clicked()
{
    if(m_socketRecver.getRecverState() != QAbstractSocket::UnconnectedState){
        qDebug() << "Recver is already running" << m_socketRecver.recverUrl() << m_socketRecver.recverPort();
        return;
    }
    updateRecverState(RecverState::Connecting);
    m_socketRecver.start(m_socketRecverUrl);
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

