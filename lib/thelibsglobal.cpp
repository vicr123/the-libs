#include "the-libs_global.h"

#include <QDesktopWidget>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include "tsettings.h"

#include "private/nativeeventfilter.h"

#ifdef QT_WIDGETS_LIB
    #include <QPainter>
#endif

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
    tSettings* contemporarySettings;
#endif
};

theLibsGlobal::theLibsGlobal() : QObject(nullptr) {
    d = new theLibsGlobalPrivate();
    d->contemporarySettings = new tSettings("theSuite", "contemporary_widget", this);

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
    return d->powerStretch;
}

void theLibsGlobal::powerStretchChangedPrivate(bool isOn) {
    d->powerStretch = isOn;

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

QColor theLibsGlobal::lineColor(QColor textColor) {
    if (instance()->d->contemporarySettings->value("Lines/reduceIntensity").toBool()) {
        textColor.setAlpha(127);
    }
    return textColor;
}

#ifdef QT_WIDGETS_LIB
void theLibsGlobal::tintImage(QImage& image, QColor tint) {
    //bool doPaint = true;
    int failNum = 0;
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            QColor pixelCol = image.pixelColor(x, y);
            //int blue = pixelCol.blue(), green = pixelCol.green(), red = pixelCol.red();
            if ((pixelCol.blue() > pixelCol.green() - 10 && pixelCol.blue() < pixelCol.green() + 10) &&
                (pixelCol.green() > pixelCol.red() - 10 && pixelCol.green() < pixelCol.red() + 10)) {
            } else {
                failNum++;
                //doPaint = false;
            }
        }
    }

    if (failNum < (image.size().width() * image.size().height()) / 8) {
        QPainter painter(&image);
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(0, 0, image.width(), image.height(), tint);
        painter.end();
    }
}
#endif
