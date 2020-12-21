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
#ifndef TSCRIM_H
#define TSCRIM_H

#include "the-libs_global.h"
#include <QWidget>

struct tScrimPrivate;
class THELIBSSHARED_EXPORT tScrim : public QWidget {
        Q_OBJECT
    public:
        explicit tScrim(QWidget* parent = nullptr);
        ~tScrim();

        static tScrim* scrimForWidget(QWidget* widget);

        void addIgnoreWidget(QWidget* ignoreWidget);
        void setBlurEnabled(bool blurEnabled);

        void show();
        void hide();

    signals:
        void scrimClicked();

    private:
        tScrimPrivate* d;

        void updateGeometry();

        // QWidget interface
    protected:
        void paintEvent(QPaintEvent* event);
        bool eventFilter(QObject* watched, QEvent* event);
        void mousePressEvent(QMouseEvent* event);
        void mouseReleaseEvent(QMouseEvent* event);
};

#endif // TSCRIM_H
