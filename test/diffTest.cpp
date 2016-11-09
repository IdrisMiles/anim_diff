#include <QtTest/QtTest>

class UnitTests: public QObject
{

public:
    UnitTests(){}
    virtual ~UnitTests(){}

private:

    void toUpper()
    {
        QString str = "Hello";
        QCOMPARE(str.toUpper(), QString("HELLO"));
    }
};

QTEST_MAIN(UnitTests)
