#include "websocketwatcher.h"
#include <QtCore/QtDebug>

WebSocketWatcher::WebSocketWatcher(QObject *parent)
    : QObject(parent),
      m_interruptTimer(this)
{
    m_interruptTimer.setInterval(SOCKETWATCHER_INTERRUPT_TIME);
    m_interruptTimer.setSingleShot(true);

    initConnections();
}

void WebSocketWatcher::updateCurrentFile(QString fileName)
{
    m_fileName = fileName;
    m_interruptTimer.start();
}

void WebSocketWatcher::initConnections()
{
    connect(&m_interruptTimer, &QTimer::timeout, this, &WebSocketWatcher::onInterrupted);
}

void WebSocketWatcher::onInterrupted()
{
    qInfo() << "WebSocketWatcher: Transmission interrupted on file" << m_fileName;
}
