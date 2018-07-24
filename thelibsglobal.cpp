#include "the-libs_global.h"

#include <QDesktopWidget>

theLibsGlobal::theLibsGlobal() : QObject(NULL) {
    #ifdef Q_OS_UNIX
        QDBusMessage message = QDBusMessage::createMethodCall("org.thesuite.theshell", "/org/thesuite/Power", "org.thesuite.Power", "powerStretch");
        QDBusReply<bool> reply = QDBusConnection::sessionBus().call(message);
        if (reply.isValid()) {
            powerStretch = reply.value();
        }

        QDBusConnection::sessionBus().connect("org.thesuite.theshell", "/org/thesuite/Power", "org.thesuite.Power", "powerStretchChanged", this, SIGNAL(powerStretchChangedPrivate(bool)));
    #endif
}

theLibsGlobal* theLibsGlobal::instance() {
    static theLibsGlobal* appInst;
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

bool theLibsGlobal::allowSystemAnimations() {
    #ifdef Q_OS_UNIX
        return themeSettings->value("accessibility/systemAnimations", true).toBool();
    #else
        return true;
    #endif
}

float theLibsGlobal::getDPIScaling() {
    float currentDPI = QApplication::desktop()->logicalDpiX();
    return currentDPI / (float) 96;
}
