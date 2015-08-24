#ifndef LOGGER_H
#define LOGGER_H

#include <QStringList>
#include <QString>
#include <QMutexLocker>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>

namespace Helpers {
    class Logger
    {
    public:
        static Logger& getInstance()
        {
            static Logger instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
            return instance;
        }

    public:
        void setLogFilePath(const QString &filepath) {
            m_LogFilepath = filepath;
        }

        void log(const QString &message) {
            QMutexLocker locker(&m_Mutex);
            m_LogsStorage[m_ActiveIndex].append(message);
        }

        void flush() {
            int previousIndex = m_ActiveIndex;

            m_Mutex.lock();
            {
                m_ActiveIndex = (m_ActiveIndex + 1) % 2;
            }
            m_Mutex.unlock();

            flushStream(previousIndex);
        }

        void clear() {
            QMutexLocker locker(&m_Mutex);

            m_LogsStorage[0].clear();
            m_LogsStorage[1].clear();

            QFile outFile(m_LogFilepath);
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream ts(&outFile);
                ts << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " - cleared log";
                endl(ts);
            }
        }

    private:
        void flushStream(int index) {
            QStringList &logItems = m_LogsStorage[index];

            if (logItems.length() > 0) {
                QFile outFile(m_LogFilepath);
                if (outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
                    QTextStream ts(&outFile);

                    foreach (const QString &logItem, logItems) {
                        ts << logItem;
                        endl(ts);
                    }
                }
            }
        }

    private:
        Logger() {}
        Logger(Logger const&);
        void operator=(Logger const&);

    private:
        QString m_LogFilepath;
        QStringList m_LogsStorage[2];
        QMutex m_Mutex;
        int m_ActiveIndex;
    };

    class LogsManager : QObject {
        Q_OBJECT
    public:
        LogsManager(QObject *parent=0) :
            QObject(parent),
            m_msec(2000)
        {
            m_Timer = new QTimer(this);
            // TODO: refactor timer to wait for flush finish
            m_Timer->setSingleShot(false);
            connect(m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
        }

        ~LogsManager() { m_Timer->stop(); delete m_Timer; }

    public:
        void initLogger(const QString &path) {
            Logger &logger = Logger::getInstance();
            logger.setLogFilePath(path);
        }

        void clearLogs() {
            m_Timer->stop();

            Logger &logger = Logger::getInstance();
            logger.clear();

            m_Timer->start(m_msec);
        }

        void startFlushing() {
            m_Timer->start(m_msec);
        }

    public slots:
        void onTimer() {
            Logger &logger = Logger::getInstance();
            logger.flush();
        }

    private:
        QTimer *m_Timer;
        int m_msec;
    };
}

#endif // LOGGER_H
