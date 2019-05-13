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
#ifndef CSDSIZEGRIP_H
#define CSDSIZEGRIP_H

#include <QWidget>

class CsdSizeGrip : public QWidget
{
        Q_OBJECT
    public:
        explicit CsdSizeGrip(int side, QWidget *parent);

        void resizeGrip();
        static int borderWidth();

    signals:

    public slots:
        void csdsEnabledChanged(bool enabled);

    private:
        int side;
        QWidget* parentWidget;

        bool moving = false;
        QPoint startPoint;

        int hitTest(QPoint pos);

        void paintEvent(QPaintEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // CSDSIZEGRIP_H
