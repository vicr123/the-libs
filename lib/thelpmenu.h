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
#ifndef THELPMENU_H
#define THELPMENU_H

#include "the-libs_global.h"
#include <QMenu>

struct tHelpMenuPrivate;
class THELIBSSHARED_EXPORT tHelpMenu : public QMenu {
        Q_OBJECT
    public:
        explicit tHelpMenu(QWidget* parent);
        ~tHelpMenu();

        void addAction(QAction* action);

    signals:

    private:
        tHelpMenuPrivate* d;
};

#endif // THELPMENU_H
