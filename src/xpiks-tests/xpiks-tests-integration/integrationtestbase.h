#ifndef INTEGRATIONTESTBASE
#define INTEGRATIONTESTBASE

#include <QString>
#include <QDebug>

namespace Commands {
    class CommandManager;
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
    virtual void teardown() = 0;

    void VERIFY(bool condition, const QString &message) {
        if (!condition) {
            qCritical("Test %s FAILED with message %s",
                      testName().toStdString().c_str(),
                      message.toStdString().c_str());
        }
    }

protected:
    Commands::CommandManager *m_CommandManager;
};

#endif // INTEGRATIONTESTBASE

