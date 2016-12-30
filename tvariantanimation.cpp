#include "tvariantanimation.h"

tVariantAnimation::tVariantAnimation(QObject *parent) : QVariantAnimation(parent)
{

}

tVariantAnimation::~tVariantAnimation() {

}

void tVariantAnimation::start(QAbstractAnimation::DeletionPolicy policy) {
    qDebug() << "tVariantAnimation::start() called";
    //QVariantAnimation::start(policy);
    emit valueChanged(this->endValue());
    emit finished();
}
