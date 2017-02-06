#ifndef TRANSLATORBASICTEST_H
#define TRANSLATORBASICTEST_H

#include "integrationtestbase.h"

class TranslatorBasicTest : public IntegrationTestBase
{
public:
    TranslatorBasicTest(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

    // IntegrationTestBase interface
public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();
};

#endif // TRANSLATORBASICTEST_H
