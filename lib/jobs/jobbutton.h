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
#ifndef JOBBUTTON_H
#define JOBBUTTON_H

#include <QToolButton>

class tJob;
struct JobButtonPrivate;
class JobButton : public QToolButton {
        Q_OBJECT
    public:
        explicit JobButton(QWidget* parent = nullptr);
        ~JobButton();

    signals:

    private:
        JobButtonPrivate* d;

        void trackJob(tJob* job);
        void updateJobs();

        void paintEvent(QPaintEvent* event);
};

#endif // JOBBUTTON_H
