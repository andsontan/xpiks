#ifndef FINDANDREPLACEMODELTEST_H
#define FINDANDREPLACEMODELTEST_H

#include "integrationtestbase.h"

class FindAndReplaceModelTest : public IntegrationTestBase
{
public:
    FindAndReplaceModelTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // FINDANDREPLACEMODELTEST_H
