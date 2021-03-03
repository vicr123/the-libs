#include "tvariantanimation.h"

tVariantAnimation::tVariantAnimation(QObject* parent) : QVariantAnimation(parent) {

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

tVariantAnimation* tVariantAnimation::singleShot(QObject* parent, QVariant start, QVariant end, int duration, QEasingCurve easingCurve, tVariantAnimation::ValueChangedFunction valueChangedCallback, FinishedFunction finishedCallback) {
    tVariantAnimation* anim = new tVariantAnimation(parent);
    anim->setStartValue(start);
    anim->setEndValue(end);
    anim->setDuration(duration);
    anim->setEasingCurve(easingCurve);
    connect(anim, &tVariantAnimation::valueChanged, parent, [ = ](QVariant value) {
        valueChangedCallback(value);
    });
    connect(anim, &tVariantAnimation::finished, parent, [ = ] {
        finishedCallback();
        anim->deleteLater();
    });
    anim->start();

    return anim;
}

tVariantAnimation* tVariantAnimation::singleShot(QObject* parent, QVariant start, QVariant end, int duration, QEasingCurve easingCurve, tVariantAnimation::ValueChangedFunction valueChangedCallback) {
    return tVariantAnimation::singleShot(parent, start, end, duration, easingCurve, valueChangedCallback, [ = ] {});
}

tVariantAnimation* tVariantAnimation::singleShot(QObject* parent, QVariant start, QVariant end, int duration, tVariantAnimation::ValueChangedFunction valueChangedCallback, FinishedFunction finishedCallback) {
    return tVariantAnimation::singleShot(parent, start, end, duration, QEasingCurve::OutCubic, valueChangedCallback, finishedCallback);
}

tVariantAnimation* tVariantAnimation::singleShot(QObject* parent, QVariant start, QVariant end, int duration, tVariantAnimation::ValueChangedFunction valueChangedCallback) {
    return tVariantAnimation::singleShot(parent, start, end, duration, valueChangedCallback, [ = ] {});
}
