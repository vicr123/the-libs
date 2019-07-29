#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include <tnotification.h>
#include <tsystemsound.h>

class UnitTesting : public QObject
{
        Q_OBJECT

    public:
        UnitTesting();

    private Q_SLOTS:
        void initTestCase();
        void cleanupTestCase();

        void sound();
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

    QVERIFY(true);
}

void UnitTesting::sound() {
    tSystemSound* sound = tSystemSound::play("desktop-login");
    if (sound == nullptr) {
        //Didn't work
        QVERIFY(false);
    } else {
        QEventLoop* loop = new QEventLoop();
        connect(sound, &tSystemSound::done, [=] {
            loop->quit();
        });
        loop->exec();
        loop->deleteLater();

        QVERIFY(true);
    }
}

QTEST_MAIN(UnitTesting)

#include "tst_unittesting.moc"
