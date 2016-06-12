#ifndef DELETELOGSHELPER_H
#define DELETELOGSHELPER_H

#include <QString>

#define criticalLogsSizeMB 10
#define criticalLogsSizeB criticalLogsSizeMB * 1024 * 1024
#define critialLogsAgeDays 60
#define criticalLogsNumber 100

namespace Helpers {
    struct FileInfoHolder {
        int days;
        int size;
        QString name;
    };

    void performCleanLogs();

    QVector<FileInfoHolder> getFilesToDelete(const QVector<FileInfoHolder> &files, int sizeFolderB);
    bool operator <(const FileInfoHolder &arg1, const FileInfoHolder &arg2);
}
#endif // DELETELOGSHELPER_H
