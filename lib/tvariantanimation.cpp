#include "tvariantanimation.h"

tVariantAnimation::tVariantAnimation(QObject *parent) : QVariantAnimation(parent)
{

}

tVariantAnimation::~tVariantAnimation() {

}

void tVariantAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    if (this->state() != Running) {
        if ((!theLibsGlobal::instance()->allowSystemAnimations() || theLibsGlobal::instance()->powerStretchEnabled()) && !forceAnim) {
            QVariant newValue;
            int newTime;
            if (this->direction() == Forward) {
                newValue = this->endValue();
                newTime = this->duration();
            } else {
                newValue = this->startValue();
                newTime = 0;
            }

            QMetaObject::invokeMethod(this, "setCurrentTime", Qt::QueuedConnection, Q_ARG(int, newTime));
            QMetaObject::invokeMethod(this, "valueChanged", Qt::QueuedConnection, Q_ARG(QVariant, newValue));
            QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
        } else {
            QVariantAnimation::start(policy);
        }
    }
}

void tVariantAnimation::setForceAnimation(bool force) {
    forceAnim = force;
}

bool tVariantAnimation::forceAnimation() {
    return forceAnim;
}
