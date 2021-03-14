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
#ifndef TSTYLEMANAGER_H
#define TSTYLEMANAGER_H

#include "tapplication.h"
#include <QObject>

struct tStyleManagerPrivate;
class THELIBSSHARED_EXPORT tStyleManager : public QObject {
        Q_OBJECT
    public:
        static tStyleManager* instance();

        enum Style {
            System,
            ContemporaryLight,
            ContemporaryDark
        };

        static void setOverrideStyleForApplication(Style style);
        static void setOverrideStyleOnPlatforms(tApplication::Platforms platforms);
        static bool isOverridingStyle();

    signals:

    private:
        explicit tStyleManager(QObject* parent = nullptr);
        tStyleManagerPrivate* d;

        void updateStyle();
};

#endif // TSTYLEMANAGER_H
