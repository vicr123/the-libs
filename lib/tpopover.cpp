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
#include <QGraphicsBlurEffect>
#include <QPainter>
#include <QTimer>
#include "tpropertyanimation.h"
#include "tcsdtools.h"

#ifdef Q_OS_MAC
    #include <QDialog>
    #include <QBoxLayout>
#endif

struct tPopoverPrivate {
    QWidget* popoverWidget;
    QWidget* parentWidget;
    QFrame* verticalSeperator;

#ifdef Q_OS_MAC
    QDialog* wrapperDialog;
#endif

    QWidget* blanker;
//    QGraphicsOpacityEffect* blankerEffect;
    QGraphicsBlurEffect* blurEffect;
    float blankerOpacity = 1;

    tPopover::PopoverSide side = tPopover::Trailing;

    int width = -1;
    bool showing = false;
    bool performBlanking = true;
    bool dismissable = true;

    const int blankerOverscan = 100;

    static QMap<QWidget*, tPopover*> activePopovers;
};
QMap<QWidget*, tPopover*> tPopoverPrivate::activePopovers = QMap<QWidget*, tPopover*>();

tPopover::tPopover(QWidget* popoverWidget, QObject* parent) : QObject(parent) {
    d = new tPopoverPrivate();
    d->popoverWidget = popoverWidget;
    popoverWidget->setAutoFillBackground(true);

    d->blanker = new QWidget();
    d->blanker->setAutoFillBackground(true);
    d->blanker->installEventFilter(this);

//    d->blankerEffect = new QGraphicsOpacityEffect();
//    d->blankerEffect->setOpacity(0);
//    d->blanker->setGraphicsEffect(d->blankerEffect);

    d->blurEffect = new QGraphicsBlurEffect();
    d->blurEffect->setBlurHints(QGraphicsBlurEffect::AnimationHint);
    d->blanker->setGraphicsEffect(d->blurEffect);

    d->verticalSeperator = new QFrame();
}

tPopover::~tPopover() {
    tPopoverPrivate::activePopovers.remove(d->popoverWidget);
    if (d->blanker) d->blanker->deleteLater();
    delete d;
}

bool tPopover::isOpeningOnRight() {
    return (QApplication::layoutDirection() == Qt::LeftToRight && d->side == Trailing) ||
        (QApplication::layoutDirection() == Qt::RightToLeft && d->side == Leading);
}

void tPopover::updateGeometry() {
    d->blanker->setGeometry(-d->blankerOverscan, -d->blankerOverscan, d->parentWidget->width() + d->blankerOverscan * 2, d->parentWidget->height() + d->blankerOverscan * 2);

    if (d->width == -1) {
        d->popoverWidget->resize(d->parentWidget->width(), d->parentWidget->height());
    } else if (d->width < 0) {
        if (d->side == Bottom) {
            d->popoverWidget->setGeometry(0, -d->width, d->parentWidget->width(), d->parentWidget->height() + d->width);
        } else if (isOpeningOnRight()) {
            d->popoverWidget->setGeometry(-d->width, 0, d->parentWidget->width() + d->width, d->parentWidget->height());
        } else {
            d->popoverWidget->setGeometry(0, 0, d->parentWidget->width() - d->width, d->parentWidget->height());
        }
    } else {
        if (d->side == Bottom) {
            d->popoverWidget->setGeometry(0, d->parentWidget->height() - d->width, d->parentWidget->width(), d->width);
        } else if (isOpeningOnRight()) {
            d->popoverWidget->setGeometry(d->parentWidget->width() - d->width, 0, d->width, d->parentWidget->height());
        } else {
            d->popoverWidget->setGeometry(0, 0, d->width, d->parentWidget->height());
        }
    }

    if (d->side == Bottom) {
        d->verticalSeperator->setGeometry(0, d->popoverWidget->y() - 1, d->parentWidget->width(), 1);
    } else if (isOpeningOnRight()) {
        d->verticalSeperator->setGeometry(d->popoverWidget->x() - 1, 0, 1, d->parentWidget->height());
    } else {
        d->verticalSeperator->setGeometry(d->popoverWidget->geometry().right() + 1, 0, 1, d->parentWidget->height());
    }
}

