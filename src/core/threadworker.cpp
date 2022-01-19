#include "threadworker.h"
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QThread>

QSemaphore ThreadWorker::fileSemaphore(1);

ThreadWorker::ThreadWorker(const QByteArray &fileData, const QString &filePath, const int &fileFrameID) :
      QObject(nullptr),
      m_fileData(fileData),
      m_filePath(filePath),
      m_fileFrameID(fileFrameID)
{

}

void ThreadWorker::saveFileFrameToDisk()
{
    if(m_fileData.length() <= 0){
        qInfo() << m_filePath << "empty file data";
        emit saveFileFinish(0);
        return;
    }
    fileSemaphore.acquire(m_fileFrameID + 1);
    // in
    QFile file(m_filePath);
    if(m_fileFrameID == 0 && file.exists()){
        file.remove();
    }
    if(!file.open(QIODevice::Append)){
        qInfo() << m_filePath << "write failed, device not open";
        emit saveFileFinish(0);
        return;
    }
    const qint64 writeBytes = file.write(m_fileData);
    file.close();
    // out
    fileSemaphore.release(m_fileFrameID + 2);
    emit saveFileFinish(writeBytes);
}

// reset semaphore before send every file
void ThreadWorker::resetSemaphore()
{
    fileSemaphore.acquire(fileSemaphore.available() - 1);
}
