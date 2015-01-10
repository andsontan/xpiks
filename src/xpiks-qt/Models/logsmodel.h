#ifndef LOGSMODEL
#define LOGSMODEL

#include <QString>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QStandardPaths>
#include "Helpers/constants.h"

namespace Models {
    class LogsModel : public QObject {
        Q_OBJECT
    public:
        Q_INVOKABLE QString getAllLogsText() {
            QString result;
#ifdef QT_NO_DEBUG
            QDir logFileDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
            QString logFilePath = logFileDir.filePath(Constants::LOG_FILENAME);
            QFile f(logFilePath);


            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&f);
                result = in.readAll();
            }
#else
            result = "Logs are available in Release version";
#endif
            return result;

        }
    };
}

#endif // LOGSMODEL

