#include "jsonparser.h"

JsonParser::JsonParser()
{

}

QByteArray JsonParser::genFileInfoFromString(const WebSocketFileInfo &fileInfo)
{
    QJsonDocument jsonDoc;
    QJsonObject jsonObj;
    jsonObj.insert(QStringLiteral(WEBSOCKET_FILEINFO_FILENAME_NAME), fileInfo.m_fileName);
    jsonObj.insert(QStringLiteral(WEBSOCKET_FILEINFO_FILESIZE_NAME), fileInfo.m_fileSize);
    jsonObj.insert(QStringLiteral(WEBSOCKET_FILEINFO_FILECHKSUM_NAME), fileInfo.m_fileChkSum);
    jsonObj.insert(QStringLiteral(WEBSOCKET_FILEINFO_FILEID_NAME), fileInfo.m_fileID);
    jsonObj.insert(QStringLiteral(WEBSOCKET_FILEINFO_FILEFRAME_COUNT_NAME), fileInfo.m_fileFrameCount);
    jsonDoc.setObject(jsonObj);
    return jsonDoc.toJson();
}

WebSocketFileInfo JsonParser::parseFileInfoFromArray(const QByteArray &fileInfoArray)
{
    if(fileInfoArray.length() <= 0){
        return WebSocketFileInfo();
    }

    QJsonParseError jsonErr;
    const QJsonDocument jsonDoc = jsonDoc.fromJson(fileInfoArray, &jsonErr);
    if(jsonDoc.isNull() || jsonErr.error != QJsonParseError::NoError){
        qInfo() << "json parse error when parsing file info from array:" << jsonErr.errorString();
        return WebSocketFileInfo();
    }
    const QJsonObject jsonObj = jsonDoc.object();
    // qlonglong == qint64
    return WebSocketFileInfo(jsonObj.value(QStringLiteral(WEBSOCKET_FILEINFO_FILENAME_NAME)).toString(),
                             jsonObj.value(QStringLiteral(WEBSOCKET_FILEINFO_FILESIZE_NAME)).toVariant().toLongLong(),
                             jsonObj.value(QStringLiteral(WEBSOCKET_FILEINFO_FILECHKSUM_NAME)).toString(),
                             jsonObj.value(QStringLiteral(WEBSOCKET_FILEINFO_FILEID_NAME)).toString(),
                             jsonObj.value(QStringLiteral(WEBSOCKET_FILEINFO_FILEFRAME_COUNT_NAME)).toVariant().toLongLong());
}

QByteArray JsonParser::genDirListsFromVector(const dir_lists &dirs)
{
    QJsonDocument JsonDoc;
    QJsonObject jsonObj;
    qint64 key = 0;
    for(const QString &dir: dirs){
        jsonObj.insert(QString::number(key), dir);
        key++;
    }
    JsonDoc.setObject(jsonObj);
    return JsonDoc.toJson();
}

dir_lists JsonParser::parseDirListsFromArray(const QByteArray &dirListsArray)
{
    dir_lists d;
    if(dirListsArray.length() <= 0){
        qInfo() << "empty dir lists(length <= 0)";
        return d;
    }
    QJsonParseError jsonErr;
    const QJsonDocument jsonDoc = jsonDoc.fromJson(dirListsArray, &jsonErr);
    if(jsonDoc.isNull() || jsonErr.error != QJsonParseError::NoError){
        qInfo() << "json parse error when parsing dir lists from array:" << jsonErr.errorString();
        return d;
    }
    const QJsonObject obj = jsonDoc.object();
    QJsonObject::const_iterator it = obj.constBegin();

    while(it != obj.constEnd()){
        d.append(it.value().toString());
        it++;
    }
    return d;
}
