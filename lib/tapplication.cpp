#include "tapplication.h"
#include <QFileOpenEvent>

tApplication::tApplication(int& argc, char** argv) : QApplication(argc, argv)
{

}

bool tApplication::event(QEvent *event) {
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = (QFileOpenEvent*) event;
        emit openFile(openEvent->file());
    }

    return QApplication::event(event);
}
