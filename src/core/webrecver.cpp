#include "webrecver.h"

#include <QTextDocument>

WebRecver::WebRecver(const QUrl &url, QObject *parent) : QObject(parent)
{
    connect(&m_socket, &QWebSocket::connected, this, &WebRecver::onConnected);
    connect(&m_socket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors), this, &WebRecver::onSslErrors);
    m_socket.open(QUrl(url));
}

void WebRecver::sendMessage(const QString &msg)
{
    m_socket.sendTextMessage(msg);
}

void WebRecver::onConnected()
{
    qDebug() << "WebSocket connected";
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &WebRecver::onTextMessageReceived);
}

void WebRecver::onTextMessageReceived(QString message)
{
    qDebug() << "Message received:" << QTextDocument(message).toRawText();
    emit recvedMessage(message);

}

void WebRecver::onSslErrors(const QList<QSslError> &errors)
{
    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    foreach(QSslError error, errors){
        qDebug() << error.errorString();
    }
    m_socket.ignoreSslErrors();
}
