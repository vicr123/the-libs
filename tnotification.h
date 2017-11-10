#ifndef TNOTIFICATION_H
#define TNOTIFICATION_H

#include <QObject>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

class tNotification : public QObject
{
    Q_OBJECT
public:
    enum Urgency {
        Low = 0,
        Normal = 1,
        Critical = 2
    };

    explicit tNotification(QString summary = "", QString text = "", QObject *parent = 0);

    void setTransient(bool transient);
    bool transient();

    void setTimeout(int timeout);
    int timeout();

    void setSummary(QString summary);
    QString summary();

    void setText(QString text);
    QString text();

    void setCategory(QString category);
    QString category();

    void setSound(QString sound);
    QString sound();

    void setSoundOn(bool soundOn);
    bool soundOn();

    void setUrgency(Urgency urgency);
    Urgency urgency();

    void setAppName(QString appName);
    QString appName();
signals:
    void actionClicked(QString key);

public slots:
    void post(bool deleteWhenDone = true);

private slots:

private:
    QString sum, txt, cat, snd, app;
    bool isTransient = false, noSound = false;
    uint replace = 0;
    int time = -1;
    Urgency urg = Normal;
};

#endif // TNOTIFICATION_H
