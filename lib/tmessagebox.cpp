#include "tmessagebox.h"

tMessageBox::tMessageBox(QWidget *parent) : QMessageBox(parent)
{

}

void tMessageBox::setWindowTitle(QString windowTitle) {
#ifdef Q_OS_MAC
    QMessageBox::setText(windowTitle);
#else
    QMessageBox::setWindowTitle(windowTitle);
#endif
}

void tMessageBox::setText(QString text) {
#ifdef Q_OS_MAC
    QMessageBox::setInformativeText(text);
#else
    QMessageBox::setText(text);
#endif
}
