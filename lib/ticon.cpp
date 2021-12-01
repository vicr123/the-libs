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
#include "ticon.h"
#include "tapplication.h"

#include <QWidget>
#include <QAbstractButton>
#include <QTimer>

QIcon tIcon::fromTheme(QString iconName) {
    return fromTheme(iconName, QIcon());
}

QIcon tIcon::fromTheme(QString iconName, QIcon fallback) {
    return fromTheme(QLocale(), iconName, fallback);
}

QIcon tIcon::fromTheme(QLocale locale, QString iconName) {
    return fromTheme(locale, iconName, QIcon());
}

QIcon tIcon::fromTheme(QLocale locale, QString iconName, QIcon fallback) {
    QStringList preferred = preferredIconNames(iconName, locale);
    for (const QString& preferredIconName : qAsConst(preferred)) {
        if (QIcon::hasThemeIcon(preferredIconName)) {
            return QIcon::fromTheme(preferredIconName);
        }
    }
    return fallback;
}

void tIcon::processWidget(QWidget* widget) {
    processWidget(widget, QLocale());
}

void tIcon::processWidget(QWidget* widget, QLocale locale) {
    QList<QWidget*> widgets = widget->findChildren<QWidget*>();
    for (QWidget* widget : qAsConst(widgets)) {
        if (QAbstractButton* button = qobject_cast<QAbstractButton*>(widget)) {
            if (!button->icon().name().isEmpty()) button->setIcon(fromTheme(locale, button->icon().name()));
        }
    }
}

void tIcon::processWidgetLater(QWidget* widget) {
    processWidgetLater(widget, QLocale());
}

void tIcon::processWidgetLater(QWidget* widget, QLocale locale) {
    QTimer::singleShot(0, widget, [ = ] {
        processWidget(widget, locale);
    });
}

tIcon::tIcon(QObject* parent) : QObject(parent) {

}

QStringList tIcon::preferredIconNames(QString iconName) {
    return preferredIconNames(iconName, QLocale());
}

QStringList tIcon::preferredIconNames(QString iconName, QLocale locale) {
    QStringList preferred;
    for (QString language : locale.uiLanguages()) {
        preferred.append(iconName + "-" + language.toLower());
        preferred.append(iconName + "-" + language);
    }
    if (locale.textDirection() == Qt::RightToLeft || QApplication::layoutDirection() == Qt::RightToLeft) preferred.append(iconName + "-rtl");
    preferred.append(iconName);
    return preferred;
}
