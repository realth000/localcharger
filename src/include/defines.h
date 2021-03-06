#ifndef DEFINES_H
#define DEFINES_H
#include <QtCore/QtGlobal>
#include <QtCore/QUrl>

#define TO_STR_(I1) #I1
#define TO_STR(I2) TO_STR_(I2)

#define APP_NAME "LocalCharger"
#define APP_CONFIGFILE_NAME "config.ini"
#define APP_CONFIGFILE_WEBSOCKET_SENDER_IP_PATH   "/WebSocket/SenderIP"
#define APP_CONFIGFILE_WEBSOCKET_SENDER_PORT_PATH "/WebSocket/SenderPort"
#define APP_CONFIGFILE_WEBSOCKET_RECVER_PORT_PATH "/WebSocket/RecverPort"
#define APP_CONFIGFILE_WEBSOCKET_RECVER_FILE_SAVE_PATH "/WebSocket/RecverSaveFilePath"
#define APP_CONFIGFILE_CLIENT_READABLENAME_PATH "/Client/ReadableName"
#define APP_CONFIGFILE_CLINET_AUTOCONNECT_PATH "/Client/AutoConnect"

#define APP_CONFIG_SAVE_PATH_ANDROID "/data/data/com.th000.localcharger/files/config.ini"

#define TITLEBAR_TITLETEXT "LocalCharger"
#define TITLEBAR_TITLEICON ":/pic/LocalCharger.ico"
#define TITLEBAR_CLOSEICON ":/pic/close.png"

#define LINEEDIT_PROPERTY_TEXTVALID_NAME    "textValidity"
#define LINEEDIT_PROPERTY_TEXTVALID_VALID   "valid"
#define LINEEDIT_PROPERTY_TEXTVALID_INVALID "invalid"
#define MSG_SEND_TYPE_HTML 0
#define RANDOM_STRING "12TocJn%BFde6Ng}0fGSY5s34H-PIwWEhi+#x)DuvptklabZUKq8z9jQmM$VA{R7C[X(rLOy"

#define WEBSOCKET_SENDER_IP_DEFAULT ""
#define WEBSOCKET_PORT_DEFAULT 3080
#define WEBSOCKET_SENDER_PORT_DEFAULT WEBSOCKET_PORT_DEFAULT
#define WEBSOCKET_RECVER_URL_DEFAULT "wss://" WEBSOCKET_SENDER_IP_DEFAULT ":" TO_STR(WEBSOCKET_SENDER_PORT_DEFAULT)
#define WEBSOCKET_RECVER_PORT_DEFAULT WEBSOCKET_PORT_DEFAULT

#define WEBSOCKET_CONFIG_SENDER_SOCKET_NAME APP_NAME "Server"
#define WEBSOCKET_CONFIG_CERTFILT_PATH               ":/config/localhost.cert"
#define WEBSOCKET_CONFIG_KEYFILE_PATH                ":/config/localhost.key"

#define WEBSOCKET_RECVER_FILE_SAVE_PATH_ANDROID "/storage/emulated/0/Download/LocalCharger/"

#define WEBSOCKET_FILEINFO_FILENAME_NAME        "fileName"
#define WEBSOCKET_FILEINFO_FILESIZE_NAME        "fileSize"
#define WEBSOCKET_FILEINFO_FILECHKSUM_NAME      "fileChkSum"
#define WEBSOCKET_FILEINFO_FILEID_NAME          "fileID"
#define WEBSOCKET_FILEINFO_FILEID_LENGTH        32
#define WEBSOCKET_FILEINFO_FILEFRAME_COUNT_NAME "fileFrameCount"
#define WEBSOCKET_FILEFRAME_FRAME_LENGTH   1048576 // 1024*1024*1=1MB
#define WEBSOCKET_FILEFRAME_ID_LENGTH      8

#define WEBSOCKET_FILEINFO_ARRAYLENGTH_LENGTH  8
#define WEBSOCKET_MESSAGETYPE_LENGTH          10

#define VALIDATOR_TYPE_IP_EXPRESSION "^((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))|localhost$"
#define VALIDATOR_TYPE_PORT_MIN      0
#define VALIDATOR_TYPE_PORT_MAX      65535

