#include "tnotification.h"

#include <QSystemTrayIcon>
#include <QTimer>

void tNotification::post(bool deleteWhenDone) {
    QSystemTrayIcon* ic = new QSystemTrayIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation));
    ic->show();
    ic->showMessage(sum, txt, QSystemTrayIcon::Information, 10000);
    QTimer::singleShot(10000, [=] {
        ic->hide();
        ic->deleteLater();
    });
}
