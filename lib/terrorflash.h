#ifndef TERRORFLASH_H
#define TERRORFLASH_H

#include <QObject>
#include "the-libs_global.h"

struct tErrorFlashPrivate;
class THELIBSSHARED_EXPORT tErrorFlash : public QObject
{
        Q_OBJECT
    public:
        static void flashError(QWidget* flash);

    signals:
        void startingFlash(QWidget* flash);

    public slots:

    private:
        explicit tErrorFlash(QObject *parent = nullptr);
        static tErrorFlashPrivate* d;
};

#endif // TERRORFLASH_H
