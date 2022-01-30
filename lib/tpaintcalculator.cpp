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
    QStringList namedRects;
    QMap<QString, QRectF> rects;
    QMap<QString, tPaintCalculator::DrawFunction> functions;
    Qt::LayoutDirection direction = Qt::LeftToRight;
    QRectF drawBounds = QRectF(0, 0, 0, 0);
    QPainter* painter = nullptr;
    quint16 generatedRectName = 0;
    QStringList boundsCalculationExcludeList;
};

struct tPaintCalculatorScoperPrivate {
    tPaintCalculator* paintCalculator;
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
    d->namedRects = other.d->namedRects;
    d->rects = other.d->rects;
    d->functions = other.d->functions;
    d->direction = other.d->direction;
    d->drawBounds = other.d->drawBounds;
    d->painter = other.d->painter;
    d->generatedRectName = other.d->generatedRectName;
    d->boundsCalculationExcludeList = other.d->boundsCalculationExcludeList;
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

void tPaintCalculator::setDrawBounds(QSizeF size) {
    d->drawBounds = QRectF(QPointF(0, 0), size);
}

void tPaintCalculator::addRect(QRectF rect, DrawFunction drawFunction) {
    QString name = QStringLiteral("PaintCalculatorGeneratedRect%1").arg(d->generatedRectName);
    d->generatedRectName++;
    this->addRect(name, rect, drawFunction);
}

void tPaintCalculator::addRect(QString name, QRectF rect, DrawFunction drawFunction) {
    d->namedRects.append(name);
    d->rects.insert(name, rect);
    d->functions.insert(name, drawFunction);
}

void tPaintCalculator::performPaint() {
    for (const QString& rectName : d->namedRects) {
        performPaint(rectName);
    }
}

void tPaintCalculator::performPaint(QString rectName) {
    if (!d->functions.contains(rectName)) return;

    if (d->painter) {
        d->painter->setLayoutDirection(d->direction);
    }

    d->functions.value(rectName)(this->boundsOf(rectName));
}

QRectF tPaintCalculator::boundsOf(QString name) const {
    QRectF newRect = d->rects.value(name);
    if (d->direction == Qt::RightToLeft) {
        //Flip the layout
        double left = newRect.left() - d->drawBounds.left();
        newRect.moveRight(d->drawBounds.right() - left);
    }
    return newRect;
}

QRectF tPaintCalculator::boundingRect() const {
    if (d->rects.isEmpty()) return QRectF();
    QRectF totalRect;
    for (const QString& rectName : d->rects.keys()) {
        if (d->boundsCalculationExcludeList.contains(rectName)) continue; //Exclude this from bounds calculations
        totalRect = totalRect.united(d->rects.value(rectName));
    }
    return totalRect;
}

QRectF tPaintCalculator::anchoredBoundingRect() const {
    QRectF bounding = this->boundingRect();
    bounding.setTop(0);
    bounding.setLeft(0);
    return bounding;
}

QRectF tPaintCalculator::visualBoundingRect() const {
    if (d->rects.isEmpty()) return QRectF();
    QRectF totalRect;
    for (const QString& rectName : d->rects.keys()) {
        if (d->boundsCalculationExcludeList.contains(rectName)) continue; //Exclude this from bounds calculations
        totalRect = totalRect.united(boundsOf(rectName));
    }
    return totalRect;
}

QSizeF tPaintCalculator::sizeWithMargins() const {
    return sizeWithMargins(QPointF(0, 0));
}

QSizeF tPaintCalculator::sizeWithMargins(QPointF origin) const {
    return boundingRect().size() + QSize(boundingRect().left() - origin.x(), boundingRect().top() - origin.y()) * 2;
}

QStringList tPaintCalculator::rectNames() const {
    return d->rects.keys();
}

void tPaintCalculator::setBoundsCalculationExcludeList(QStringList excludeList) {
    d->boundsCalculationExcludeList = excludeList;
}

QStringList tPaintCalculator::hitTest(QPointF point, bool includeDefaultNamed) const {
    QStringList hits;
    for (const QString& rectName : d->namedRects) {
        if (!includeDefaultNamed && rectName.startsWith(QStringLiteral("PaintCalculatorGeneratedRect"))) continue;
        if (this->boundsOf(rectName).contains(point)) hits.append(rectName);
    }
    return hits;
}

Qt::LayoutDirection tPaintCalculator::layoutDirection() const {
    return d->direction;
}

void tPaintCalculator::setLayoutDirection(Qt::LayoutDirection direction) {
    d->direction = direction;
}

tPaintCalculatorScoper::tPaintCalculatorScoper(tPaintCalculator* paintCalculator) {
    d = new tPaintCalculatorScoperPrivate();
    d->paintCalculator = paintCalculator;
}

tPaintCalculatorScoper::~tPaintCalculatorScoper() {
    d->paintCalculator->performPaint();
    delete d->paintCalculator;
    delete d;
}
