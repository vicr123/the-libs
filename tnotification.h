#ifndef TNOTIFICATION_H
#define TNOTIFICATION_H

#include <QObject>
#include <QDBusConnection>
#include <QString>
#include <QDBusMessage>
#include <QApplication>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

class tNotification : public QObject
{
    Q_OBJECT
public:
    enum Urgency {
        Low,
        Normal,
        Critical
    };

    explicit tNotification(QObject *parent = 0);

    void setAppName(QString app);
    void setSummary(QString summary);
    void setText(QString text);
    void setCategory(QString category);
    void setSound(QString sound);
    void setSuppressSound(bool suppress);
    void setUrgency(Urgency urgency);
    void setTimeout(int timeout);
signals:

public slots:
    void post();

private:
    QString appn, sum, txt, cat, snd;
    bool supSnd = false;
    int time = -1;
    uint replaces = 0;
    Urgency urg = Normal;
};

#endif // TNOTIFICATION_H
