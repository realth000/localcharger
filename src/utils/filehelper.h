#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QtCore/QString>

class FileHelper
{
public:
    static bool checkDirectoryInfo(const QString &dirPath, qint64 &fileCount, qint64 &totalSize);
};

#endif // FILEHELPER_H
