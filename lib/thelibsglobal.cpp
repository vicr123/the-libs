#include "the-libs_global.h"

#include <QDesktopWidget>
#include <QDir>
#include <QDirIterator>

theLibsGlobal::theLibsGlobal() : QObject(NULL) {
    #ifdef T_OS_UNIX_NOT_MAC
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
    #ifdef T_OS_UNIX_NOT_MAC
        return themeSettings->value("accessibility/systemAnimations", true).toBool();
    #else
        return true;
    #endif
}

#ifdef QT_WIDGETS_LIB
float theLibsGlobal::getDPIScaling() {
    float currentDPI = QApplication::desktop()->logicalDpiX();
    return currentDPI / (float) 96;
}
#endif

QStringList theLibsGlobal::searchInPath(QString executable) {
    QStringList executables;
    QStringList pathDirs = QString(qgetenv("PATH")).split(":");
    for (QString dir : pathDirs) {
        QDir path(dir);
        QDirIterator i(path);
        while (i.hasNext()) {
            i.next();
            if (i.fileName() == executable) {
                executables.append(i.filePath());
            }
        }
    }

    return executables;
}
