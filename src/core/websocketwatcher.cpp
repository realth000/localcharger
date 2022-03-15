#include "websocketwatcher.h"
#include <QtCore/QtDebug>

WebSocketWatcher::WebSocketWatcher(QObject *parent)
    : QObject(parent),
      m_interruptTimer(this)
{
    m_interruptTimer.setInterval(SOCKETWATCHER_INTERRUPT_TIME);
    m_interruptTimer.setSingleShot(true);

    m_autoconnectTimer.setInterval(SOCKETWATCHER_AUTOCONNECT_TIME);
    m_autoconnectTimer.setSingleShot(true);

    initConnections();
}

void WebSocketWatcher::updateCurrentFile(QString fileName)
{
    m_fileName = fileName;
    m_interruptTimer.start();
}

void WebSocketWatcher::startAutoConnectTimeout()
{
    m_autoconnectTimer.start();
}

void WebSocketWatcher::finishAutoConnectTimeout()
{
    m_autoconnectTimer.stop();
}

void WebSocketWatcher::initConnections()
{
    connect(&m_interruptTimer, &QTimer::timeout, this, &WebSocketWatcher::onInterrupted);
    connect(&m_autoconnectTimer, &QTimer::timeout, this, &WebSocketWatcher::onAutoConnectTimeout);
}

void WebSocketWatcher::onInterrupted()
{
    qInfo() << "WebSocketWatcher: Transmission interrupted on file" << m_fileName;
}

void WebSocketWatcher::onAutoConnectTimeout()
{
    qInfo() << "WebSocketWatcher: Autoconnect timeout";
}
