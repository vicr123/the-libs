#ifndef TNOTIFICATION_H
#define TNOTIFICATION_H

#include "the-libs_global.h"
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QCoreApplication>

#ifdef T_OS_UNIX_NOT_MAC
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>
#include <QDBusMessage>
#include <QDBusConnection>
#endif

struct tNotificationPrivateByOS;
class THELIBSSHARED_EXPORT tNotification : public QObject
{
    Q_OBJECT
public:
    enum Urgency {
        Low = 0,
        Normal = 1,
        Critical = 2
    };

    explicit tNotification(QString summary = "", QString text = "", QObject *parent = 0);
    ~tNotification();

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

    void setAppIcon(QString appIcon);
    QString appIcon();

    void insertAction(QString key, QString text);
    void removeAction(QString key);

    void insertHint(QString key, QVariant hint);
signals:
    void actionClicked(QString key);

public slots:
    void post(bool deleteWhenDone = true);

private slots:

private:
    QString sum, txt, cat, snd, app, aIcon;
    bool isTransient = false, noSound = false;
    int time = -1;
    Urgency urg = Normal;
    QVariantMap extraHints;
    QMap<QString, QString> actions;

    tNotificationPrivateByOS* dd;

    void initialize();
    void destroy();
};

#endif // TNOTIFICATION_H
