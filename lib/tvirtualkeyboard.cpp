#include "tvirtualkeyboard.h"

tVirtualKeyboard::tVirtualKeyboard() : QObject(NULL) {
#ifdef T_OS_UNIX_NOT_MAC
    keyboardInterface = new QDBusInterface("org.thesuite.tskbd", "/org/thesuite/tskbd", "org.thesuite.tskbd");
    QDBusConnection::sessionBus().connect("org.thesuite.tskbd", "/org/thesuite/tskbd", "org.thesuite.tskbd", "keyboardVisibleChanged", this, SIGNAL(keyboardVisibleChanged(bool)));
#endif
}

tVirtualKeyboard* tVirtualKeyboard::instance() {
    static tVirtualKeyboard* appInst;
    if (appInst == NULL) {
        appInst = new tVirtualKeyboard;
    }

    return appInst;
}

bool tVirtualKeyboard::isKeyboardRunning() {
#ifdef T_OS_UNIX_NOT_MAC
    return QDBusConnection::sessionBus().interface()->registeredServiceNames().value().contains("org.thesuite.tskbd");
#else
    return false;
#endif
}

void tVirtualKeyboard::hideKeyboard() {
#ifdef T_OS_UNIX_NOT_MAC
    if (isKeyboardRunning()) {
        keyboardInterface->call(QDBus::Block, "hideKeyboard");
    }
#endif
}

void tVirtualKeyboard::showKeyboard() {
#ifdef T_OS_UNIX_NOT_MAC
    if (isKeyboardRunning()) {
        keyboardInterface->call(QDBus::Block, "showKeyboard");
    }
#endif
}

int tVirtualKeyboard::height() {
#ifdef T_OS_UNIX_NOT_MAC
    if (isKeyboardRunning()) {
        return keyboardInterface->call(QDBus::Block, "height").arguments().first().toInt();
    } else {
        return -1;
    }
#else
    return -1;
#endif
}
