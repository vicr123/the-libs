#include "tvariantanimation.h"

tVariantAnimation::tVariantAnimation(QObject *parent) : QVariantAnimation(parent)
{

}

tVariantAnimation::~tVariantAnimation() {

}

void tVariantAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    if (this->state() != Running) {
        if (theLibsGlobal::instance()->powerStretchEnabled() && !forceAnim) {
            //emit valueChanged(this->endValue());
            QMetaObject::invokeMethod(this, "valueChanged", Qt::QueuedConnection, Q_ARG(QVariant, this->endValue()));
            QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
            //emit finished();
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
