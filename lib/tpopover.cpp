/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "tpopover.h"

#include <QFrame>
#include <QGraphicsOpacityEffect>
#include "tpropertyanimation.h"

struct tPopoverPrivate {
    QWidget* popoverWidget;
    QWidget* parentWidget;
    QFrame* verticalSeperator;

    QWidget* blanker;
    QGraphicsOpacityEffect* blankerEffect;

    tPopover::PopoverSide side = tPopover::Trailing;

    int width = -1;
    bool showing = false;

    static QMap<QWidget*, tPopover*> activePopovers;
};
QMap<QWidget*, tPopover*> tPopoverPrivate::activePopovers = QMap<QWidget*, tPopover*>();

tPopover::tPopover(QWidget* popoverWidget, QObject *parent) : QObject(parent)
{
    d = new tPopoverPrivate();
    d->popoverWidget = popoverWidget;
    popoverWidget->setAutoFillBackground(true);

    d->blanker = new QWidget();
    d->blanker->setAutoFillBackground(true);
    d->blanker->installEventFilter(this);

    d->blankerEffect = new QGraphicsOpacityEffect();
    d->blankerEffect->setOpacity(0);
    d->blanker->setGraphicsEffect(d->blankerEffect);

    d->verticalSeperator = new QFrame();
    d->verticalSeperator->setFrameShape(QFrame::VLine);
}

tPopover::~tPopover() {
    tPopoverPrivate::activePopovers.remove(d->popoverWidget);
    delete d;
}

bool tPopover::isOpeningOnRight() {
    return (QApplication::layoutDirection() == Qt::LeftToRight && d->side == Trailing) ||
            (QApplication::layoutDirection() == Qt::RightToLeft && d->side == Leading);
}

void tPopover::setPopoverWidth(int width) {
    d->width = width;
}

void tPopover::setPopoverSide(PopoverSide side) {
    d->side = side;
}

void tPopover::show(QWidget* parent) {
    if (d->showing) return;
    tPopoverPrivate::activePopovers.insert(d->popoverWidget, this);

    d->parentWidget = parent;
    d->parentWidget->installEventFilter(this);

    d->blanker->hide();
    d->verticalSeperator->hide();
    d->popoverWidget->hide();

    d->blanker->setParent(parent);
    d->popoverWidget->setParent(parent);
    d->verticalSeperator->setParent(parent);

    d->blanker->setGeometry(0, 0, parent->width(), parent->height());
    d->verticalSeperator->resize(1, parent->height());
    if (d->width == -1) {
        d->popoverWidget->resize(parent->width(), parent->height());
    } else if (d->width < 0) {
        d->popoverWidget->resize(parent->width() + d->width, parent->height());
    } else {
        d->popoverWidget->resize(d->width, parent->height());
    }

    tPropertyAnimation* blankerAnim = new tPropertyAnimation(d->blankerEffect, "opacity");
    blankerAnim->setStartValue(d->blankerEffect->opacity());
    blankerAnim->setEndValue((qreal) 0.75);
    blankerAnim->setDuration(250);
    blankerAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(blankerAnim, &tVariantAnimation::finished, blankerAnim, &tVariantAnimation::deleteLater);
    blankerAnim->start();

    tVariantAnimation* popoverAnim = new tVariantAnimation();
    if (isOpeningOnRight()) {
        popoverAnim->setStartValue(parent->width());
        popoverAnim->setEndValue(parent->width() - d->popoverWidget->width());
    } else {
        popoverAnim->setStartValue(-d->popoverWidget->width());
        popoverAnim->setEndValue(0);
    }
    popoverAnim->setDuration(250);
    popoverAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(popoverAnim, &tVariantAnimation::valueChanged, [=](QVariant value) {
        d->popoverWidget->move(value.toInt(), 0);
        if (isOpeningOnRight()) {
            d->verticalSeperator->move(value.toInt() - 1, 0);
        } else {
            d->verticalSeperator->move(value.toInt() + d->popoverWidget->width(), 0);
        }
    });
    connect(popoverAnim, &tVariantAnimation::finished, popoverAnim, &tVariantAnimation::deleteLater);
    popoverAnim->start();

    d->blanker->show();
    d->verticalSeperator->show();
    d->popoverWidget->show();

    d->blanker->raise();
    d->verticalSeperator->raise();
    d->popoverWidget->raise();

    d->popoverWidget->setFocus();

    d->showing = true;
}

