#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include <tnotification.h>

class UnitTesting : public QObject
{
        Q_OBJECT

    public:
        UnitTesting();

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();
        void notification();
};

UnitTesting::UnitTesting()
{
}

void UnitTesting::initTestCase()
{

}

void UnitTesting::cleanupTestCase()
{

}

void UnitTesting::notification()
{
    tNotification* notification = new tNotification("Notification", "This is a test notification");
    notification->post();
    QVERIFY(true);
}

QTEST_MAIN(UnitTesting)

#include "tst_unittesting.moc"
