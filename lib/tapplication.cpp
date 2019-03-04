#include "tapplication.h"
#include <QFileOpenEvent>
#include <QTranslator>

struct tApplicationPrivate {
    QTranslator translator;

    QIcon applicationIcon;
};

tApplication* tApplicationInstance;

tApplication::tApplication(int& argc, char** argv) : QApplication(argc, argv)
{
    tApplicationInstance = this;
    d = new tApplicationPrivate();
    //Mark some strings for translation so apps look right on macOS
    if (false) {
        QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Services");
        QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Hide %1");
        QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Hide Others");
        QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Show All");
        QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Preferences...");
        QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "About %1");
        QT_TRANSLATE_NOOP("MAC_APPLICATION_MENU", "Quit %1");
    }

    Q_INIT_RESOURCE(thelibs_translations);
    d->translator.load(QLocale::system().name(), ":/the-libs/translations/");
    installTranslator(&d->translator);

    //localTranslator.
}

bool tApplication::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = (QFileOpenEvent*) event;
        emit openFile(openEvent->file());
    }

    return QApplication::event(event);
}

tApplication::~tApplication() {
    delete d;
}

QIcon tApplication::applicationIcon() {
    return tApplicationInstance->d->applicationIcon;
}

void tApplication::setApplicationIcon(QIcon icon) {
    tApplicationInstance->d->applicationIcon = icon;
}
