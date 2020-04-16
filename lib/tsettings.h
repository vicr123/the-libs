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
#ifndef TSETTINGS_H
#define TSETTINGS_H

#include <QObject>
#include "the-libs_global.h"

struct tSettingsPrivate;
class THELIBSSHARED_EXPORT tSettings : public QObject {
        Q_OBJECT
    public:
        explicit tSettings(QObject* parent = nullptr);
        ~tSettings();

        static void registerDefaults(QString filename);
        static void deregisterDefaults(QString filename);

        bool contains(QString key);
        void setValue(QString key, QVariant value);
        QVariant value(QString key);

        void setDelimitedList(QString key, QStringList value);
        QStringList delimitedList(QString key);

        QStringList childGroups();
        QStringList childKeys();

        void clear();
        void sync();

    signals:
        void settingChanged(QString key, QVariant value);

    private:
        static tSettingsPrivate* d;
};

#endif // TSETTINGS_H
