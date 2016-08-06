#ifndef INTEGRATIONTESTBASE
#define INTEGRATIONTESTBASE

#include <QString>
#include <QDebug>
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
    QUrl getImagePathForTest(const QString &prefix) {
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

protected:
    Commands::CommandManager *m_CommandManager;
};

#endif // INTEGRATIONTESTBASE

