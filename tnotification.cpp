#include "tnotification.h"

tNotification::tNotification(QString summary, QString text, QObject *parent) : QObject(parent)
{
    this->sum = summary;
    this->txt = text;
}

void tNotification::post(bool deleteWhenDone) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications", "Notify");

    QVariantMap hints;
    hints.insert("transient", this->isTransient);
    hints.insert("suppress-sound", this->noSound);
    hints.insert("category", this->cat);
    hints.insert("urgency", (int) urg);

    if (this->snd != "") {
        hints.insert("sound-file", this->snd);
    }

    QVariantList args;
    if (this->app == "") {
        args.append(QCoreApplication::applicationName());
    } else {
        args.append(this->app);
    }
    args.append(replace);
    args.append("");
    args.append(sum);
    args.append(txt);
    args.append(QStringList());
    args.append(hints);
    args.append(time);
    message.setArguments(args);

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, [=](QDBusPendingCallWatcher* watcher) {
        if (deleteWhenDone) {
            this->deleteLater();
        } else {
            QDBusPendingReply<uint> reply = *watcher;
            if (!reply.isError()) {
                this->replace = reply.argumentAt<0>();
            }
        }
    });
}

void tNotification::setTransient(bool transient) {
    this->isTransient = transient;
}

bool tNotification::transient() {
    return isTransient;
}

void tNotification::setTimeout(int timeout) {
    this->time = timeout;
}

int tNotification::timeout() {
    return this->time;
}

void tNotification::setSummary(QString summary) {
    this->sum = summary;
}

QString tNotification::summary() {
    return this->sum;
}

void tNotification::setText(QString text) {
    this->txt = text;
}

QString tNotification::text() {
    return this->txt;
}

void tNotification::setCategory(QString category) {
    this->cat = category;
}

QString tNotification::category() {
    return this->cat;
}

void tNotification::setSound(QString sound) {
    this->snd = sound;
}

QString tNotification::sound() {
    return this->snd;
}

void tNotification::setSoundOn(bool soundOn) {
    this->noSound = !soundOn;
}

bool tNotification::soundOn() {
    return !this->noSound;
}

void tNotification::setUrgency(Urgency urgency) {
    this->urg = urgency;
}

tNotification::Urgency tNotification::urgency() {
    return this->urg;
}

void tNotification::setAppName(QString appName) {
    this->app = appName;
}

QString tNotification::appName() {
    return this->app;
}
