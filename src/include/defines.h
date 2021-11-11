#ifndef DEFINES_H
#define DEFINES_H
#include <QtCore/QtGlobal>
#include <QtCore/QUrl>

#define APP_NAME "LocalCharger"

#define MSG_SEND_TYPE_HTML 0

#define WEBSOCKET_SENDER_PORT_DEFAULT 1234
#define WEBSOCKET_RECVER_URL_DEFAULT "wss://localhost:1234"

#define WEBSOCKET_CONFIG_SENDER_SOCKET_NAME APP_NAME "Server"
#define WEBSOCKET_CONFIG_CERTFILT_PATH ":/config/localhost.cert"
#define WEBSOCKET_CONFIG_KEYFILE_PATH ":/config/localhost.key"

using url_t = QUrl;
using port_t = quint16;

#endif // DEFINES_H
