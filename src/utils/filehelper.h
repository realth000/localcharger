#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QtCore/QString>
#include <QtCore/QVector>
#include "defines.h"

class FileHelper
{
public:
    static bool checkDirectoryInfo(const QString &dirPath, qint64 &fileCount, qint64 &totalSize, dir_lists &dirVector);
};

#endif // FILEHELPER_H
