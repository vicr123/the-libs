#include "tapplication.h"
#include <QFileOpenEvent>
#include <QTranslator>

tApplication::tApplication(int& argc, char** argv) : QApplication(argc, argv)
{
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

    QTranslator localTranslator;
    localTranslator.load(QLocale::system().name(), ":/translations/");
    installTranslator(&localTranslator);
}

bool tApplication::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = (QFileOpenEvent*) event;
        emit openFile(openEvent->file());
    }

    return QApplication::event(event);
}
