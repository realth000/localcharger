#ifndef WEBRECVER_H
#define WEBRECVER_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>

class WebRecver : public QObject
{
    Q_OBJECT
public:
    explicit WebRecver(const QUrl &url, QObject *parent = nullptr);

signals:
    void recvedMessage(const QString msg);

public slots:
    void sendMessage(const QString &msg);

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocket m_socket;
};

#endif // WEBRECVER_H
