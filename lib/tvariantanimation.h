#ifndef TVARIANTANIMATION_H
#define TVARIANTANIMATION_H

#include <QVariantAnimation>
#include <QDebug>
#include "the-libs_global.h"

class THELIBSSHARED_EXPORT tVariantAnimation : public QVariantAnimation
{
    Q_OBJECT

public:
    tVariantAnimation(QObject *parent = nullptr);
    ~tVariantAnimation();

    void setForceAnimation(bool force);
    bool forceAnimation();

public slots:
    void start(QAbstractAnimation::DeletionPolicy policy = KeepWhenStopped);

private:
    bool forceAnim = false;
};

#endif // TVARIANTANIMATION_H
