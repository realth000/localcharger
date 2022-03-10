#ifndef WEBSOCKETWATCHER_H
#define WEBSOCKETWATCHER_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#define SOCKETWATCHER_INTERRUPT_TIME 3000 // 3s

class WebSocketWatcher : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketWatcher(QObject *parent = nullptr);

public slots:
    void updateCurrentFile(QString fileName);

signals:

private:
    QTimer m_interruptTimer;
    QString m_fileName;

    void initConnections();

private slots:
    void onInterrupted();
};

#endif // WEBSOCKETWATCHER_H
