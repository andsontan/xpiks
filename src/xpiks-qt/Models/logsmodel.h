#ifndef LOGSMODEL
#define LOGSMODEL

#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QStandardPaths>
#include "Helpers/constants.h"

namespace Models {
    class LogsModel : public QObject {
        Q_OBJECT
    public:
        Q_INVOKABLE QString getAllLogsText() {
            QDir logFileDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
            QString logFilePath = logFileDir.filePath(Constants::LOG_FILENAME);
            QFile f(logFilePath);

            QString result;
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&f);
                result = in.readAll();
            }

            return result;
        }
    };
}

#endif // LOGSMODEL

