#include "tpropertyanimation.h"

tPropertyAnimation::tPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent) :
    tVariantAnimation(parent) {
    targetObject = target;
    targetName = propertyName;

    connect(this, SIGNAL(valueChanged(QVariant)), this, SLOT(propertyChanged(QVariant)));
    connect(targetObject, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
    connect(this, &QVariantAnimation::stateChanged, [=](State newState, State oldState) {
       if (newState == Running) {
           targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue(this));
       } else {
           targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue((tPropertyAnimation*) NULL));
       }
    });
    connect(this, &QVariantAnimation::finished, [=]() {
        targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue((tPropertyAnimation*) NULL));
    });
}

tPropertyAnimation::~tPropertyAnimation() {

}

void tPropertyAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    if (targetObject->property("t-anim:" + targetName).value<tPropertyAnimation*>() != NULL) {
        targetObject->property("t-anim:" + targetName).value<tPropertyAnimation*>()->stop();
    }
    targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue(this));
    tVariantAnimation::start(policy);
}

void tPropertyAnimation::propertyChanged(QVariant value) {
    targetObject->setProperty(targetName, value);
}
