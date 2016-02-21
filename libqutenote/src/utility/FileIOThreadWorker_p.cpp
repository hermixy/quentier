#include "FileIOThreadWorker_p.h"
#include <qute_note/utility/Utility.h>
#include <qute_note/logging/QuteNoteLogger.h>
#include <QFile>
#include <QTimerEvent>

namespace qute_note {

FileIOThreadWorkerPrivate::FileIOThreadWorkerPrivate(QObject * parent) :
    QObject(parent),
    m_idleTimePeriodSeconds(30),
    m_postOperationTimerId(0)
{}

void FileIOThreadWorkerPrivate::setIdleTimePeriod(const qint32 seconds)
{
    QNDEBUG("FileIOThreadWorkerPrivate::setIdleTimePeriod: seconds = " << seconds);
    m_idleTimePeriodSeconds = seconds;
}

#define RESTART_TIMER() \
    if (m_postOperationTimerId != 0) { \
        killTimer(m_postOperationTimerId); \
    } \
    m_postOperationTimerId = startTimer(SEC_TO_MSEC(m_idleTimePeriodSeconds)); \
    QNTRACE("FileIOThreadWorkerPrivate: started timer with id " << m_postOperationTimerId)

void FileIOThreadWorkerPrivate::onWriteFileRequest(QString absoluteFilePath, QByteArray data,
                                                   QUuid requestId, bool append)
{
    QNDEBUG("FileIOThreadWorkerPrivate::onWriteFileRequest: file path = " << absoluteFilePath
            << ", request id = " << requestId << ", append = " << (append ? "true" : "false"));

    QFileInfo fileInfo(absoluteFilePath);
    QDir folder = fileInfo.absoluteDir();
    if (!folder.exists())
    {
        bool madeFolder = folder.mkpath(folder.absolutePath());
        if (!madeFolder) {
            QString error = QT_TR_NOOP("Can't create folder to write file into: ") + absoluteFilePath;
            QNDEBUG(error);
            emit writeFileRequestProcessed(false, error, requestId);
            RESTART_TIMER();
            return;
        }
    }

    QFile file(absoluteFilePath);

    QIODevice::OpenMode mode;
    if (append) {
        mode = QIODevice::Append;
    }
    else {
        mode = QIODevice::WriteOnly;
    }

    bool open = file.open(mode);
    if (!open) {
        QString error = QT_TR_NOOP("Can't open file: ") + absoluteFilePath;
        QNDEBUG(error);
        emit writeFileRequestProcessed(false, error, requestId);
        RESTART_TIMER();
        return;
    }

    qint64 writtenBytes = file.write(data);
    if (writtenBytes < data.size()) {
        QString error = QT_TR_NOOP("Can't write the whole data: ") + absoluteFilePath;
        QNDEBUG(error);
        emit writeFileRequestProcessed(false, error, requestId);
        RESTART_TIMER();
        return;
    }

    file.close();
    QNDEBUG("Successfully wrote the file " + absoluteFilePath);
    emit writeFileRequestProcessed(true, QString(), requestId);
    RESTART_TIMER();
}

void FileIOThreadWorkerPrivate::onReadFileRequest(QString absoluteFilePath, QUuid requestId)
{
    QNDEBUG("FileIOThreadWorkerPrivate::onReadFileRequest: file path = " << absoluteFilePath
            << ", request id = " << requestId);

    QFile file(absoluteFilePath);
    if (!file.exists()) {
        QNTRACE("The file to read does not exist, sending empty data in return");
        emit readFileRequestProcessed(true, QString(), QByteArray(), requestId);
        RESTART_TIMER();
        return;
    }

    bool open = file.open(QIODevice::ReadOnly);
    if (!open) {
        emit readFileRequestProcessed(false, QT_TR_NOOP("Can't open file for reading: ") + absoluteFilePath,
                                      QByteArray(), requestId);
        RESTART_TIMER();
        return;
    }

    QByteArray data = file.readAll();
    emit readFileRequestProcessed(true, QString(), data, requestId);
    RESTART_TIMER();
}

void FileIOThreadWorkerPrivate::timerEvent(QTimerEvent * pEvent)
{
    if (!pEvent) {
        QNWARNING("Detected null pointer to QTimerEvent in FileIOThreadWorkerPrivate");
        return;
    }

    qint32 timerId = pEvent->timerId();

    if (timerId != m_postOperationTimerId) {
        QNTRACE("Received unidentified timer event for FileIOThreadWorkerPrivate");
        return;
    }

    killTimer(timerId);
    m_postOperationTimerId = 0;

    emit readyForIO();
}

} // namespace qute_note
