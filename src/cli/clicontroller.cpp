#include "clicontroller.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QRandomGenerator>
#include <QtCore/QSettings>
#include <QtCore/QThread>
#include <QtNetwork/QNetworkInterface>
#include "utils/networkinfohelper.h"

CliController::CliController(QObject *parent)
    : QObject(parent),
      m_sockerSenderIp(QStringLiteral(WEBSOCKET_SENDER_IP_DEFAULT)),
      m_socketSenderPort(WEBSOCKET_SENDER_PORT_DEFAULT),
      m_socketRecverUrl(QStringLiteral(WEBSOCKET_RECVER_URL_DEFAULT)),
      m_socketRecverPort(WEBSOCKET_RECVER_PORT_DEFAULT),
      m_socketSenderState(SenderState::Disconnected),
      m_socketRecverState(RecverState::Disconnected),
      m_configFilePath(QCoreApplication::applicationDirPath() + QStringLiteral(NATIVE_PATH_SEP) + QStringLiteral(APP_CONFIGFILE_NAME)),
      m_saveFileDirPath(QCoreApplication::applicationDirPath()),
      m_enableAutoConnect(false),
      m_localClientReadableName("default"),
      m_localClientId(QRandomGenerator::securelySeeded().bounded(1000, 10000)),
      m_localWorkingPort(WEBSOCKET_PORT_DEFAULT)
{

}
