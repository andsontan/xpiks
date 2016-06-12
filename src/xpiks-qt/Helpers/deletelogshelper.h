#ifndef DELETELOGSHELPER_H
#define DELETELOGSHELPER_H

#include <QString>
#include <QVector>

#define MAX_LOGS_SIZE_MB 10
#define MAX_LOGS_SIZE_BYTES (MAX_LOGS_SIZE_MB * 1024 * 1024)
#define MAX_LOGS_AGE_DAYS 60
#define MAX_LOGS_NUMBER 100

namespace Helpers {
    struct FileInfoHolder {
        QString m_Filepath;
        qint64 m_SizeBytes;
        int m_AgeDays;
    };

    void performCleanLogs();
    void getFilesToDelete(const QVector<FileInfoHolder> &logFiles, qint64 overallSizeBytes,
                          QVector<FileInfoHolder> &filesToDelete);
    bool operator <(const FileInfoHolder &arg1, const FileInfoHolder &arg2);
}
#endif // DELETELOGSHELPER_H
