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
#ifndef TPOPOVER_H
#define TPOPOVER_H

#include <QObject>
#include <the-libs_global.h>

struct tPopoverPrivate;
class THELIBSSHARED_EXPORT tPopover : public QObject {
        Q_OBJECT
    public:
        explicit tPopover(QWidget* popoverWidget, QObject* parent = nullptr);
        ~tPopover();

        enum PopoverSide {
            Leading,
            Trailing,
            Bottom
        };

        void setPopoverWidth(int width);
        void setPopoverSide(PopoverSide side);
        void setPerformBlanking(bool performBlanking);
        void setDismissable(bool dismissable);
        Q_DECL_DEPRECATED void setPerformBlur(bool performBlur); // Use tScrim::setBlurEnabled(bool) instead

        static tPopover* popoverForWidget(QWidget* popoverWidget);

        // Iterates through all of popoverWidget's children to find the tPopover
        static tPopover* popoverForPopoverWidget(QWidget* popoverWidget);

    signals:
        void dismissed();

    public slots:
        void show(QWidget* parent);
        void dismiss();

    private:
        tPopoverPrivate* d;

        bool eventFilter(QObject* watched, QEvent* event);
        bool isOpeningOnRight();
        void updateGeometry();
};

#endif // TPOPOVER_H
