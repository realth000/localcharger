#include "webrecver.h"

WebRecver::WebRecver(QObject *parent) : QObject(parent)
{
    connect(&m_socket, &QWebSocket::connected, this, &WebRecver::onConnected, Qt::UniqueConnection);
    connect(&m_socket, &QWebSocket::disconnected, this, &WebRecver::onDisconnected, Qt::UniqueConnection);
    connect(&m_socket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors), this, &WebRecver::onSslErrors, Qt::UniqueConnection);

}

WebRecver::WebRecver(const url_t &url, QObject *parent) : WebRecver(parent)
{
    openUrl(url);
}

WebRecver::~WebRecver()
{
    m_socket.close();
}

QAbstractSocket::SocketState WebRecver::getRecverState() const noexcept
{
    return m_socket.state();
}

url_t WebRecver::recverUrl() const noexcept
{
    return m_socket.requestUrl();
}

port_t WebRecver::recverPort() const noexcept
{
    return m_socket.localPort();
}

bool WebRecver::start(const url_t &url)
{
    if(!url.isValid()){
        qDebug() << "Recver start failed: invalid url" << url;
        return false;
    }
    openUrl(url);
    return true;
}

void WebRecver::sendMessage(const QString &msg)
{
    m_socket.sendTextMessage(msg);
}

void WebRecver::openUrl(const url_t &url)
{
    m_socket.open(url);
}

void WebRecver::onConnected()
{
    emit recverConnected();
    qDebug() << "WebSocket connected";
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &WebRecver::onTextMessageReceived);
}

void WebRecver::onDisconnected()
{
    emit recverDisconnected();
}

void WebRecver::onTextMessageReceived(QString message)
{
    qDebug() << "Message received:" << message;
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
