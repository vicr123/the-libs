#include "tvirtualkeyboard.h"

tVirtualKeyboard::tVirtualKeyboard() : QObject(NULL) {
    keyboardInterface = new QDBusInterface("org.thesuite.tskbd", "/org/thesuite/tskbd", "org.thesuite.tskbd");
    QDBusConnection::sessionBus().connect("org.thesuite.tskbd", "/org/thesuite/tskbd", "org.thesuite.tskbd", "keyboardVisibleChanged", this, SIGNAL(keyboardVisibleChanged(bool)));
}

tVirtualKeyboard* tVirtualKeyboard::instance() {
    static tVirtualKeyboard* appInst;
    if (appInst == NULL) {
        appInst = new tVirtualKeyboard;
    }

    return appInst;
}

bool tVirtualKeyboard::isKeyboardRunning() {
    return QDBusConnection::sessionBus().interface()->registeredServiceNames().value().contains("org.thesuite.tskbd");
}

void tVirtualKeyboard::hideKeyboard() {
    if (isKeyboardRunning()) {
        keyboardInterface->call(QDBus::Block, "hideKeyboard");
    }
}

void tVirtualKeyboard::showKeyboard() {
    if (isKeyboardRunning()) {
        keyboardInterface->call(QDBus::Block, "showKeyboard");
    }
}

int tVirtualKeyboard::height() {
    if (isKeyboardRunning()) {
        return keyboardInterface->call(QDBus::Block, "height").arguments().first().toInt();
    } else {
        return -1;
    }
}

bool tVirtualKeyboard::keyboardVisible() {
    if (isKeyboardRunning()) {
        return keyboardInterface->call(QDBus::Block, "keyboardVisible").arguments().first().toBool();
    } else {
        return false;
    }
}
