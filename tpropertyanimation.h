#ifndef TPROPERTYANIMATION_H
#define TPROPERTYANIMATION_H

#include <QObject>
#include "tvariantanimation.h"

class THELIBSSHARED_EXPORT tPropertyAnimation : public tVariantAnimation
{
    Q_OBJECT

public:
    tPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = Q_NULLPTR);
    ~tPropertyAnimation();

    QObject* targetObject;
    QByteArray targetName;

    using tVariantAnimation::finished;

public slots:
    void start(QAbstractAnimation::DeletionPolicy policy = DeleteWhenStopped);

protected slots:
    void overtake();

private slots:
    void propertyChanged(QVariant value);

private:
};

Q_DECLARE_METATYPE(tPropertyAnimation*)

#endif // TPROPERTYANIMATION_H
