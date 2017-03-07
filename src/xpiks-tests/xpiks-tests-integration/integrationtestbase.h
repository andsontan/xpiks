#ifndef INTEGRATIONTESTBASE
#define INTEGRATIONTESTBASE

#include <QString>
#include <QDebug>
#include <QDir>
#include "../../xpiks-qt/Commands/commandmanager.h"

#define VERIFY(condition, message) \
    if (!(condition)) {\
    qCritical(message);\
    return 1;\
    }

class IntegrationTestBase {
public:
    IntegrationTestBase(Commands::CommandManager *commandManager):
        m_CommandManager(commandManager)
    {}
    virtual ~IntegrationTestBase() {}

    virtual QString testName() = 0;
    virtual void setup() = 0;
    virtual int doTest() = 0;
    virtual void teardown() { m_CommandManager->cleanup(); }

protected:
    QUrl getFilePathForTest(const QString &prefix) {
        QFileInfo fi(prefix);
        int tries = 6;
        QStringList parents;
        while (tries--) {
            if (!fi.exists()) {
                parents.append("..");
                fi.setFile(parents.join('/') + "/" + prefix);
            } else {
                return QUrl::fromLocalFile(fi.absoluteFilePath());
            }
        }

        return QUrl::fromLocalFile(QFileInfo(prefix).absoluteFilePath());
    }

    QUrl getDirPathForTest(const QString &prefix) {
        QString path = prefix;
        if (path.startsWith('/')) { path.remove(0, 1); }
        QDir dir(path);
        int tries = 6;
        while (tries--) {
            if (!dir.exists()) {
                path = "../" + prefix;
                dir.setPath(path);
            } else {
                return QUrl::fromLocalFile(dir.absolutePath());
            }
        }

        return QUrl::fromLocalFile(QDir(prefix).absolutePath());
    }

protected:
    Commands::CommandManager *m_CommandManager;
};

#endif // INTEGRATIONTESTBASE

