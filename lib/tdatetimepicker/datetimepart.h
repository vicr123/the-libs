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
#ifndef DATETIMEPART_H
#define DATETIMEPART_H

#include <QLabel>

struct DateTimePartPrivate;
class DateTimePart : public QLabel
{
        Q_OBJECT
    public:
        explicit DateTimePart(QWidget *parent);
        ~DateTimePart();

        QSize sizeHint() const;

        void setValue(int value, bool animate = true);
        int value();

        void setMaxValue(int maxValue);
        void setValueType(QChar valueType);

    signals:
        void valueChanged(int value);

    public slots:
        void increment();
        void decrement();

    private:
        DateTimePartPrivate* d;

        void paintEvent(QPaintEvent *event);
        void enterEvent(QEvent* event);
        void leaveEvent(QEvent* event);
        void wheelEvent(QWheelEvent* event);
        void keyPressEvent(QKeyEvent *event);

        bool eventFilter(QObject* watched, QEvent* event);

        void calculateLeave();

        void incrementAnimation();
        void decrementAnimation();
};

#endif // DATETIMEPART_H
