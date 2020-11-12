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
#ifndef TJOBMANAGER_H
#define TJOBMANAGER_H

#include "the-libs_global.h"
#include <QObject>

class tJob;
struct tJobManagerPrivate;
class THELIBSSHARED_EXPORT tJobManager : public QObject {
        Q_OBJECT
    public:
        ~tJobManager();

        static tJobManager* instance();

        static void trackJob(tJob* job);
        static void trackJobDelayed(tJob* job, quint64 delay = 1000);

        static QList<tJob*> jobs();

        static QWidget* makeJobButton();
        static void showJobsPopover(QWidget* parent);

    signals:
        void jobAdded(tJob* job);

    private:
        explicit tJobManager(QObject* parent = nullptr);
        tJobManagerPrivate* d;
};

#endif // TJOBMANAGER_H
