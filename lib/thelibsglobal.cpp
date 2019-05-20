#include "the-libs_global.h"

#include <QDesktopWidget>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

#include "private/nativeeventfilter.h"

#ifdef Q_OS_MAC
#define DPI_100_PERCENT 72.0
#else
#define DPI_100_PERCENT 96.0
#endif

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

struct theLibsGlobalPrivate {
    bool powerStretch = false;
    theLibsPrivate::NativeEventFilter* filter;
#ifdef T_OS_UNIX_NOT_MAC
    QSettings* themeSettings = new QSettings("theSuite", "ts-qtplatform");
#endif
};

theLibsGlobal::theLibsGlobal() : QObject(nullptr) {
    d = new theLibsGlobalPrivate();

    //Install the native event filter
    d->filter = new theLibsPrivate::NativeEventFilter(this);
    QApplication::instance()->installNativeEventFilter(d->filter);
    connect(d->filter, &theLibsPrivate::NativeEventFilter::powerStretchChanged, this, &theLibsGlobal::powerStretchChangedPrivate);

    #ifdef T_OS_UNIX_NOT_MAC
        QDBusMessage message = QDBusMessage::createMethodCall("org.thesuite.theshell", "/org/thesuite/Power", "org.thesuite.Power", "powerStretch");
        QDBusReply<bool> reply = QDBusConnection::sessionBus().call(message);
        if (reply.isValid()) {
            d->powerStretch = reply.value();
        }

        QDBusConnection::sessionBus().connect("org.thesuite.theshell", "/org/thesuite/Power", "org.thesuite.Power", "powerStretchChanged", this, SIGNAL(powerStretchChangedPrivate(bool)));
    #elif defined(Q_OS_WIN)
        //Register for power notifications
        QWidget* powerNotificationHandleWidget = new QWidget();
        RegisterPowerSettingNotification(HWND(powerNotificationHandleWidget->winId()), &GUID_POWER_SAVING_STATUS, 0);

        //Query power saver
        SYSTEM_POWER_STATUS powerStatus;
        BOOL success = GetSystemPowerStatus(&powerStatus);
        if (success) {
            d->powerStretch = powerStatus.SystemStatusFlag;
        }

    #endif
}

theLibsGlobal* theLibsGlobal::instance() {
    static theLibsGlobal* appInst;
    if (appInst == nullptr) {
        appInst = new theLibsGlobal;
    }

    return appInst;
}

bool theLibsGlobal::powerStretchEnabled() {
//#ifdef Q_OS_WIN
//    SYSTEM_POWER_STATUS powerStatus;
//    BOOL success = GetSystemPowerStatus(&powerStatus);
//    if (success) {
//        return powerStatus.SystemStatusFlag;
//    } else {
//        return false;
//    }
//#else
    return d->powerStretch;
//#endif
}

void theLibsGlobal::powerStretchChangedPrivate(bool isOn) {
    d->powerStretch = isOn;
    //QApplication::setStyle(QApplication::style()->)

    emit powerStretchChanged(isOn);

}

bool theLibsGlobal::allowSystemAnimations() {
    #ifdef T_OS_UNIX_NOT_MAC
        return d->themeSettings->value("accessibility/systemAnimations", true).toBool();
    #else
        return true;
    #endif
}

#ifdef QT_WIDGETS_LIB
double theLibsGlobal::getDPIScaling() {
    double currentDPI = QApplication::desktop()->logicalDpiX();
    return currentDPI / DPI_100_PERCENT;
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
