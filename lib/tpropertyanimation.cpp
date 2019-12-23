#include "tpropertyanimation.h"

tPropertyAnimation::tPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent) :
    tVariantAnimation(parent) {
    targetObject = target;
    targetName = propertyName;

    connect(this, &tPropertyAnimation::valueChanged, this, &tPropertyAnimation::propertyChanged);
    connect(targetObject, &tPropertyAnimation::destroyed, this, &tPropertyAnimation::stop);
    connect(targetObject, &tPropertyAnimation::destroyed, this, &tPropertyAnimation::deleteLater);
    connect(this, &tPropertyAnimation::stateChanged, targetObject, [=](State newState, State oldState) {
       if (newState == Running) {
           targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue(this));
       } else {
           targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue(static_cast<tPropertyAnimation*>(nullptr)));
       }
    });
    connect(this, &tPropertyAnimation::finished, targetObject, [=]() {
        targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue(static_cast<tPropertyAnimation*>(nullptr)));
    });
}

tPropertyAnimation::~tPropertyAnimation() {
    /*if (!targetObject->property("t-anim").value<propertyAnimationInfo*>() != NULL) {
        if (targetObject->property("t-anim").value<propertyAnimationInfo*>()->runningAnimation == this) {
            targetObject->setProperty("t-anim", "");
        }
    }*/
    disconnect(this, SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)));
    disconnect(this, SIGNAL(finished()));
}

void tPropertyAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    if (targetObject->property("t-anim:" + targetName).value<tPropertyAnimation*>() == nullptr) {
        targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue(this));
        tVariantAnimation::start(policy);
    } else {
        if (targetObject->property("t-anim:" + targetName).value<tPropertyAnimation*>()->targetName == targetName) {
            targetObject->property("t-anim:" + targetName).value<tPropertyAnimation*>()->overtake();
        }
        targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue(this));
        tVariantAnimation::start(policy);
    }
    targetObject->setProperty("t-anim:" + targetName, QVariant::fromValue(this));
    tVariantAnimation::start(policy);
}

void tPropertyAnimation::propertyChanged(QVariant value) {
    targetObject->setProperty(targetName, value);
}

void tPropertyAnimation::overtake() {
    this->stop();
    this->deleteLater();
}
