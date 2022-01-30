#include "mainui.h"
#include "ui_mainui.h"
#include <QtCore/QRandomGenerator>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtGui/QDesktopServices>
#include <QtNetwork/QNetworkInterface>
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>
#include <QtWidgets/QScrollBar>
#include "defines.h"
#include "iconinstaller.h"
#include "qssinstaller.h"
#include "titlebar.h"
#include "utils/networkinfohelper.h"
#include "messageboxexx.h"

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
      m_configFilePath(QCoreApplication::applicationDirPath() + QStringLiteral(NATIVE_PATH_SEP) + QStringLiteral(APP_CONFIGFILE_NAME)),
      m_saveFileDirPath(QCoreApplication::applicationDirPath()),
      m_enableAutoConnect(false),
      m_localClientReadableName("default"),
      m_localClientId(QRandomGenerator::securelySeeded().bounded(1000, 10000)),
      m_localWorkingPort(WEBSOCKET_PORT_DEFAULT),
      m_pushButtonStyle(new PushButtonStyle),
      m_hScrollStyle(new HorizontalScrollBarStyle),
      m_vScrollStyle(new VerticalScrollBarStyle),
      m_comboBoxStyle(new ComboBoxStyle),
      m_checkBoxStyle(new CheckBoxStyle)
{
    ui->setupUi(this);
    loadDefaultConfig();
    loadConfig();
    m_identifier = new WebIdentifier(m_localClientReadableName, m_localClientId, m_localWorkingPort, this);
    initUi();
    initConnections();
    getLocalIp();
    updateWebSocketConfig();
    updateSenderState(SenderState::Disconnected);
    updateRecverState(RecverState::Disconnected);
    m_identifier->boardcastIdentityMessage();
    qInfo() << "openssl lib state" << QSslSocket::sslLibraryBuildVersionNumber() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionNumber() << QSslSocket::sslLibraryVersionString();
    if(QSslSocket::sslLibraryVersionNumber() == 0){
        MessageBoxExY::critical("openssl初始化失败", QString("openssl初始化失败，当前openssl版本:\"%1\"(%2)").arg(QSslSocket::sslLibraryVersionString(), QString::number(QSslSocket::sslLibraryVersionNumber())));
    }
}

