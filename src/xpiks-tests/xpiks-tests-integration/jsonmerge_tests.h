#ifndef JSONMERGE_TESTS_H
#define JSONMERGE_TESTS_H

#include "integrationtestbase.h"
#include <QJsonObject>
#include <QJsonValue>
#include "../../xpiks-qt/Helpers/comparevaluesjson.h"


class JsonMergeTests:
    public Helpers::CompareValuesJson,
    public IntegrationTestBase
{
public:
    JsonMergeTests(Commands::CommandManager *commandManager):
        IntegrationTestBase(commandManager)
    {}

protected:
    virtual int operator()(const QJsonObject &val1, const QJsonObject &val2) {
        return (val1["name"] == val2["name"]) ? 0 : -1;
    }

public:
    virtual QString testName();
    virtual void setup();
    virtual int doTest();

private:
    int mergeTwoFilesObjects();
    int mergeTwoFilesStrings();
};

#endif // JSONMERGE_TESTS_H
