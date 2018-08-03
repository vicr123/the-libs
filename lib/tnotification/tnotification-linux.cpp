#include "tnotification.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

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

    for (QString key : extraHints.keys()) {
        hints.insert(key, extraHints.value(key));
    }

    QVariantList args;
    if (this->app == "") {
        args.append(QCoreApplication::applicationName());
    } else {
        args.append(this->app);
    }
    args.append(replace);
    args.append(aIcon);
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
