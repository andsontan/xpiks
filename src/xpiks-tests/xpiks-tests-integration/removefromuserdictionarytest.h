#ifndef REMOVEFROMUSERDICTIONARYTEST_H
#define REMOVEFROMUSERDICTIONARYTEST_H

#include "integrationtestbase.h"

class RemoveFromUserDictionaryTest:
    public IntegrationTestBase
{
public:
    RemoveFromUserDictionaryTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface

public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // REMOVEFROMUSERDICTIONARYTEST_H
