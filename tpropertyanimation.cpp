#include "tpropertyanimation.h"

tPropertyAnimation::tPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent) :
    tVariantAnimation(parent) {
    targetObject = target;
    targetName = propertyName;

    connect(this, SIGNAL(valueChanged(QVariant)), this, SLOT(propertyChanged(QVariant)));
    connect(targetObject, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));
    connect(this, &QVariantAnimation::stateChanged, [=](State newState, State oldState) {
       if (newState == Running) {
           targetObject->setProperty("t-anim", propertyName);
       } else {
           targetObject->setProperty("t-anim", "");
       }
    });
    connect(this, &QVariantAnimation::finished, [=]() {
        targetObject->setProperty("t-anim", "");
    });
}

tPropertyAnimation::~tPropertyAnimation() {

}

void tPropertyAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    if (targetObject->property("t-anim").toByteArray() != targetName) {
        targetObject->setProperty("t-anim", targetName);
        tVariantAnimation::start(policy);
    }
}

void tPropertyAnimation::propertyChanged(QVariant value) {
    targetObject->setProperty(targetName, value);
}
