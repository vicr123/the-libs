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
#include "datetimepartbutton.h"

#include <QPainter>

DateTimePartButton::DateTimePartButton(QWidget *parent) : QPushButton(parent)
{

}

void DateTimePartButton::paintEvent(QPaintEvent *event) {
    QPushButton::paintEvent(event);

    //Draw borders
    QPainter painter(this);
    painter.setPen(this->palette().color(QPalette::WindowText));
    painter.drawLine(0, 0, 0, this->height());
    painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height());

    if (isTopSide) {
        painter.drawLine(0, 0, this->width(), 0);
    } else {
        painter.drawLine(0, this->height() - 1, this->width(), this->height() - 1);
    }
}

void DateTimePartButton::setIsTopSide(bool isTopSide) {
    this->isTopSide = isTopSide;
}
