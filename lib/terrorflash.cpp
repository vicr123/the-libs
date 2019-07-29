#include "terrorflash.h"

#include <QWidget>
#include <QTimer>
#include "tvariantanimation.h"

struct tErrorFlashPrivate {
    tErrorFlash* instance = nullptr;
};

tErrorFlashPrivate* tErrorFlash::d = new tErrorFlashPrivate();

tErrorFlash::tErrorFlash(QObject *parent) : QObject(parent)
{

}

void tErrorFlash::flashError(QWidget *flash) {
    if (d->instance == nullptr) d->instance = new tErrorFlash();
    emit d->instance->startingFlash(flash);

    QColor flashColor = QColor(255, 0, 0);

    QPalette initialPalette = flash->palette();

    tVariantAnimation* anim2 = new tVariantAnimation();
    anim2->setStartValue(flashColor);
    anim2->setEndValue(initialPalette.color(QPalette::Base));
    anim2->setDuration(1000);
    anim2->setEasingCurve(QEasingCurve::OutCubic);

    tVariantAnimation* anim = new tVariantAnimation();
    anim->setStartValue(flashColor);
    anim->setEndValue(initialPalette.color(QPalette::Window));
    anim->setDuration(1000);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(flash, &QWidget::destroyed, anim, &tVariantAnimation::stop);
    connect(flash, &QWidget::destroyed, anim, &tVariantAnimation::deleteLater);
    connect(anim, &tVariantAnimation::valueChanged, [=](QVariant value) {
        anim2->setCurrentTime(anim->currentTime());

        QPalette pal = initialPalette;
        pal.setColor(QPalette::Window, value.value<QColor>());
        pal.setColor(QPalette::Base, anim2->currentValue().value<QColor>());
        flash->setPalette(pal);
    });
    connect(anim, &tVariantAnimation::finished, anim, &tVariantAnimation::deleteLater);
    connect(anim, &tVariantAnimation::destroyed, anim2, &tVariantAnimation::deleteLater);
    connect(d->instance, &tErrorFlash::startingFlash, anim, [=](QWidget* widget) {
        //Stop the animation if a flash occurs again
        if (flash == widget) {
            anim->stop();
            anim->setCurrentTime(anim->duration());
            anim->deleteLater();
        }
    });
    anim->start();
}
