#ifndef __LIB_QUTE_NOTE__UTILITY__FILE_IO_THREAD_WORKER_P_H
#define __LIB_QUTE_NOTE__UTILITY__FILE_IO_THREAD_WORKER_P_H

#include <qute_note/utility/Qt4Helper.h>
#include <QObject>
#include <QString>
#include <QUuid>
#include <QIODevice>

namespace qute_note {

class FileIOThreadWorkerPrivate: public QObject
{
    Q_OBJECT
public:
    explicit FileIOThreadWorkerPrivate(QObject * parent = Q_NULLPTR);

    void setIdleTimePeriod(const qint32 seconds);

Q_SIGNALS:
    void readyForIO();
    void writeFileRequestProcessed(bool success, QString errorDescription, QUuid requestId);
    void readFileRequestProcessed(bool success, QString errorDescription, QByteArray data, QUuid requestId);

public Q_SLOTS:
    void onWriteFileRequest(QString absoluteFilePath, QByteArray data,
                            QUuid requestId, bool append);

    void onReadFileRequest(QString absoluteFilePath, QUuid requestId);

private:
    virtual void timerEvent(QTimerEvent * pEvent);

private:
    qint32  m_idleTimePeriodSeconds;
    qint32  m_postOperationTimerId;
};

} // namespace qute_note

#endif // __LIB_QUTE_NOTE__UTILITY__FILE_IO_THREAD_WORKER_P_H
