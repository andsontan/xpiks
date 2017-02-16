#ifndef USERDICTEDITTEST_H
#define USERDICTEDITTEST_H

#include "integrationtestbase.h"

class UserDictEditTest : public IntegrationTestBase
{
public:
    UserDictEditTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // USERDICTEDITTEST_H
