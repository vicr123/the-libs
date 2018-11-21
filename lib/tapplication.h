#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include "the-libs_global.h"
#include <QApplication>

class tApplicationPrivate;

class THELIBSSHARED_EXPORT tApplication : public QApplication
{
    Q_OBJECT
public:
    explicit tApplication(int &argc, char **argv);
    ~tApplication();

signals:
    void openFile(QString file);

public slots:

private:
    bool event(QEvent * event);

    tApplicationPrivate* d;
};

#endif // TAPPLICATION_H
