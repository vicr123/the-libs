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
#ifndef TNOTIFICATIONLINUX_H
#define TNOTIFICATIONLINUX_H

#include <QObject>
class tNotificationLinuxHelper : public QObject {
        Q_OBJECT
    public:
        tNotificationLinuxHelper(QObject* parent = nullptr);
        ~tNotificationLinuxHelper();

        void setListeningId(uint id);
        void dismiss();

    Q_SIGNALS:
        void didClick(QString key);
        void didDismiss();

    public Q_SLOTS:
        void actionClicked(uint id, QString key);
        void dismissed(uint id);

    private:
        uint currentId = 0;
};

#endif // TNOTIFICATIONLINUX_H
