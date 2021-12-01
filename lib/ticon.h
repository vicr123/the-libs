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
#ifndef TICON_H
#define TICON_H

#include "the-libs_global.h"
#include <QObject>
#include <QIcon>
#include <QLocale>

class THELIBSSHARED_EXPORT tIcon : public QObject {
        Q_OBJECT
    public:
        static QIcon fromTheme(QString iconName);
        static QIcon fromTheme(QString iconName, QIcon fallback);
        static QIcon fromTheme(QLocale locale, QString iconName);
        static QIcon fromTheme(QLocale locale, QString iconName, QIcon fallback);

        static void processWidget(QWidget* widget);
        static void processWidget(QWidget* widget, QLocale locale);

        static void processWidgetLater(QWidget* widget);
        static void processWidgetLater(QWidget* widget, QLocale locale);

    signals:

    private:
        explicit tIcon(QObject* parent = nullptr);

        static QStringList preferredIconNames(QString iconName);
        static QStringList preferredIconNames(QString iconName, QLocale locale);
};

#endif // TICON_H
