#include "filehelper.h"
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QtDebug>

#ifndef NATIVE_SEPARATOR
#if defined(Q_OS_WINDWOS) || defined(Q_OS_WIN)
#define NATIVE_SEPARATOR "\\"
#else
#define NATIVE_SEPARATOR "/"
#endif // if defined(Q_OS_WINDWOS)
#endif // ifndef NATIVE_SEPARATOR

bool FileHelper::checkDirectoryInfo(const QString &dirPath, qint64 &fileCount, qint64 &totalSize)
{
    if(dirPath.isEmpty()){
        qInfo() << "Empty directory path:" << dirPath;
        return false;
    }
    if(!QFileInfo::exists(dirPath)){
        qInfo() << "Directory path not exists:" << dirPath;
        return false;
    }
    if(!QFileInfo(dirPath).isDir()){
        qInfo() << "Path is not a directory:" << dirPath;
        return false;
    }
    fileCount = 0;
    totalSize = 0;
    QDirIterator it(dirPath, QDir::Files | QDir::Dirs | QDir::Hidden);
    while(it.hasNext()){
        totalSize += it.fileInfo().size();
        if(it.fileInfo().isFile()){
            fileCount++;
        }
        it.next();
    }
    totalSize += it.fileInfo().size();
    if(it.fileInfo().isFile()){
        fileCount++;
    }
    return true;
}
