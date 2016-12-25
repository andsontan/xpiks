#ifndef FIXSPELLINGMARKSMODIFIEDTEST_H
#define FIXSPELLINGMARKSMODIFIEDTEST_H

#include "integrationtestbase.h"

class FixSpellingMarksModifiedTest : public IntegrationTestBase
{
public:
    FixSpellingMarksModifiedTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // FIXSPELLINGMARKSMODIFIEDTEST_H
