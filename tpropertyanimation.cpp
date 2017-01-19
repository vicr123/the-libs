#include "tpropertyanimation.h"

tPropertyAnimation::tPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent) :
    tVariantAnimation(parent) {
    targetObject = target;
    targetName = propertyName;

    connect(this, SIGNAL(valueChanged(QVariant)), this, SLOT(propertyChanged(QVariant)));
    connect(targetObject, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
    connect(this, &QVariantAnimation::stateChanged, [=](State newState, State oldState) {
       if (newState == Running) {
           targetObject->setProperty("t-anim", QVariant::fromValue(this));
       } else {
           targetObject->setProperty("t-anim", "");
       }
    });
    connect(this, &QVariantAnimation::finished, [=]() {
        targetObject->setProperty("t-anim", "");
    });
}

tPropertyAnimation::~tPropertyAnimation() {
    /*if (!targetObject->property("t-anim").value<propertyAnimationInfo*>() != NULL) {
        if (targetObject->property("t-anim").value<propertyAnimationInfo*>()->runningAnimation == this) {
            targetObject->setProperty("t-anim", "");
        }
    }*/
}

void tPropertyAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    if (targetObject->property("t-anim").value<tPropertyAnimation*>() == NULL) {
        targetObject->setProperty("t-anim", QVariant::fromValue(this));
        tVariantAnimation::start(policy);
    } else {
        if (targetObject->property("t-anim").value<tPropertyAnimation*>()->targetName == targetName) {
            targetObject->property("t-anim").value<tPropertyAnimation*>()->overtake();
        }
        targetObject->setProperty("t-anim", QVariant::fromValue(this));
        tVariantAnimation::start(policy);
    }
}

void tPropertyAnimation::propertyChanged(QVariant value) {
    targetObject->setProperty(targetName, value);
}

void tPropertyAnimation::overtake() {
    this->stop();
    this->deleteLater();
}
