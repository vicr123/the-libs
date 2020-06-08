/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
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
#include "tconditionalwidget.h"

#include <tvariantanimation.h>

struct tConditionalWidgetPrivate {
    bool isExpanded = false;
    bool fullyExpanded = false;

    tVariantAnimation* heightAnim;
};

tConditionalWidget::tConditionalWidget(QWidget* parent) : QWidget(parent) {
    d = new tConditionalWidgetPrivate();

    d->heightAnim = new tVariantAnimation(this);
    d->heightAnim->setEasingCurve(QEasingCurve::OutCubic);
    d->heightAnim->setDuration(250);
    connect(d->heightAnim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        this->setFixedHeight(value.toInt());
    });
    this->setFixedHeight(0);
//    connect(d->heightAnim, &tVariantAnimation::finished, this, &tConditionalWidget::updateGeometry);
}

tConditionalWidget::~tConditionalWidget() {
    delete d;
}

void tConditionalWidget::expand() {
    if (d->isExpanded) return;

    if (!this->isVisible()) {
        d->isExpanded = true;
        d->fullyExpanded = true;
        this->setFixedHeight(QWIDGETSIZE_MAX);
        return;
    }

    d->heightAnim->stop();
    d->heightAnim->setStartValue(this->height());
    d->heightAnim->setEndValue(QWidget::sizeHint().height());

    QMetaObject::Connection* c = new QMetaObject::Connection();
    *c = connect(d->heightAnim, &tVariantAnimation::stateChanged, this, [ = ](tVariantAnimation::State newState, tVariantAnimation::State oldState) {
        if (newState == tVariantAnimation::Running) return;
        disconnect(*c);
        delete c;
        d->fullyExpanded = true;
        this->setFixedHeight(QWIDGETSIZE_MAX);
    });
    d->heightAnim->start();

    d->isExpanded = true;
}

void tConditionalWidget::collapse() {
    if (!d->fullyExpanded) return;

    if (!this->isVisible()) {
        d->isExpanded = false;
        d->fullyExpanded = false;
        this->setFixedHeight(0);
        return;
    }

    d->heightAnim->stop();
    d->heightAnim->setStartValue(this->height());
    d->heightAnim->setEndValue(0);

    QMetaObject::Connection* c = new QMetaObject::Connection();
    *c = connect(d->heightAnim, &tVariantAnimation::stateChanged, this, [ = ](tVariantAnimation::State newState, tVariantAnimation::State oldState) {
        if (newState == tVariantAnimation::Running) return;
        disconnect(*c);
        delete c;
        d->isExpanded = false;
        this->setFixedHeight(0);
    });
    d->heightAnim->start();
    d->fullyExpanded = false;
}

void tConditionalWidget::setExpanded(bool expanded) {
    if (expanded) {
        this->expand();
    } else {
        this->collapse();
    }
}

