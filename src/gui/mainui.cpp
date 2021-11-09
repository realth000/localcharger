﻿#include "mainui.h"
#include "ui_mainui.h"


#include <QThread>

MainUi::MainUi(QWidget *parent)
    : QWidget(parent)
      , ui(new Ui::MainUi)
      , sender(1234, this)
      , recver(QUrl(QStringLiteral("wss://localhost:1234")), this)
{
    ui->setupUi(this);
    initUi();

    connect(&recver, &WebRecver::recvedMessage, this, &MainUi::recoredRecvedMsg);
}

void MainUi::initUi()
{
    ui->msgSendTextEdit->setReadOnly(true);
    ui->msgRecvTextEdit->setReadOnly(true);
}

MainUi::~MainUi()
{
    delete ui;
}


void MainUi::on_sendMsgButton_clicked()
{
    const QString &msg(ui->msgReadyToSendTextEdit->toHtml());
    if(msg.isEmpty()){
        return;
    }
    recver.sendMessage(msg);
    ui->msgReadyToSendTextEdit->clear();
    ui->msgSendTextEdit->append(msg);
}

void MainUi::recoredRecvedMsg(const QString &msg)
{
    ui->msgRecvTextEdit->append(msg);
}

