#ifndef WEBSOCKETWATCHER_H
#define WEBSOCKETWATCHER_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "defines.h"

#define SOCKETWATCHER_INTERRUPT_TIME 3000 // 3s
#define SOCKETWATCHER_AUTOCONNECT_TIME 5000 // 5s

class WebSocketWatcher : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketWatcher(QObject *parent = nullptr);

signals:
    void watcherMessaged(MBoxLevel level, QString msg);

public slots:
    void updateCurrentFile(QString fileName);
    void startAutoConnectTimeout();
    void finishAutoConnectTimeout();

private:
    QTimer m_interruptTimer;
    QTimer m_autoconnectTimer;
    QString m_fileName;

    void initConnections();

private slots:
    void onInterrupted();
    void onAutoConnectTimeout();
};

#endif // WEBSOCKETWATCHER_H
