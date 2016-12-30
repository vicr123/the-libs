#include "tpropertyanimation.h"

/*tPropertyAnimation::tPropertyAnimation(QObject* parent) :
    tVariantAnimation(parent)
{
    //connect(tVariantAnimation, SIGNAL(finished()), this, SIGNAL(finished()));
    //connect(QPropertyAnimation, SIGNAL(finished()), this, SIGNAL(finished()));
}*/

tPropertyAnimation::tPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent) :
    tVariantAnimation(parent) {
    targetObject = target;
    targetName = propertyName;

    /*connect(this, tVariantAnimation::finished, [=]() {
        emit this->finished();
    });*/
    connect(this, SIGNAL(valueChanged(QVariant)), this, SLOT(propertyChanged(QVariant)));
}

tPropertyAnimation::~tPropertyAnimation() {

}

void tPropertyAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    qDebug() << "tPropertyAnimation::start() called";
    tVariantAnimation::start(policy);
}

void tPropertyAnimation::propertyChanged(QVariant value) {
    targetObject->setProperty(targetName, value);
}
