#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <QtCore/QString>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QtDebug>
#include "defines.h"

class JsonParser
{
public:
    JsonParser();
    static QByteArray genFileInfoFromString(const WebSocketFileInfo &fileInfo);
    static WebSocketFileInfo parseFileInfoFromArray(const QByteArray &fileInfoArray);
};

#endif // JSONPARSER_H
