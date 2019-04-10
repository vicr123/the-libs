#include "tnotification.h"
#include "tnotification-linux.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

struct tNotificationPrivateByOS {
    tNotificationLinuxHelper* h;
    uint replace = 0;
};

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

    hints.insert("desktop-entry", QApplication::applicationName().toLower());
    for (QString key : extraHints.keys()) {
        hints.insert(key, extraHints.value(key));
    }

    QStringList actions;
    for (QString key : this->actions.keys()) {
        actions << key << this->actions.value(key);
    }

    QVariantList args;
    if (this->app == "") {
        args.append(QCoreApplication::applicationName());
    } else {
        args.append(this->app);
    }
    args.append(dd->replace);
    args.append(aIcon);
    args.append(sum);
    args.append(txt);
    args.append(actions);
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
                dd->replace = reply.argumentAt<0>();
                dd->h->setListeningId(dd->replace);
            }
        }
    });
}

void tNotification::initialize() {
    dd = new tNotificationPrivateByOS();
    dd->h = new tNotificationLinuxHelper();

    QDBusConnection::sessionBus().connect("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications", "ActionInvoked", dd->h, SLOT(actionClicked(uint,QString)));
    connect(dd->h, &tNotificationLinuxHelper::didClick, this, &tNotification::actionClicked);
}

void tNotification::destroy() {
    QDBusConnection::sessionBus().disconnect("org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications", "ActionInvoked", dd->h, SLOT(actionClicked(uint,QString)));

    dd->h->deleteLater();
    delete dd;
}

tNotificationLinuxHelper::tNotificationLinuxHelper(QObject* parent) : QObject(parent) {

}

tNotificationLinuxHelper::~tNotificationLinuxHelper() {

}

void tNotificationLinuxHelper::actionClicked(uint id, QString key) {
    if (this->currentId == id) {
        emit didClick(key);
    }
}

void tNotificationLinuxHelper::setListeningId(uint id) {
    this->currentId = id;
}