void tPopover::setPopoverWidth(int width) {
    if (d->showing) {
        int endValue;

        if (width >= 0) {
            //Normal width
            endValue = width;
        } else if (width == -1) {
            //Full Screen
            endValue = d->parentWidget->width();
        } else {
            //Based on parent width
            //Add because width will be negative
            endValue = d->parentWidget->width() + width;
        }

        tVariantAnimation* animation = new tVariantAnimation(this);
        animation->setStartValue(d->popoverWidget->width());
        animation->setEndValue(endValue);
        animation->setDuration(250);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        connect(animation, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
            d->width = value.toInt();
            this->updateGeometry();
        });
        connect(animation, &tVariantAnimation::finished, this, [ = ] {
            d->width = width;
            this->updateGeometry();
            animation->deleteLater();
        });
        animation->start();
    } else {
        d->width = width;
    }
}

void tPopover::setPopoverSide(PopoverSide side) {
    d->side = side;
}

void tPopover::setPerformBlanking(bool performBlanking) {
    d->performBlanking = performBlanking;
}

void tPopover::setPerformBlur(bool performBlur) {
    d->blurEffect->setEnabled(performBlur);
}

void tPopover::setDismissable(bool dismissable) {
    d->dismissable = dismissable;
}

void tPopover::show(QWidget* parent) {
    parent = tCsdTools::widgetForPopover(parent);
    if (d->showing) return;
    tPopoverPrivate::activePopovers.insert(d->popoverWidget, this);

#ifdef Q_OS_MAC
    //Just show the popover as a sheet

    d->wrapperDialog = new QDialog(parent);
    d->wrapperDialog->resize(d->popoverWidget->size());

    QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setMargin(0);
    l->addWidget(d->popoverWidget);
    d->wrapperDialog->setLayout(l);

    d->wrapperDialog->setWindowModality(Qt::ApplicationModal); //Keyboard focus issues if this is window modal
    d->wrapperDialog->setWindowFlag(Qt::Sheet);
    d->wrapperDialog->open();
#else
    d->popoverWidget->setWindowFlags(Qt::Widget);
    d->parentWidget = parent;
    d->parentWidget->installEventFilter(this);

    d->blanker->hide();
    d->verticalSeperator->hide();
    d->popoverWidget->hide();

    d->blanker->setParent(parent);
    d->popoverWidget->setParent(parent);
    d->verticalSeperator->setParent(parent);

    if (d->side == Bottom) {
        d->verticalSeperator->resize(parent->width(), 1);
        d->verticalSeperator->setFrameShape(QFrame::HLine);
    } else {
        d->verticalSeperator->resize(1, parent->height());
        d->verticalSeperator->setFrameShape(QFrame::VLine);
    }

    d->blanker->setGeometry(-d->blankerOverscan, -d->blankerOverscan, parent->width() + d->blankerOverscan * 2, parent->height() + d->blankerOverscan * 2);
    if (d->width == -1) {
        d->popoverWidget->resize(parent->width(), parent->height());
    } else {
        if (d->side == Bottom) {
            if (d->width < 0) {
                d->popoverWidget->resize(parent->width(), parent->height() + d->width);
            } else {
                d->popoverWidget->resize(parent->width(), d->width);
            }
        } else {
            if (d->width < 0) {
                d->popoverWidget->resize(parent->width() + d->width, parent->height());
            } else {
                d->popoverWidget->resize(d->width, parent->height());
            }
        }
    }

    if (d->performBlanking) {
        tVariantAnimation* blankerAnim = new tVariantAnimation();
        blankerAnim->setStartValue(d->blurEffect->blurRadius());
        blankerAnim->setEndValue((qreal) 10);
        blankerAnim->setDuration(400);
        blankerAnim->setEasingCurve(QEasingCurve::OutCubic);
        connect(blankerAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
            d->blurEffect->setBlurRadius(value.toReal());
            d->blankerOpacity = 1 - value.toReal() / 10.0 * 0.75;
            d->blanker->update();

            //Constantly change the size of the blanker because Qt doesn't seem to blur correctly otherwise
            if (d->blanker->geometry().size().height() == parent->height() + d->blankerOverscan * 2) {
                d->blanker->resize(d->blanker->width(), parent->height() + d->blankerOverscan * 2 + 1);
            } else {
                d->blanker->resize(d->blanker->width(), parent->height() + d->blankerOverscan * 2);
            }
        });
        connect(blankerAnim, &tPropertyAnimation::finished, blankerAnim, &tPropertyAnimation::deleteLater);
        blankerAnim->start();
    }

    tVariantAnimation* popoverAnim = new tVariantAnimation();
    if (d->side == Bottom) {
        popoverAnim->setStartValue(parent->height());
        popoverAnim->setEndValue(parent->height() - d->popoverWidget->height());
    } else if (isOpeningOnRight()) {
        popoverAnim->setStartValue(parent->width());
        popoverAnim->setEndValue(parent->width() - d->popoverWidget->width());
    } else {
        popoverAnim->setStartValue(-d->popoverWidget->width());
        popoverAnim->setEndValue(0);
    }
    popoverAnim->setDuration(250);
    popoverAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(popoverAnim, &tVariantAnimation::valueChanged, [ = ](QVariant value) {
        if (d->side == Bottom) {
            d->popoverWidget->move(0, value.toInt());
            d->verticalSeperator->move(0, value.toInt() - 1);
        } else if (isOpeningOnRight()) {
            d->popoverWidget->move(value.toInt(), 0);
            d->verticalSeperator->move(value.toInt() - 1, 0);
        } else {
            d->popoverWidget->move(value.toInt(), 0);
            d->verticalSeperator->move(value.toInt() + d->popoverWidget->width(), 0);
        }
    });
    connect(popoverAnim, &tVariantAnimation::finished, popoverAnim, &tVariantAnimation::deleteLater);
    popoverAnim->start();
    popoverAnim->valueChanged(popoverAnim->startValue());

    d->blanker->show();
    d->verticalSeperator->show();
    d->popoverWidget->show();

    d->blanker->raise();
    d->verticalSeperator->raise();
    d->popoverWidget->raise();

    d->popoverWidget->setFocus();
