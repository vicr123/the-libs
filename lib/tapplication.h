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

signals:
    void openFile(QString file);

public slots:
    static void setApplicationIcon(QIcon icon);

private:
    bool event(QEvent * event);

    tApplicationPrivate* d;
};

#endif // TAPPLICATION_H
