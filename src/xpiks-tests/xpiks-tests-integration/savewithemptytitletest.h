#ifndef SAVEWITHEMPTYTITLETEST_H
#define SAVEWITHEMPTYTITLETEST_H

#include "integrationtestbase.h"

class SaveWithEmptyTitleTest : public IntegrationTestBase
{
public:
    SaveWithEmptyTitleTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // SAVEWITHEMPTYTITLETEST_H
