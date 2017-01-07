#include "the-libs_global.h"

theLibsGlobal::theLibsGlobal() : QObject(NULL) {
    QDBusMessage message = QDBusMessage::createMethodCall("org.thesuite.theshell", "/org/thesuite/Power", "org.thesuite.Power", "powerStretch");
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(message);
    if (reply.isValid()) {
        powerStretch = reply.value();
    }

    QDBusConnection::sessionBus().connect("org.thesuite.theshell", "/org/thesuite/Power", "org.thesuite.Power", "powerStretchChanged", this, SIGNAL(powerStretchChangedPrivate(bool)));
}

theLibsGlobal* theLibsGlobal::instance() {
    if (appInst == NULL) {
        appInst = new theLibsGlobal;
    }

    return appInst;
}

bool theLibsGlobal::powerStretchEnabled() {
    return powerStretch;
}

void theLibsGlobal::powerStretchChangedPrivate(bool isOn) {
    this->powerStretch = isOn;
    //QApplication::setStyle(QApplication::style()->)

    emit powerStretchChanged(isOn);

}
