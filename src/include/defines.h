#ifndef DEFINES_H
#define DEFINES_H
#include <QtCore/QtGlobal>
#include <QtCore/QUrl>

#define APP_NAME "LocalCharger"

#define TITLEBAR_TITLETEXT "LocalCharger"
#define TITLEBAR_TITLEICON ":/pic/LocalCharger.ico"
#define TITLEBAR_CLOSEICON ":/pic/close.png"

#define MSG_SEND_TYPE_HTML 0

#define WEBSOCKET_SENDER_PORT_DEFAULT 1234
#define WEBSOCKET_RECVER_URL_DEFAULT "wss://localhost:1234"

#define WEBSOCKET_CONFIG_SENDER_SOCKET_NAME APP_NAME "Server"
#define WEBSOCKET_CONFIG_CERTFILT_PATH ":/config/localhost.cert"
#define WEBSOCKET_CONFIG_KEYFILE_PATH ":/config/localhost.key"

#define VALIDATOR_TYPE_IP_EXPRESSION "^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$"
#define VALIDATOR_TYPE_PORT_MIN 0
#define VALIDATOR_TYPE_PORT_MAX 65535

using url_t = QUrl;
using port_t = quint16;



#endif // DEFINES_H
