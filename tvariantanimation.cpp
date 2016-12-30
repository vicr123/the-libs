#include "tvariantanimation.h"

tVariantAnimation::tVariantAnimation(QObject *parent) : QVariantAnimation(parent)
{
    QDBusMessage message = QDBusMessage::createMethodCall("org.thesuite.theshell", "/org/thesuite/Power", "org.thesuite.Power", "powerStretch");
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(message);
    if (reply.isValid()) {
        isPowerStretchOn = reply.value();
    }

    QDBusConnection::sessionBus().connect("org.thesuite.theshell", "/org/thesuite/Power", "org.thesuite.Power", "powerStretchChanged", this, SLOT(powerStretchUpdate(bool)));
}

tVariantAnimation::~tVariantAnimation() {

}

void tVariantAnimation::powerStretchUpdate(bool isOn) {
    isPowerStretchOn = isOn;
}

void tVariantAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    if (this->state() != Running) {
        if (isPowerStretchOn && !forceAnim) {
            emit valueChanged(this->endValue());
            emit finished();
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
