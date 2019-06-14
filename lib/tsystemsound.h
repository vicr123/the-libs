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
#ifndef TSYSTEMSOUND_H
#define TSYSTEMSOUND_H

#include <QObject>
#include "the-libs_global.h"

class THELIBSSHARED_EXPORT tSystemSound : public QObject
{
        Q_OBJECT
    public:
        static QString soundLocation(QString soundName);
        static bool isSoundEnabled(QString soundName);
        static void setSoundEnabled(QString soundName, bool isEnabled);

    signals:
        void done();

    public slots:
        static tSystemSound* play(QString soundName, qreal volume = 1);

    private:
        explicit tSystemSound(QObject *parent = nullptr);
};

#endif // TSYSTEMSOUND_H