#endif

    d->showing = true;
}

void tPopover::dismiss() {
    if (!d->showing) return;

#ifdef Q_OS_MAC
    //d->popoverWidget->hide();
    d->wrapperDialog->close();
    d->wrapperDialog->deleteLater();
    d->showing = false;
    emit dismissed();
#else
    if (d->performBlanking) {
        tVariantAnimation* blankerAnim = new tVariantAnimation();
        blankerAnim->setStartValue(d->blurEffect->blurRadius());
        blankerAnim->setEndValue((qreal) 0);
        blankerAnim->setDuration(250);
        blankerAnim->setEasingCurve(QEasingCurve::OutCubic);
        connect(blankerAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
            d->blurEffect->setBlurRadius(value.toReal());
            d->blankerOpacity = 1 - value.toReal() / 10.0 * 0.75;
            d->blanker->update();

            //Constantly change the size of the blanker because Qt doesn't seem to blur correctly otherwise
            if (d->blanker->geometry().size().height() == d->parentWidget->height() + d->blankerOverscan * 2) {
                d->blanker->resize(d->blanker->width(), d->parentWidget->height() + d->blankerOverscan * 2 + 1);
            } else {
                d->blanker->resize(d->blanker->width(), d->parentWidget->height() + d->blankerOverscan * 2);
            }
        });
        connect(blankerAnim, &tPropertyAnimation::finished, blankerAnim, &tPropertyAnimation::deleteLater);
        blankerAnim->start();
    }

    tVariantAnimation* popoverAnim = new tVariantAnimation();
    if (d->side == Bottom) {
        //Opening on the bottom
        popoverAnim->setStartValue(d->popoverWidget->y());
        popoverAnim->setEndValue(d->parentWidget->height());
    } else if (isOpeningOnRight()) {
        //Opening on the right
        popoverAnim->setStartValue(d->popoverWidget->x());
        popoverAnim->setEndValue(d->parentWidget->width());
    } else {
        //Opening on the left
        popoverAnim->setStartValue(d->popoverWidget->x());
        popoverAnim->setEndValue(-d->parentWidget->width());
    }
    popoverAnim->setDuration(250);
    popoverAnim->setEasingCurve(QEasingCurve::OutCubic);
    connect(popoverAnim, &tVariantAnimation::valueChanged, [ = ](QVariant value) {
        if (d->side == Bottom) {
            d->popoverWidget->move(0, value.toInt());
            d->verticalSeperator->move(0, value.toInt() - 1);
        } else if (isOpeningOnRight()) {
            d->popoverWidget->move(value.toInt(), 0);
            d->verticalSeperator->move(value.toInt() - 1, 0);
        } else {
            d->popoverWidget->move(value.toInt(), 0);
            d->verticalSeperator->move(value.toInt() + d->popoverWidget->width(), 0);
        }
    });
    connect(popoverAnim, &tVariantAnimation::finished, [ = ] {
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
#endif
}

bool tPopover::eventFilter(QObject* watched, QEvent* event) {
    if (watched == d->parentWidget) {
        if (event->type() == QEvent::Resize) {
            if (d->showing) {
                updateGeometry();
            }
        }
    } else if (watched == d->blanker) {
        if (event->type() == QEvent::MouseButtonPress && d->dismissable) {
            this->dismiss();
        } else if (event->type() == QEvent::Paint) {
            QPixmap image(d->parentWidget->width(), d->parentWidget->height());
            image.fill(d->parentWidget->palette().color(QPalette::Window));

            QPainter imagePainter(&image);
            QObjectList children = d->parentWidget->children();
            for (QObject* child : children) {
                if (child == d->blanker || child == d->popoverWidget) continue;
                if (QWidget* childWidget = qobject_cast<QWidget*>(child)) {
                    childWidget->render(&imagePainter, childWidget->geometry().topLeft(), QRegion(), QWidget::DrawChildren);
                }
            }
            imagePainter.end();

            QPainter p(d->blanker);
//            p.setPen(Qt::transparent);
//            p.setBrush(d->blanker->palette().color(QPalette::Window));
//            p.drawRect(0, 0, d->blanker->width(), d->blanker->height());
            p.setOpacity(d->blankerOpacity);
            p.drawPixmap(QRect(QPoint(d->blankerOverscan, d->blankerOverscan), image.size()), image);

//            QColor col = d->blanker->palette().color(QPalette::Window);
//            col.setAlpha(127);
//            p.setBrush(col);
//            p.drawRect(0, 0, d->blanker->width(), d->blanker->height());
        }
    }
    return false;
}

tPopover* tPopover::popoverForWidget(QWidget* popoverWidget) {
    return tPopoverPrivate::activePopovers.value(popoverWidget, nullptr);
}

tPopover* tPopover::popoverForPopoverWidget(QWidget* popoverWidget) {
    QWidget* checkWidget = popoverWidget;
    while (checkWidget) {
        for (tPopover* popover : tPopoverPrivate::activePopovers.values()) {
            if (popover->d->popoverWidget == checkWidget) return popover;
        }
        checkWidget = checkWidget->parentWidget();
    }
    return nullptr;
}
