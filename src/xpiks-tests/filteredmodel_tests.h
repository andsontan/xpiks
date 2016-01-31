#ifndef FILTEREDMODELTESTS_H
#define FILTEREDMODELTESTS_H


#include <QtTest/QTest>
#include "Mocks/artitemsmodelmock.h"
#include "Mocks/commandmanagermock.h"
#include "../xpiks-qt/Models/filteredartitemsproxymodel.h"

class FilteredModelTests : public QObject
{
    Q_OBJECT
public:
    void invertSelectionTest();

signals:

public slots:
};

#endif // FILTEREDMODELTESTS_H
