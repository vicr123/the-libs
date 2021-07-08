/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
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
#include "tpaintcalculator.h"

#include <QPainter>

struct tPaintCalculatorPrivate {
    QList<QPair<QRectF, tPaintCalculator::DrawFunction>> rects;
    Qt::LayoutDirection direction = Qt::LeftToRight;
    QRectF drawBounds = QRectF(0, 0, 0, 0);
    QPainter* painter = nullptr;
};

tPaintCalculator::tPaintCalculator() {
    d = new tPaintCalculatorPrivate();

    QGuiApplication* app = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
    if (app) {
        d->direction = app->layoutDirection();
    }
}

tPaintCalculator::tPaintCalculator(const tPaintCalculator& other) {
    d = new tPaintCalculatorPrivate();
    d->rects = other.d->rects;
    d->direction = other.d->direction;
    d->drawBounds = other.d->drawBounds;
    d->painter = other.d->painter;
}


tPaintCalculator::~tPaintCalculator() {
    delete d;
}

void tPaintCalculator::setPainter(QPainter* painter) {
    d->painter = painter;
}

void tPaintCalculator::setDrawBounds(QRectF bounds) {
    d->drawBounds = bounds;
}

void tPaintCalculator::addRect(QRectF rect, DrawFunction drawFunction) {
    d->rects.append({rect, drawFunction});
}

void tPaintCalculator::performPaint() {
    if (d->painter) {
        d->painter->setLayoutDirection(d->direction);
    }

    for (QPair<QRectF, tPaintCalculator::DrawFunction> rect : qAsConst(d->rects)) {
        QRectF newRect = rect.first;
        if (d->direction == Qt::RightToLeft) {
            //Flip the layout
            double left = newRect.left() - d->drawBounds.left();
            newRect.moveRight(d->drawBounds.right() - left);
        }
        rect.second(newRect);
    }
}

QRectF tPaintCalculator::boundingRect() {
    if (d->rects.isEmpty()) return QRectF();
    QRectF totalRect = d->rects.first().first;
    for (QPair<QRectF, tPaintCalculator::DrawFunction> rect : qAsConst(d->rects)) {
        totalRect = totalRect.united(rect.first);
    }
    return totalRect;
}

QRectF tPaintCalculator::anchoredBoundingRect() {
    QRectF bounding = this->boundingRect();
    bounding.setTop(0);
    bounding.setLeft(0);
    return bounding;
}

QSizeF tPaintCalculator::sizeWithMargins() {
    return boundingRect().size() + QSize(boundingRect().left(), boundingRect().top()) * 2;
}

Qt::LayoutDirection tPaintCalculator::layoutDirection() {
    return d->direction;
}

void tPaintCalculator::setLayoutDirection(Qt::LayoutDirection direction) {
    d->direction = direction;
}
