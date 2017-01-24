#include "tnotification.h"

tNotification::tNotification(QObject *parent) : QObject(parent)
{

}



void tNotification::setAppName(QString app) {
    appn = app;
}

void tNotification::setSummary(QString summary) {
    sum = summary;
}

void tNotification::setText(QString text) {
    txt = text;
}

void tNotification::setCategory(QString category) {
    cat = category;
}

void tNotification::setSound(QString sound) {
    snd = sound;
}

void tNotification::setUrgency(Urgency urgency) {
    urg = urgency;
}

void tNotification::setTimeout(int timeout) {
    time = timeout;
}

void tNotification::post() {
    QDBusMessage message = QDBusMessage::createMethodCall("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications", "Notify");

    QVariantMap hints;
    hints.insert("category", cat);
    hints.insert("suppress-sound", supSnd);
    if (snd != "") {
        hints.insert("sound-file", snd);
    }

    QString appName = appn;
    if (appName == "") appName = QApplication::applicationName();

    QVariantList args;
    args.append(appName);
    args.append(replaces);
    args.append("");
    args.append(sum);
    args.append(txt);
    args.append(QStringList());
    args.append(hints);
    args.append(time);

    message.setArguments(args);

    QDBusPendingReply<uint> reply = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, [=]() {
        QDBusPendingReply<uint> reply = *watcher;
        if (!reply.isError()) {
            this->replaces = reply.argumentAt<0>();
        }
    });
}