#define MSGSEND_TEXTEDIT_SENDING_HEAD_COLOR "#3366ff"
#define MSGSEND_TEXTEDIT_SENDING_TAIL_COLOR "#ffffcc"
#define MSGSEND_TEXTEDIT_SENDED_HEAD_COLOR  "#009966"
#define MSGSEND_TEXTEDIT_SENDED_TAIL_COLOR  "#ffffcc"

#define MSGRECV_TEXTEDIT_RECVING_HEAD_COLOR "#3366ff"
#define MSGRECV_TEXTEDIT_RECVING_TAIL_COLOR "#ffffcc"
#define MSGRECV_TEXTEDIT_RECVED_HEAD_COLOR  "#009966"
#define MSGRECV_TEXTEDIT_RECVED_TAIL_COLOR  "#ffffcc"

#if defined(Q_OS_WINDOWS) || defined(Q_OS_WIN)
#define NATIVE_PATH_SEP "\\"
#else
#define NATIVE_PATH_SEP "/"
#endif

using url_t = QUrl;
using port_t = quint16;
using dir_lists = QVector<QString>;
struct WebSocketFileInfo{
    explicit WebSocketFileInfo(){};
    explicit WebSocketFileInfo(QString fileName, qint64 fileSize, QString fileChkSum, QString fileID, qint64 fileFrameCount):
          m_fileName(fileName),
          m_fileSize(fileSize),
          m_fileChkSum(fileChkSum),
          m_fileID(fileID),m_fileFrameCount(fileFrameCount){};
    QString m_fileName;
    qint64 m_fileSize;
    QString m_fileChkSum = "";
    QString m_fileID;
    qint64 m_fileFrameCount;
};

/*
 * should not smaller than 0x3B9ACA00 to keep a 10-bytes length in decimal
 * SingleFile:
 *    S i n  g l e F
 *   5 1 17 8 1 E F
 *
 * TODO: Message should mention the start of whole progress first,
 * whether send file or dir.
 * NOTE: The changes above may be incompatible
 */
enum  WebSocketBinaryMessageType{
    SingleFile = 0x511781EF,                // Send a file this time(maybe part of dir)
    SingleFileWithPath = SingleFile + 1,    // Used in sending dir
    MakeDir = SingleFile + 10,              // Mkdir before send dir
//    SingleFileOnly = SingleFile + 15        // The file is whole progress
    StartProgress = SingleFile + 100         // Notify the start of progress
};

enum class SenderState{
    Disconnected = 0,
    Listening,
    Connected
};
enum class RecverState{
    Disconnected = 0,
    Connecting,
    Connected
};

// Daemon settings
#define DAEMON_SERVICE_NAME "th000.localcharger.daemon"
#define DAEMON_SERVICE_PATH "/th000/localcharger/daemon"
#define DAEMON_METHOD_GET_STATUS        "getStatus"
#define DAEMON_METHOD_GET_SENDER_STATUS "getSenderStatus"
#define DAEMON_METHOD_GET_RECVER_STATUS "getRecverStatus"
#define DAEMON_METHOD_CONNECT_REMOTE    "connectRemote"
#define DAEMON_METHOD_EXIT_DAEMON       "exitDaemon"
#define DAEMON_METHOS_SEND_MESSAGE      "sendMessage"
#define DAEMON_METHOD_GET_SENDER_STATUS_CODE "getSenderStatusCode"
#define DAEMON_METHOD_GET_RECVER_STATUS_CODE "getRecverStatusCode"
#define DAEMON_METHOD_SEND_FILE "sendFile"
#define DAEMON_METHOD_SEND_DIR "sendDir"

// Cli settings
#define CLI_SERVICE_NAME "th000.localcharger.cli"
#define CLI_SERVICE_PATH "/th000/localcharger/cli"
#define CLI_METHOD_UPDATE_SEND_FILE_PROGRESS   "updateSendProgress"
#define CLI_METHOD_EXIT_CLI  "exitCli"

enum class AppLanguage{
    En = 0,
    Zh_cn
};

// FIXME: Can not use timer
#define DISABLE_UPDATE_PROGRESS_BY_TIMER

enum class MessageBoxExMessageLevel : int
{
    Critical = 6,
    Warning,
    Information
};

using MBoxLevel = MessageBoxExMessageLevel;

#endif // DEFINES_H
