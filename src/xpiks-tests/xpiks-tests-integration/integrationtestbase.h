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

#if defined(APPVEYOR)
#define TESTS_FILES_PREFIX ""
#else
#define TESTS_FILES_PREFIX ""
#endif

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
    Commands::CommandManager *m_CommandManager;
};

#endif // INTEGRATIONTESTBASE

