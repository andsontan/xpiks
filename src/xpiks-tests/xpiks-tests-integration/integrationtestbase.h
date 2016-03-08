#ifndef INTEGRATIONTESTBASE
#define INTEGRATIONTESTBASE

namespace Commands {
    class CommandManager;
}

class IntegrationTestBase {
public:
    IntegrationTestBase(Commands::CommandManager *commandManager):
        m_CommandManager(commandManager)
    {}
    virtual ~IntegrationTestBase() {}

    virtual void setup() = 0;
    virtual void doTest() = 0;
    virtual void teardown() = 0;

protected:
    Commands::CommandManager *m_CommandManager;
};

#endif // INTEGRATIONTESTBASE

