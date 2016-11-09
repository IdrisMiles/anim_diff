#include <QtTest/QtTest>

#include "revisionUtils.h"
#include "revisionNode.h"

#include <memory>

class UnitTests: public QObject
{

public:
    UnitTests(){}
    virtual ~UnitTests(){}

private:

    void TestDiff()
    {
        // testing the testing stuff
        std::shared_ptr<RevisionNode> test1;
        std::shared_ptr<RevisionNode> test2;

        RevisionUtils::getDiff(test1, test2);


        // placeholder for now
        QString str = "Hello";
        QCOMPARE(str.toUpper(), QString("HELLO"));
    }
};

QTEST_MAIN(UnitTests)
