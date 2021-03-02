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
#ifndef JOBDBUS_H
#define JOBDBUS_H

#include <QObject>

class tJob;
struct JobDbusPrivate;
class JobDbus : public QObject {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "com.vicr123.thelibs.tjob.Job")
    public:
        explicit JobDbus(QString path, tJob* job, QObject* parent = nullptr);
        ~JobDbus();

    public slots:
        Q_SCRIPTABLE quint64 Progress();
        Q_SCRIPTABLE quint64 TotalProgress();
        Q_SCRIPTABLE QString State();

    signals:
        Q_SCRIPTABLE void ProgressChanged(quint64 progress);
        Q_SCRIPTABLE void TotalProgressChanged(quint64 totalProgress);
        Q_SCRIPTABLE void StateChanged(QString state);

    private:
        JobDbusPrivate* d;
};

#endif // JOBDBUS_H
