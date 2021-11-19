#ifndef THREADWORKER_H
#define THREADWORKER_H
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QSemaphore>

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker(const QByteArray &fileData, const QString &filePath, const int &fileFrameID);
    QMutex m_fileMutex;
    static void resetSemaphore();
    static QSemaphore fileSemaphore;

public slots:
    void saveFileFrameToDisk();

signals:
    void saveFileFinish(qint64 writeBytes);

private:
    const QByteArray m_fileData;
    const QString m_filePath;
    const int m_fileFrameID;
};

#endif // THREADWORKER_H
