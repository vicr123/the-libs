/****************************************
 *
 *   the-libs - Libraries for the* Apps
 *   Copyright (C) 2018 Victor Tran
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

#include "tstackedwidget.h"

class tStackedWidgetPrivate {
    public:
        tStackedWidgetPrivate(tStackedWidget* parent) {
            this->parent = parent;
        }

        bool doingNewAnimation = false;

        QSettings settings;

        tStackedWidget::Animation anim = tStackedWidget::None;
        QAbstractAnimation* currentAnimation = nullptr;

        tStackedWidget* parent;
};

tStackedWidget::tStackedWidget(QWidget* parent) : QStackedWidget(parent) {
    d = new tStackedWidgetPrivate(this);
}

tStackedWidget::~tStackedWidget() {
    delete d;
}

void tStackedWidget::setCurrentIndex(int index, bool doAnimation) {
    if (doAnimation) {
        //Do some checks before setting the current index.
        if (currentIndex() != index && !d->doingNewAnimation) {
            doSetCurrentIndex(index);
        }
    } else {
        QStackedWidget::setCurrentIndex(index);
        d->doingNewAnimation = false;

        emit switchingFrame(index);
    }
}

void tStackedWidget::doSetCurrentIndex(int index) {
    //Check if Power Stretch is on or if animations are disabled
    if (theLibsGlobal::instance()->powerStretchEnabled() || !theLibsGlobal::instance()->allowSystemAnimations()) {
        //Forego animations; power stretch is on
        QStackedWidget::setCurrentIndex(index);
    } else {
        //Forcibly set the current index.
        QWidget* currentWidget = widget(currentIndex());
        QWidget* nextWidget = widget(index);
        if (nextWidget == nullptr) {
            QStackedWidget::setCurrentIndex(index);
        } else {
            if (d->currentAnimation != nullptr) {
                d->currentAnimation->stop();
            }

            switch (d->anim) {
                case None: {
                    QStackedWidget::setCurrentIndex(index);
                    break;
                }
                case SlideHorizontal: {
                    QRect newGeometry;
                    if ((currentIndex() < index && !QApplication::isRightToLeft()) || (currentIndex() > index && QApplication::isRightToLeft())) {
                        nextWidget->setGeometry(this->width(), 0, this->width(), this->height());
                        newGeometry.setRect(-this->width() / 8, 0, this->width(), this->height());
                    } else {
                        nextWidget->setGeometry(-this->width(), 0, this->width(), this->height());
                        newGeometry.setRect(this->width() / 8, 0, this->width(), this->height());
                    }

                    nextWidget->show();
                    nextWidget->raise();

                    QParallelAnimationGroup* group = new QParallelAnimationGroup;
                    tPropertyAnimation* animation = new tPropertyAnimation(nextWidget, "geometry");
                    animation->setStartValue(nextWidget->geometry());
                    animation->setEndValue(QRect(0, 0, this->width(), this->height()));
                    animation->setEasingCurve(QEasingCurve::OutCubic);
                    animation->setDuration(250);
                    connect(this, &tStackedWidget::resized, animation, [ = ] {
                        animation->setEndValue(QRect(0, 0, this->width(), this->height()));
                    });
                    group->addAnimation(animation);

                    tPropertyAnimation* animation2 = new tPropertyAnimation(currentWidget, "geometry");
                    animation2->setStartValue(currentWidget->geometry());
                    animation2->setEndValue(newGeometry);
                    animation2->setEasingCurve(QEasingCurve::OutCubic);
                    animation2->setDuration(250);
                    group->addAnimation(animation2);

                    connect(group, &QParallelAnimationGroup::finished, [ = ]() {
                        QStackedWidget::setCurrentIndex(index);
                        d->doingNewAnimation = false;
                        group->deleteLater();
                        d->currentAnimation = nullptr;
                    });
                    group->start();
                    d->currentAnimation = group;
                    break;
                }
                case SlideVertical: {
                    QRect newGeometry;
                    if (currentIndex() < index) {
                        nextWidget->setGeometry(0, this->height(), this->width(), this->height());
                        newGeometry.setRect(0, -this->height() / 8, this->width(), this->height());
                    } else {
                        nextWidget->setGeometry(0, -this->height(), this->width(), this->height());
                        newGeometry.setRect(0, this->height() / 8, this->width(), this->height());
                    }

                    nextWidget->show();
                    nextWidget->raise();

                    QParallelAnimationGroup* group = new QParallelAnimationGroup;
                    tPropertyAnimation* animation = new tPropertyAnimation(nextWidget, "geometry");
                    animation->setStartValue(nextWidget->geometry());
                    animation->setEndValue(QRect(0, 0, this->width(), this->height()));
                    animation->setEasingCurve(QEasingCurve::OutCubic);
                    animation->setDuration(250);
                    connect(this, &tStackedWidget::resized, animation, [ = ] {
                        animation->setEndValue(QRect(0, 0, this->width(), this->height()));
                    });
                    group->addAnimation(animation);

                    tPropertyAnimation* animation2 = new tPropertyAnimation(currentWidget, "geometry");
                    animation2->setStartValue(currentWidget->geometry());
                    animation2->setEndValue(newGeometry);
                    animation2->setEasingCurve(QEasingCurve::OutCubic);
                    animation2->setDuration(250);
                    group->addAnimation(animation2);

                    connect(group, &QParallelAnimationGroup::finished, [ = ]() {
                        QStackedWidget::setCurrentIndex(index);
                        d->doingNewAnimation = false;
                        group->deleteLater();
                        d->currentAnimation = nullptr;
                    });
                    group->start();
                    d->currentAnimation = group;
                    break;
                }
                case Fade: {
                    nextWidget->show();
                    nextWidget->raise();
                    nextWidget->resize(this->width(), this->height());

                    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
                    effect->setOpacity(0);
                    nextWidget->setGraphicsEffect(effect);

                    tPropertyAnimation* anim = new tPropertyAnimation(effect, "opacity");
                    anim->setStartValue((float) 0);
                    anim->setEndValue((float) 1);
                    anim->setEasingCurve(QEasingCurve::OutCubic);
                    anim->setDuration(250);
                    connect(anim, &tPropertyAnimation::finished, [ = ] {
                        QStackedWidget::setCurrentIndex(index);
                        anim->deleteLater();
                        effect->deleteLater();
                        d->currentAnimation = nullptr;
                    });
                    anim->start();
                    d->currentAnimation = anim;
                    break;
                }
                case Lift: {
                    //QRect newGeometry;
                    //if (currentIndex() < index) {
                    nextWidget->setGeometry(0, this->height() / 8, this->width(), this->height());
                    //newGeometry.setRect(0, -this->height() / 8, this->width(), this->height());
                    //} else {
                    //    nextWidget->setGeometry(0, -this->height(), this->width(), this->height());
                    //    newGeometry.setRect(0, this->height() / 8, this->width(), this->height());
                    //}

                    nextWidget->show();
                    nextWidget->raise();

                    QParallelAnimationGroup* group = new QParallelAnimationGroup;
                    tPropertyAnimation* animation = new tPropertyAnimation(nextWidget, "geometry");
                    animation->setStartValue(nextWidget->geometry());
                    animation->setEndValue(QRect(0, 0, this->width(), this->height()));
                    animation->setEasingCurve(QEasingCurve::OutCubic);
                    animation->setDuration(250);
                    connect(this, &tStackedWidget::resized, animation, [ = ] {
                        animation->setEndValue(QRect(0, 0, this->width(), this->height()));
                    });
                    group->addAnimation(animation);

                    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
                    effect->setOpacity(0);
                    nextWidget->setGraphicsEffect(effect);

                    tPropertyAnimation* anim = new tPropertyAnimation(effect, "opacity");
                    anim->setStartValue((float) 0);
                    anim->setEndValue((float) 1);
                    anim->setEasingCurve(QEasingCurve::OutCubic);
                    anim->setDuration(250);
                    group->addAnimation(anim);

                    connect(group, &QParallelAnimationGroup::finished, [ = ]() {
                        QStackedWidget::setCurrentIndex(index);
                        d->doingNewAnimation = false;
                        group->deleteLater();
                        effect->deleteLater();
                        d->currentAnimation = nullptr;
                    });
                    group->start();
                    d->currentAnimation = group;
                    break;
                }
            }
        }
    }
    emit switchingFrame(index);
}

void tStackedWidget::setCurrentWidget(QWidget* w, bool doAnimation) {
    this->setCurrentIndex(indexOf(w), doAnimation);
}

tStackedWidget::Animation tStackedWidget::CurrentAnimation() {
    return d->anim;
}

void tStackedWidget::setCurrentAnimation(Animation animation) {
    d->anim = animation;
    emit CurrentAnimationChanged(animation);
}

int tStackedWidget::addWidget(QWidget* w) {
    w->setAutoFillBackground(true);
    int index = QStackedWidget::addWidget(w);
    emit widgetAdded();
    return index;
}

int tStackedWidget::insertWidget(int index, QWidget* w) {
    w->setAutoFillBackground(true);
    int indexReturn = QStackedWidget::insertWidget(index, w);
    emit widgetAdded();
    return indexReturn;
}

void tStackedWidget::resizeEvent(QResizeEvent* event) {
    emit resized();
}
