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
#ifndef TPAINTCALCULATOR_H
#define TPAINTCALCULATOR_H

#include "the-libs_global.h"
#include <QObject>
#include <QSharedDataPointer>

struct tPaintCalculatorPrivate;
class THELIBSSHARED_EXPORT tPaintCalculator {
    public:
        tPaintCalculator();
        tPaintCalculator(const tPaintCalculator& other);
        ~tPaintCalculator();

        typedef std::function<void(QRectF)> DrawFunction;

        void setPainter(QPainter* painter);
        void setDrawBounds(QRectF bounds);
        void setDrawBounds(QSizeF size);

        void addRect(QRectF rect, DrawFunction drawFunction);
        void addRect(QString name, QRectF rect, DrawFunction drawFunction);
        void performPaint();
        QRectF boundsOf(QString name) const;
        QRectF boundingRect() const;
        QRectF anchoredBoundingRect() const;
        QSizeF sizeWithMargins() const;

        Qt::LayoutDirection layoutDirection() const;
        void setLayoutDirection(Qt::LayoutDirection direction);

    private:
        tPaintCalculatorPrivate* d;
};

struct tPaintCalculatorScoperPrivate;
class THELIBSSHARED_EXPORT tPaintCalculatorScoper {
    public:
        tPaintCalculatorScoper(tPaintCalculator* paintCalculator);
        ~tPaintCalculatorScoper();

    private:
        tPaintCalculatorScoperPrivate* d;
};

#endif // TPAINTCALCULATOR_H
