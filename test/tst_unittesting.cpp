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
    notification->insertAction("okay", "With Actions!");
    notification->post(false);

    QEventLoop* loop = new QEventLoop();
    connect(notification, &tNotification::actionClicked, [=](QString key) {
        loop->quit();
    });
    loop->exec();
    loop->deleteLater();

    QVERIFY(true);
}

QTEST_MAIN(UnitTesting)

#include "tst_unittesting.moc"
