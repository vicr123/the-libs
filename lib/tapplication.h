#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include "the-libs_global.h"
#include <QApplication>

struct tApplicationPrivate;

class THELIBSSHARED_EXPORT tApplication : public QApplication
{
    Q_OBJECT
    public:
        explicit tApplication(int &argc, char **argv);
        ~tApplication();

        static QIcon applicationIcon();

    Q_SIGNALS:
        void openFile(QString file);

    public Q_SLOTS:
        void registerCrashTrap();
        static QStringList exportBacktrace();
        static void setApplicationIcon(QIcon icon);

    private:
        bool event(QEvent * event);

        static tApplicationPrivate* d;
};

#endif // TAPPLICATION_H
