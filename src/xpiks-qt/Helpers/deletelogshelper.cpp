#include "deletelogshelper.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QtConcurrent>
#include <QVector>
#include <QtAlgorithms>
#include "../Helpers/logger.h"
#include "../Common/defines.h"

namespace Helpers {
    void cleanLogsLogic(const QString &logsDir);
    QDateTime getDateFromName(const QString &name);
    void deleteLogsFilesFromList(const QVector<FileInfoHolder> &files);
    void deleteLogFile(const QString &fileNameFull);
    int initFileInfo(const QString &logsDir, QVector<FileInfoHolder> &files);

    bool operator <(const FileInfoHolder &arg1, const FileInfoHolder &arg2) {
        return arg1.days < arg2.days || ((arg1.days == arg2.days) && (arg1.size < arg2.size));
    }

    void performCleanLogs() {
#ifdef WITH_LOGS
        QString appDataPath = XPIKS_USERDATA_PATH;
        QString logFileDir = QDir::cleanPath(appDataPath + QDir::separator() + "logs");
        QtConcurrent::run(cleanLogsLogic, logFileDir);
#endif
    }

    void cleanLogsLogic(const QString &logsDir) {
        QVector<FileInfoHolder> files;
        QVector<FileInfoHolder> filesToDelete;
        int sizeFolderB = initFileInfo(logsDir, files);
        filesToDelete = getFilesToDelete(files, sizeFolderB);
    }

    QVector<FileInfoHolder> getFilesToDelete(const QVector<FileInfoHolder> &files, int sizeFolderB) {
        QVector<FileInfoHolder> filesToDelete;
        int N = files.size();
        int i = N - 1;

        while ((i >= criticalLogsNumber) || (sizeFolderB >= criticalLogsSizeB) ||
            ((i >= 0) && (files[i].days >= critialLogsAgeDays))) {
            sizeFolderB -= files[i].size;
            LOG_DEBUG << "log file " << files[i].name << "marked for deletion";
            filesToDelete.append(files[i]);
            i--;
        }

        return filesToDelete;
    }

    int initFileInfo(const QString &logsDir, QVector<FileInfoHolder> &files) {
        Helpers::Logger &logger = Helpers::Logger::getInstance();
        QString logFilePath = logger.getLogFilePath();
        QDirIterator it(logsDir, QStringList() << "xpiks-qt-*.log", QDir::Files);
        QDateTime currentTime = QDateTime::currentDateTime();
        int logsSizeB = 0;

        while (it.hasNext()) {
            QString fileNameFull = it.next();
            if (fileNameFull == logFilePath) {
                continue;
            }

            QFileInfo fileInfo(fileNameFull);
            int size = fileInfo.size();
            logsSizeB += size;
            QString fileName = fileInfo.fileName();
            QDateTime createTime = getDateFromName(fileName);
            int deltaTimeDays = createTime.daysTo(currentTime);
            FileInfoHolder temp;
            temp.days = deltaTimeDays;
            temp.size = size;
            temp.name = fileNameFull;
            files.append(temp);
        }

        qSort(files.begin(), files.end());

        return logsSizeB;
    }

    void deleteLogsFilesFromList(const QVector<FileInfoHolder> &files) {
        int size = files.size();

        for (int i = 0; i < size; i++) {
            deleteLogFile(files[i].name);
        }
    }

    void deleteLogFile(const QString &fileNameFull) {
        QFile file(fileNameFull);

        if (file.exists()) {
            LOG_DEBUG << "Removing log file " << fileNameFull;
            if (!file.remove()) {
                LOG_WARNING << "Failed to delete file " << fileNameFull;
            }
        }
    }

    QDateTime getDateFromName(const QString &name) {
        QString createDateStr = name.mid(9, 8);

        return QDateTime::fromString(createDateStr, "ddMMyyyy");
    }
}
