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
#ifndef TDOCUMENTTABBER_H
#define TDOCUMENTTABBER_H

#include <QWidget>
#include "the-libs_global.h"

namespace Ui {
    class tDocumentTabber;
}

class tStackedWidget;
struct tDocumentTabberPrivate;
class THELIBSSHARED_EXPORT tDocumentTabber : public QWidget {
        Q_OBJECT

    public:
        explicit tDocumentTabber(QWidget* parent = nullptr);
        ~tDocumentTabber();

        void setStackedWidget(tStackedWidget* stackedWidget);

    private:
        Ui::tDocumentTabber* ui;
        tDocumentTabberPrivate* d;

        void updateTabs();
};

class THELIBSSHARED_EXPORT tDocumentTabberDocument : public QWidget {
        Q_OBJECT

    public:
        explicit tDocumentTabberDocument(QWidget* parent = nullptr);

        virtual QString tabTitle() = 0;

    signals:
        void tabTitleChanged(QString tabTitle);
};

#endif // TDOCUMENTTABBER_H
