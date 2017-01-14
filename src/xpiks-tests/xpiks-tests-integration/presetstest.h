#ifndef PRESETTESTS_H
#define PRESETTESTS_H

#include "integrationtestbase.h"

class PresetsTest: public IntegrationTestBase
{
public:
    PresetsTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // PRESETTESTS_H
