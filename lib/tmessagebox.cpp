#include "tmessagebox.h"

#include "tapplication.h"
#include <QPainter>

tMessageBox::tMessageBox(QWidget *parent) : QMessageBox(parent)
{

}

tMessageBox::~tMessageBox() {

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

int tMessageBox::exec() {
#ifdef Q_OS_MAC
    if (icon() == Warning && !tApplication::applicationIcon().isNull()) {
        //Overlay the application icon over the warning icon
        QIcon warningIcon = tApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning);
        int iconSizeInt = tApplication::style()->pixelMetric(QStyle::PM_MessageBoxIconSize);

        QSize iconSize(iconSizeInt, iconSizeInt);

        QPixmap newPixmap(iconSize);
        newPixmap.fill(Qt::transparent);

        QPainter painter(&newPixmap);
        painter.drawPixmap(0, 0, warningIcon.pixmap(iconSize));

        QSize appIconSize = iconSize / 2;
        painter.drawPixmap(appIconSize.width(), appIconSize.height(), tApplication::applicationIcon().pixmap(appIconSize));

        painter.end();
        setIconPixmap(newPixmap);
    }
#endif

    return QMessageBox::exec();
}
