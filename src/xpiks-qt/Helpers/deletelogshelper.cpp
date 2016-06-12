#include "deletelogshelper.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QtConcurrent>
#include <QVector>
#include <algorithm>
#include "../Helpers/logger.h"
#include "../Common/defines.h"

namespace Helpers {
    void cleanLogsLogic(const QString &logsDir);
    QDateTime getDateFromName(const QString &name);
    void deleteLogsFilesFromList(const QVector<FileInfoHolder> &files);
    void deleteLogFile(const QString &fileNameFull);
    qint64 findLogFiles(const QString &logsDir, QVector<FileInfoHolder> &logFiles);

    bool operator <(const FileInfoHolder &arg1, const FileInfoHolder &arg2) {
        return arg1.m_AgeDays < arg2.m_AgeDays ||
                ((arg1.m_AgeDays == arg2.m_AgeDays) && (arg1.m_SizeBytes < arg2.m_SizeBytes));
    }

    void performCleanLogs() {
#ifdef WITH_LOGS
        QString appDataPath = XPIKS_USERDATA_PATH;
        QString logFileDir = QDir::cleanPath(appDataPath + QDir::separator() + "logs");
        QtConcurrent::run(cleanLogsLogic, logFileDir);
#endif
    }

    void cleanLogsLogic(const QString &logsDir) {
        QVector<FileInfoHolder> logFiles;

        qint64 overallSizeBytes = findLogFiles(logsDir, logFiles);
        std::sort(logFiles.begin(), logFiles.end());

        QVector<FileInfoHolder> filesToDelete;
        getFilesToDelete(logFiles, overallSizeBytes, filesToDelete);

        deleteLogsFilesFromList(filesToDelete);
    }

    void getFilesToDelete(const QVector<FileInfoHolder> &logFiles, qint64 overallSizeBytes,
                          QVector<FileInfoHolder> &filesToDelete) {
        int size = logFiles.size();

        for (int i = size - 1; i >= 0; --i) {
            const FileInfoHolder &info = logFiles.at(i);

            bool shouldDelete = false;
            shouldDelete = shouldDelete || (i >= MAX_LOGS_NUMBER);
            shouldDelete = shouldDelete || (info.m_AgeDays >= MAX_LOGS_AGE_DAYS);
            shouldDelete = shouldDelete || (overallSizeBytes >= MAX_LOGS_SIZE_BYTES);

            if (shouldDelete) {
                LOG_DEBUG << "Log file" << info.m_Filepath << "marked for deletion";
                filesToDelete.append(info);
                overallSizeBytes -= info.m_SizeBytes;
                Q_ASSERT(overallSizeBytes >= 0);
            } else {
                break;
            }
        }
    }

    qint64 findLogFiles(const QString &logsDir, QVector<FileInfoHolder> &logFiles) {
        Helpers::Logger &logger = Helpers::Logger::getInstance();
        QString logFilePath = logger.getLogFilePath();
        QDirIterator it(logsDir, QStringList() << "xpiks-qt-*.log", QDir::Files);
        QDateTime currentTime = QDateTime::currentDateTime();
        qint64 logsSizeBytes = 0;

        while (it.hasNext()) {
            QString fileNameFull = it.next();

            if (fileNameFull == logFilePath) {
                continue;
            }

            QFileInfo fileInfo(fileNameFull);
            qint64 fileSize = fileInfo.size();
            QString fileName = fileInfo.fileName();

            logsSizeBytes += fileSize;
            QDateTime createTime = getDateFromName(fileName);
            int deltaTimeDays = createTime.daysTo(currentTime);

            logFiles.append({
                             fileNameFull, // m_FilePath
                             fileSize, // m_SizeBytes
                             deltaTimeDays, // m_AgeDays
                         });
        }

        return logsSizeBytes;
    }

    void deleteLogsFilesFromList(const QVector<FileInfoHolder> &files) {
        LOG_INFO << files.size() << "logs to delete";
        int size = files.size();

        for (int i = 0; i < size; i++) {
            deleteLogFile(files[i].m_Filepath);
        }
    }

    void deleteLogFile(const QString &fileNameFull) {
        QFile file(fileNameFull);

        if (file.exists()) {
            LOG_DEBUG << "Removing log file " << fileNameFull;
            bool removed = false;

            try {
                removed = file.remove();
            } catch(...) {
                removed = false;
            }

            if (!removed) {
                LOG_WARNING << "Failed to delete file " << fileNameFull;
            }
        }
    }

    QDateTime getDateFromName(const QString &name) {
        QString createDateStr = name.mid(9, 8);
        return QDateTime::fromString(createDateStr, "ddMMyyyy");
    }
}
