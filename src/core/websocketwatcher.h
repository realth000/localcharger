#ifndef WEBSOCKETWATCHER_H
#define WEBSOCKETWATCHER_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#define SOCKETWATCHER_INTERRUPT_TIME 3000 // 3s
#define SOCKETWATCHER_AUTOCONNECT_TIME 5000 // 5s

class WebSocketWatcher : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketWatcher(QObject *parent = nullptr);

public slots:
    void updateCurrentFile(QString fileName);
    void startAutoConnectTimeout();
    void finishAutoConnectTimeout();

signals:

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
