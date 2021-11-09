#ifndef WEBSENDER_H
#define WEBSENDER_H

#include <QtWebSockets/QWebSocket>
#include <QtCore/QObject>
#include <QtNetwork/QSslError>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

class WebSender : public QObject
{
    Q_OBJECT
public:
    explicit WebSender(quint16 port, QObject *parent = nullptr);
    ~WebSender() override;

private slots:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocketServer *m_socketServer;
    QList<QWebSocket *> m_clientsList;
};

#endif // WEBSENDER_H
