#ifndef TPROPERTYANIMATION_H
#define TPROPERTYANIMATION_H

#include <QObject>
#include "tvariantanimation.h"

class THELIBSSHARED_EXPORT tPropertyAnimation : public tVariantAnimation
{
    Q_OBJECT

public:
    //tPropertyAnimation(QObject *parent = Q_NULLPTR);
    tPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = Q_NULLPTR);
    ~tPropertyAnimation();

    using tVariantAnimation::finished;

public slots:
    void start(QAbstractAnimation::DeletionPolicy policy = DeleteWhenStopped);

private slots:
    void propertyChanged(QVariant value);

private:
    QObject* targetObject;
    QByteArray targetName;
};

#endif // TPROPERTYANIMATION_H