void MainUi::initUi()
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    this->setFixedSize(this->width(), this->height());
    this->setStyleSheet(QssInstaller::QssInstallFromFile(":/stylesheet/stylesheet.css").arg(this->objectName(), "rgb(55,85,100)", "rgb(51,51,51)"));

    IconInstaller::installPushButtonIcon(ui->startSenderPushButton, ":/pic/start_connect.png");
    IconInstaller::installPushButtonIcon(ui->startRecverPushButton, ":/pic/start_connect.png");
    IconInstaller::installPushButtonIcon(ui->connectSelectedClientPushButton, ":/pic/start_connect.png");
    IconInstaller::installPushButtonIcon(ui->broadcastPushButton, ":/pic/broadcast.png");
    IconInstaller::installPushButtonIcon(ui->updateWebConfigPushButton, ":/pic/reload.png");
    IconInstaller::installPushButtonIcon(ui->sendMsgPushButton, ":/pic/send.png");
    IconInstaller::installPushButtonIcon(ui->sendFilePushButton, ":/pic/send_file.png");
    IconInstaller::installPushButtonIcon(ui->saveConfigPushButton, ":/pic/save_config.png");
    IconInstaller::installPushButtonIcon(ui->selectSaveFilePathPushButton, ":/pic/openfolder3.png");
    IconInstaller::installPushButtonIcon(ui->openDownloadDirPushButton, ":/pic/openfolder.png");

    ui->startSenderPushButton->setStyle(m_pushButtonStyle);
    ui->startRecverPushButton->setStyle(m_pushButtonStyle);
    ui->connectSelectedClientPushButton->setStyle(m_pushButtonStyle);
    ui->broadcastPushButton->setStyle(m_pushButtonStyle);
    ui->updateWebConfigPushButton->setStyle(m_pushButtonStyle);
    ui->sendMsgPushButton->setStyle(m_pushButtonStyle);
    ui->sendFilePushButton->setStyle(m_pushButtonStyle);
    ui->saveConfigPushButton->setStyle(m_pushButtonStyle);
    ui->selectSaveFilePathPushButton->setStyle(m_pushButtonStyle);
    ui->openDownloadDirPushButton->setStyle(m_pushButtonStyle);
    ui->startSenderPushButton->setFocusPolicy(Qt::NoFocus);
    ui->startRecverPushButton->setFocusPolicy(Qt::NoFocus);
    ui->connectSelectedClientPushButton->setFocusPolicy(Qt::NoFocus);
    ui->broadcastPushButton->setFocusPolicy(Qt::NoFocus);
    ui->updateWebConfigPushButton->setFocusPolicy(Qt::NoFocus);
    ui->sendMsgPushButton->setFocusPolicy(Qt::NoFocus);
    ui->sendFilePushButton->setFocusPolicy(Qt::NoFocus);
    ui->saveConfigPushButton->setFocusPolicy(Qt::NoFocus);
    ui->selectSaveFilePathPushButton->setFocusPolicy(Qt::NoFocus);
    ui->openDownloadDirPushButton->setFocusPolicy(Qt::NoFocus);

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
    ui->saveFilePathLineEdit->setReadOnly(true);
    ui->saveFilePathLineEdit->setFocusPolicy(Qt::NoFocus);
    ui->senderUrlLineEdit->setValidator(m_ipTypeValidator);
    ui->senderPortLineEdit->setValidator(m_portTypeValidator);
    ui->recverPortLineEdit->setValidator(m_portTypeValidator);

    ui->msgSendTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->msgRecvTextEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->msgSendTextEdit->setFocusPolicy(Qt::ClickFocus);
    ui->msgRecvTextEdit->setFocusPolicy(Qt::ClickFocus);
    ui->msgReadyToSendTextEdit->setFocusPolicy(Qt::ClickFocus);
    ui->msgSendTextEdit->horizontalScrollBar()->setStyle(m_hScrollStyle);
    ui->msgSendTextEdit->verticalScrollBar()->setStyle(m_vScrollStyle);
    ui->msgRecvTextEdit->horizontalScrollBar()->setStyle(m_hScrollStyle);
    ui->msgRecvTextEdit->verticalScrollBar()->setStyle(m_vScrollStyle);
    ui->msgReadyToSendTextEdit->horizontalScrollBar()->setStyle(m_hScrollStyle);
    ui->msgReadyToSendTextEdit->verticalScrollBar()->setStyle(m_vScrollStyle);
    ui->msgSendTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->msgRecvTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->msgReadyToSendTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->senderUrlLineEdit->setText(m_sockerSenderIp);
    ui->senderPortLineEdit->setText(QString::number(m_socketSenderPort));
    ui->recverPortLineEdit->setText(QString::number(m_socketRecverPort));
    ui->saveFilePathLineEdit->setText(m_saveFileDirPath);

    this->setTabOrder(ui->recverPortLineEdit, ui->senderUrlLineEdit);
    this->setTabOrder(ui->senderUrlLineEdit, ui->senderPortLineEdit);
    this->setFocus();

    ui->recverUrlHintComboBox->setView(new QListView(ui->recverUrlHintComboBox));
    ui->recverUrlHintComboBox->setStyle(m_comboBoxStyle);

    ui->clientsListWidget->horizontalScrollBar()->setStyle(m_hScrollStyle);
    ui->clientsListWidget->verticalScrollBar()->setStyle(m_vScrollStyle);
    ui->clientNameLineEdit->setText(m_localClientReadableName);
    ui->clientNameLineEdit->setFocusPolicy(Qt::ClickFocus);

    ui->clientIdLabel->setText(QString::number(m_localClientId));
    ui->autoConnectComboBox->setStyle(m_checkBoxStyle);
    ui->autoConnectComboBox->setChecked(m_enableAutoConnect);

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

    // clear info before send file
    connect(&m_socketSender, &WebSender::prepareRecvFile, &m_socketRecver, &WebRecver::onPrepareRecvFile);

    // WebIdentifier
    connect(m_identifier, &WebIdentifier::identityMessageParsed, this, &MainUi::onIdentityMessageParsed);
    connect(m_identifier, &WebIdentifier::getClientToConnect, this, &MainUi::autoConnectToClinet);
    connect(m_identifier, &WebIdentifier::getAutoConnectReply, this, &MainUi::onGetAutoConnectReply);

    connect(ui->msgSendTextEdit, &QTextEdit::customContextMenuRequested, this, &MainUi::textEditContextMenu);
    connect(ui->msgRecvTextEdit, &QTextEdit::customContextMenuRequested, this, &MainUi::textEditContextMenu);
    connect(ui->msgReadyToSendTextEdit, &QTextEdit::customContextMenuRequested, this, &MainUi::textEditContextMenu);
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
    delete m_comboBoxStyle;
    delete m_checkBoxStyle;
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
        qInfo() << "Sender already listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort() << "stop first";
        stopSender();
    }

    if(m_socketSender.start(port)){
        qInfo() << "Sender start listening at" << m_socketSender.senderUrl() << m_socketSender.senderPort();
        updateSenderState(SenderState::Listening);
    }
    else{
        qInfo() << "Sender failed to listen" << m_socketSender.senderUrl() << m_socketSender.senderPort();
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
        ui->senderStateHintLabel->setText(tr("closed"));
        ui->senderStateHintPicLabel->setPixmap(QPixmap(":/pic/disconnected.png").scaled(ui->senderStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_socketSenderState = SenderState::Disconnected;
        break;
    case SenderState::Listening:
        ui->senderStateHintLabel->setText(tr("listening"));
        ui->senderStateHintPicLabel->setPixmap(QPixmap(":/pic/connecting.png").scaled(ui->senderStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_socketSenderState = SenderState::Listening;
        break;
    case SenderState::Connected:
        ui->senderStateHintLabel->setText(tr("connected"));
        ui->senderStateHintPicLabel->setPixmap(QPixmap(":/pic/connected.png").scaled(ui->senderStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_socketSenderState = SenderState::Connected;
        break;
    default:
        break;
    }
}

void MainUi::updateRecverState(RecverState state)
{
    // FIXME: Update m_socketRecverState
    switch (state) {
    case RecverState::Disconnected:
        ui->recverStateHintLabel->setText(tr("closed"));
        ui->recverStateHintPicLabel->setPixmap(QPixmap(":/pic/disconnected.png").scaled(ui->recverStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        break;
    case RecverState::Connecting:
        ui->recverStateHintLabel->setText(tr("connecting"));
        ui->recverStateHintPicLabel->setPixmap(QPixmap(":/pic/connecting.png").scaled(ui->recverStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        break;
    case RecverState::Connected:
        ui->recverStateHintLabel->setText(tr("connected"));
        ui->recverStateHintPicLabel->setPixmap(QPixmap(":/pic/connected.png").scaled(ui->recverStateHintPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        break;
    default:
        break;
    }
}

void MainUi::loadDefaultConfig()
{
    m_socketRecver.setFileSavePath(m_saveFileDirPath);
}

void MainUi::loadConfig()
{
    if(!QFileInfo::exists(m_configFilePath)){
        qInfo() << "config file not found, load default config";
        return;
    }
    const QSettings *configIni = new QSettings(m_configFilePath, QSettings::IniFormat);
    m_sockerSenderIp = configIni->value(APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH).toString();
    m_socketSenderPort = configIni->value(APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH).toInt();
    m_socketRecverPort = configIni->value(APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH).toInt();
    m_saveFileDirPath = configIni->value(APP_CONFIGFILE_WEBSOCKET_RECVER_FILE_SAVE_PATH).toString();
    m_localClientReadableName = configIni->value(APP_CONFIGFILE_CLIENT_READABLENAME_PATH).toString();
    m_enableAutoConnect = configIni->value(APP_CONFIGFILE_CLINET_AUTOCONNECT_PATH).toBool();
    delete configIni;
    m_localWorkingPort = m_socketRecverPort;
}

void MainUi::saveConfig()
{
    QSettings *configIni = new QSettings(m_configFilePath, QSettings::IniFormat);
    configIni->setValue(APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH, m_sockerSenderIp);
    configIni->setValue(APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH, m_socketSenderPort);
    configIni->setValue(APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH, m_socketRecverPort);
    configIni->setValue(APP_CONFIGFILE_WEBSOCKET_RECVER_FILE_SAVE_PATH, m_saveFileDirPath);
    configIni->setValue(APP_CONFIGFILE_CLIENT_READABLENAME_PATH, m_localClientReadableName);
    configIni->setValue(APP_CONFIGFILE_CLINET_AUTOCONNECT_PATH, m_enableAutoConnect);
    delete configIni;
}

void MainUi::getLocalIp()
{
    // Get local ip address and netmask
    // TODO: Support more than one ip and IPV6
#if defined(Q_OS_WINDOWS) || defined(Q_OS_WIN)
    const QList<IpInfo> ipList = NetworkInfoHelper::getLocalIpAddressEx();
#else
    const QList<IpInfo> ipList = NetworkInfoHelper::getLocalIpAddress();
#endif
    QStringList ipStringList;
    foreach(IpInfo ip, ipList){
        ipStringList << QString("%1/%2").arg(ip.ipV4Address, QString::number(ip.prefixLength));
    }
    ui->recverUrlHintComboBox->insertItems(0, ipStringList);
    // TODO: when have more than one ip
    if(ipList.length() > 0){
        m_identifier->setIdentityIp(ipList[0].ipV4Address);
        m_localIp = ipList[0].ipV4Address;
    }
}

void MainUi::addDetectedClients(const QString &ip, const QString &port, const QString &readableName, const QString &id)
{
    // FIXME: UI + clients
    ui->clientsListWidget->addItem(QString(tr("Name") + ": %1\n" +
                                           tr("ID") + ": %2\n" +
                                           tr("IP") + ": %3\n" +
                                           tr("Port") + ": %4").arg(readableName, id, ip, port));
}

void MainUi::onIdentityMessageParsed(const QString &ip, const QString &port, const QString &readableName, const QString &id)
{
    qInfo() << ip << m_localIp << port << m_socketRecverPort;
    if(ip == m_localIp && port.toInt() == m_socketRecverPort){
        return;
    }
    if(!m_clientsMap.contains(id)){
        addDetectedClients(ip, port, readableName, id);
        m_clientsMap.insert(id, port);
        return;
    }
    if(m_clientsMap[id] == readableName){
        // TODO: update
        return;
    }
    addDetectedClients(ip, port, readableName, id);
    m_clientsMap.insert(id, port);
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
        qInfo() << "Recver is already running" << m_socketRecver.recverUrl() << m_socketRecver.recverPort();
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
     m_socketSender.isSenderListening() ? updateSenderState(SenderState::Listening) : updateSenderState(SenderState::Disconnected);
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
        qInfo() << "invalid url" << testUrl.toString();
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
        qInfo() << "file not exists:" << filePath;
        return;
    }
    m_socketSender.sendFile(filePath) ? qInfo() << "send file finish:" << filePath : qInfo() << "error sending file:" << filePath;
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
        qInfo() << "Invalid sender ip set in config";
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
    qInfo() << "update" << m_sockerSenderIp << m_socketSenderPort << m_socketRecverPort;
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
    ui->msgSendTextEdit->append(QString("<font color=\"%3\">Sending file:</font>"
                                        " %1 "
                                        "<font color=\"%4\">(%2 bytes)</font>").
                                arg(fielPath, QString::number(fileSize), MSGSEND_TEXTEDIT_SENDING_HEAD_COLOR, MSGSEND_TEXTEDIT_SENDING_TAIL_COLOR));
}

void MainUi::onSendFileFinish(const QString &fielPath, const qint64 &sendBytes)
{
    ui->msgSendTextEdit->append(QString("<font color=\"%3\">File sended:</font>"
                                        " %1 "
                                        "<font color=\"%4\">(%2 bytes)</font>").
                                arg(fielPath, QString::number(sendBytes), MSGSEND_TEXTEDIT_SENDED_HEAD_COLOR, MSGSEND_TEXTEDIT_SENDED_TAIL_COLOR));
}

void MainUi::onRecvFileStart(const QString &fielPath, const qint64 &fileSize)
{
    ui->msgRecvTextEdit->append(QString("<font color=\"%3\">Recving file:</font>"
                                        " %1 "
                                        "<font color=\"%4\">(%2 bytes)</font>").
                                arg(fielPath, QString::number(fileSize), MSGRECV_TEXTEDIT_RECVING_HEAD_COLOR, MSGRECV_TEXTEDIT_RECVING_TAIL_COLOR));
}

void MainUi::onRecvFileFinish(const QString &fielPath, const qint64 &recvBytes)
{
    ui->msgRecvTextEdit->append(QString("<font color=\"%3\">File recvied:</font>"
                                        " %1 "
                                        "<font color=\"%4\">(%2 bytes)</font>").
                                arg(fielPath, QString::number(recvBytes), MSGRECV_TEXTEDIT_RECVED_HEAD_COLOR, MSGRECV_TEXTEDIT_RECVED_TAIL_COLOR));
}

void MainUi::on_selectSaveFilePathPushButton_clicked()
{
    QString savePath = QFileDialog::getExistingDirectory(this, "保存到", QCoreApplication::applicationDirPath());
    if(savePath.isEmpty()){
        return;
    }
    ui->saveFilePathLineEdit->setText(savePath);
}


void MainUi::on_saveFilePathLineEdit_textChanged(const QString &arg1)
{
    m_socketRecver.setFileSavePath(arg1);
    m_saveFileDirPath = arg1;
}

void MainUi::on_connectSelectedClientPushButton_clicked()
{
    if(ui->clientsListWidget->selectedItems().length() <= 0){
        qInfo() << "client not selected";
        return;
    }
    const QStringList configList = ui->clientsListWidget->selectedItems()[0]->text().split("\n");
    ui->senderUrlLineEdit->setText(configList[2].split(" ")[1]);
    ui->senderPortLineEdit->setText(configList[3].split(" ")[1]);
    on_updateWebConfigPushButton_clicked();
//    qInfo
    const url_t t(QString("wss://%1:%2").arg(ui->senderUrlLineEdit->text(), QString::number(12336)));
    if(!t.isValid()){
        qInfo() << "invalid url to identifier:" << t;
    }
    m_identifier->startAutoConnect(t);
}


void MainUi::on_broadcastPushButton_clicked()
{
    m_identifier->boardcastIdentityMessage();
}


void MainUi::on_clientNameLineEdit_textChanged(const QString &arg1)
{
    m_localClientReadableName = arg1;
    m_identifier->setIdentityReadableName(m_localClientReadableName);
}

void MainUi::autoConnectToClinet(const QString &ip, const QString &port)
{
    // TODO: autoconnect config check
    if(!m_enableAutoConnect){
        qInfo() << "auto connect isdisabled";
        return;
    }
    if(ip.isEmpty() || port.isEmpty()){
        qInfo() << "empty cient to auto connect, ip =" << ip << "port =" << port;
        return;
    }
    ui->senderUrlLineEdit->setText(ip);
    ui->senderPortLineEdit->setText(port);
    on_updateWebConfigPushButton_clicked();
    m_identifier->sendAutoConnectReply();
}

void MainUi::on_autoConnectComboBox_stateChanged(int arg1)
{
    arg1 > 0 ? m_enableAutoConnect = true : m_enableAutoConnect = false;
}

void MainUi::onGetAutoConnectReply()
{
    on_startRecverPushButton_clicked();
}

void MainUi::on_openDownloadDirPushButton_clicked()
{
    QDesktopServices::openUrl(QUrl("file:///" + m_saveFileDirPath, QUrl::TolerantMode));
}

void MainUi::textEditContextMenu(const QPoint &pos)
{
    QTextEdit *textEdit = reinterpret_cast<QTextEdit *>(sender());
    if(textEdit == nullptr){
        return;
    }
    QMenu *menu = new QMenu(textEdit);
    QAction *cutAction = new QAction("剪切", textEdit);
    QAction *copyAction = new QAction("复制", textEdit);
    QAction *pasteAction = new QAction("粘贴", textEdit);
    QAction *selectAllAction = new QAction("全选", textEdit);
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    connect(cutAction, &QAction::triggered, textEdit, &QTextEdit::cut);
    connect(copyAction, &QAction::triggered, textEdit, &QTextEdit::copy);
    connect(pasteAction, &QAction::triggered, textEdit, &QTextEdit::paste);
    connect(selectAllAction, &QAction::triggered, textEdit, &QTextEdit::selectAll);
    if(textEdit->isReadOnly()){
        pasteAction->setEnabled(false);
    }
    menu->addAction(cutAction);
    menu->addAction(copyAction);
    menu->addAction(pasteAction);
    menu->addAction(selectAllAction);
    menu->move(cursor().pos());
    menu->show();
}

