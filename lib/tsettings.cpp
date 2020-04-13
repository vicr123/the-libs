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
#include "tsettings.h"

#include <QSettings>
#include <QDebug>

struct tSettingsPrivate {
    tSettingsPrivate() {
        allSettings.append(QSharedPointer<QSettings>(new QSettings()));
        settingsPaths.append("");
    }

    QList<tSettings*> availableSettingsObjects;

    QList<QSharedPointer<QSettings>> allSettings;
    QList<QString> settingsPaths;
};

tSettingsPrivate* tSettings::d = nullptr;

tSettings::tSettings(QObject* parent) : QObject(parent) {
    if (d == nullptr) d = new tSettingsPrivate();
    d->availableSettingsObjects.append(this);
}

tSettings::~tSettings() {
    d->availableSettingsObjects.removeOne(this);
}

void tSettings::registerDefaults(QString filename) {
    if (d == nullptr) d = new tSettingsPrivate();
    if (!d->settingsPaths.contains(filename)) {
        d->settingsPaths.append(filename);
        d->allSettings.append(QSharedPointer<QSettings>(new QSettings(filename, QSettings::IniFormat)));
    }
}

void tSettings::deregisterDefaults(QString filename) {
    if (d == nullptr) d = new tSettingsPrivate();
    if (d->settingsPaths.contains(filename)) {
        d->allSettings.removeAt(d->settingsPaths.lastIndexOf(filename));
        d->settingsPaths.removeOne(filename);
    }
}

bool tSettings::contains(QString key) {
    return d->allSettings.at(0)->contains(key);
}

void tSettings::setValue(QString key, QVariant value) {
    d->allSettings.at(0)->setValue(key, value);
    emit settingChanged(key, value);
}

QVariant tSettings::value(QString key) {
    for (QSharedPointer<QSettings> settings : d->allSettings) {
        if (settings->contains(key)) return settings->value(key);
    }

    qWarning() << "No setting value available for key:" << key;
    return QVariant();
}

QStringList tSettings::childGroups() {
    QSet<QString> groups;
    for (QSharedPointer<QSettings> settings : d->allSettings) {
        groups.unite(QSet<QString>(settings->childGroups().begin(), settings->childGroups().end()));
    }
    return groups.values();
}

QStringList tSettings::childKeys() {
    QSet<QString> groups;
    for (QSharedPointer<QSettings> settings : d->allSettings) {
        groups.unite(QSet<QString>(settings->childKeys().begin(), settings->childKeys().end()));
    }
    return groups.values();
}
