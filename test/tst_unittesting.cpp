#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QRandomGenerator>

#include <tnotification.h>
#include <tsystemsound.h>
#include <tpromise.h>

QString generateRandomString() {
    QString characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+[]{}\\|;':\",.<>/?`~";
    QRandomGenerator* generator = QRandomGenerator::system();
    QString finalString;
    int length = generator->bounded(128);
    for (int i = 0; i < length; i++) {
        finalString.append(characters.at(generator->bounded(characters.length())));
    }
    return finalString;
}

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

        void voidPromise();
        void valuePromise();
        void asyncVoidPromise();
        void asyncValuePromise();
        void promiseRejection();
        void asyncPromiseRejection();
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

void UnitTesting::voidPromise()
{
    QDateTime now = QDateTime::currentDateTime();
    (new tPromise<void>([=](QString err) {
        QThread::sleep(1);
    }))->await();

    //Allow some room for error
    QVERIFY(now.msecsTo(QDateTime::currentDateTime()) > 700);
}

void UnitTesting::valuePromise()
{
    QString testString = generateRandomString();
    tPromiseResults<QString> results = (new tPromise<QString>([=](QString err) {
        return testString;
    }))->await();

    QCOMPARE(results.result, testString);
}

void UnitTesting::asyncVoidPromise()
{
    QDateTime now = QDateTime::currentDateTime();
    (new tPromise<void>([=](auto res, auto rej) {
        QTimer::singleShot(1000, res);
    }))->await();

    //Allow some room for error
    QVERIFY(now.msecsTo(QDateTime::currentDateTime()) > 700);
}

void UnitTesting::asyncValuePromise()
{
    QString testString = generateRandomString();
    tPromiseResults<QString> results = (new tPromise<QString>([=](auto res, auto rej) {
        res(testString);
    }))->await();

    QCOMPARE(results.result, testString);
}

void UnitTesting::promiseRejection()
{
    tPromiseResults<void> results = (new tPromise<void>([=](QString& err) {
        err = "An error";
    }))->await();

    QCOMPARE(results.error, "An error");
}

void UnitTesting::asyncPromiseRejection()
{
    tPromiseResults<void> results = (new tPromise<void>([=](auto res, auto rej) {
        rej("An error");
    }))->await();

    QCOMPARE(results.error, "An error");
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