void tPopover::dismiss() {
    if (!d->showing) return;

    tPropertyAnimation* blankerAnim = new tPropertyAnimation(d->blankerEffect, "opacity");
    blankerAnim->setStartValue(d->blankerEffect->opacity());
    blankerAnim->setEndValue((qreal) 0);
    blankerAnim->setDuration(250);
    blankerAnim->setEasingCurve(QEasingCurve::OutCubic);
    blankerAnim->start();

    tVariantAnimation* popoverAnim = new tVariantAnimation();
    popoverAnim->setStartValue(d->popoverWidget->x());
    if (isOpeningOnRight()) {
        //Opening on the right
        popoverAnim->setEndValue(d->parentWidget->width());
    } else {
        popoverAnim->setEndValue(-d->parentWidget->width());
    }
    popoverAnim->setDuration(250);
    popoverAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(popoverAnim, &tVariantAnimation::valueChanged, [=](QVariant value) {
        d->popoverWidget->move(value.toInt(), 0);

        if (isOpeningOnRight()) {
            d->verticalSeperator->move(value.toInt() - 1, 0);
        } else {
            d->verticalSeperator->move(value.toInt() + d->popoverWidget->width(), 0);
        }
    });
    connect(popoverAnim, &tVariantAnimation::finished, [=] {
        d->blanker->hide();
        d->popoverWidget->hide();
        d->verticalSeperator->hide();

        d->parentWidget->removeEventFilter(this);
        d->parentWidget = nullptr;

        tPopoverPrivate::activePopovers.remove(d->popoverWidget);

        d->showing = false;
        emit dismissed();
    });
    popoverAnim->start();
}

bool tPopover::eventFilter(QObject* watched, QEvent* event) {
    if (watched == d->parentWidget) {
        if (event->type() == QEvent::Resize) {
            if (d->showing) {
                d->blanker->setGeometry(0, 0, d->parentWidget->width(), d->parentWidget->height());

                if (d->width == -1) {
                    d->popoverWidget->resize(d->parentWidget->width(), d->parentWidget->height());
                } else if (d->width < 0) {
                    if (isOpeningOnRight()) {
                        d->popoverWidget->setGeometry(-d->width, 0, d->parentWidget->width() + d->width, d->parentWidget->height());
                    } else {
                        d->popoverWidget->setGeometry(0, 0, d->parentWidget->width() - d->width, d->parentWidget->height());
                    }
                } else {
                    if (isOpeningOnRight()) {
                        d->popoverWidget->setGeometry(d->parentWidget->width() - d->width, 0, d->width, d->parentWidget->height());
                    } else {
                        d->popoverWidget->setGeometry(0, 0, d->width, d->parentWidget->height());
                    }
                }

                if (isOpeningOnRight()) {
                    d->verticalSeperator->setGeometry(d->popoverWidget->x() - 1, 0, 1, d->parentWidget->height());
                } else {
                    d->verticalSeperator->setGeometry(d->popoverWidget->geometry().right() + 1, 0, 1, d->parentWidget->height());
                }
            }
        }
    } else if (watched == d->blanker) {
        if (event->type() == QEvent::MouseButtonPress) {
            this->dismiss();
        }
    }
    return false;
}

tPopover* tPopover::popoverForWidget(QWidget *popoverWidget) {
    return tPopoverPrivate::activePopovers.value(popoverWidget, nullptr);
}
