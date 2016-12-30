#ifndef TVARIANTANIMATION_H
#define TVARIANTANIMATION_H

#include <QVariantAnimation>
#include <QDebug>
#include "the-libs_global.h"

class THELIBSSHARED_EXPORT tVariantAnimation : public QVariantAnimation
{
    Q_OBJECT

public:
    tVariantAnimation(QObject *parent = Q_NULLPTR);
    ~tVariantAnimation();

public slots:
    void start(QAbstractAnimation::DeletionPolicy policy = DeleteWhenStopped);
};

#endif // TVARIANTANIMATION_H
