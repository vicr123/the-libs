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
#include <QFileSystemWatcher>
#include "tlogger.h"
#include <QTimer>

typedef QPair<QString, QString> SettingIdentifier;

struct tSettingsGlobals {
    void initialiseInitialSettings(SettingIdentifier identifier) {

        if (allSettings.count(identifier) == 0) {
            QSharedPointer<QSettings> settings(new QSettings(identifier.first, identifier.second));
            allSettings.insert(identifier, settings);
            settingsPaths.insert(identifier, "");

#ifndef Q_OS_WIN
            //Set up the watcher once the event loop has started
            QTimer::singleShot(0, [ = ] {
                QFileSystemWatcher* watcher = new QFileSystemWatcher();
                watcher->moveToThread(qApp->thread());
                QObject::connect(watcher, &QFileSystemWatcher::fileChanged, [ = ] {
                    if (!watcher->files().contains(settings->fileName())) {
                        watcher->addPath(settings->fileName());
                    }
                    this->notifyChanges(identifier);
                });
                watcher->addPath(settings->fileName());
                watchers.insert(identifier, watcher);
                notifyChanges(identifier);
            });
#endif

            //Register settings from environment variables
            QString defaults = qEnvironmentVariable("THELIBS_TSETTINGS_DEFAULT_FILES");
            if (!defaults.isEmpty()) {
                for (QString defaultDefinition : defaults.split(":")) {
                    QStringList definition = defaultDefinition.split(";");
                    if (definition.count() != 3) {
                        tDebug("tSettings") << "THELIBS_TSETTINGS_DEFAULT_FILES: defaults definition invalid";
                        continue;
                    }

                    if (definition.at(0) == identifier.first && definition.at(1) == identifier.second) {
                        tSettings::registerDefaults(identifier.first, identifier.second, definition.at(2));
                    }
                }
            }

            tDebug("tSettings") << "Writing settings for " << identifier.second << "to" << settings->fileName();
        }
    }

    SettingIdentifier defaultIdentifier() {
        SettingIdentifier identifier;
        identifier.first = qApp->organizationName();
        identifier.second = qApp->applicationName();
        return identifier;
    }

    void notifyChanges(SettingIdentifier identifier) {
        QVariantMap values;
        tSettings settings(identifier.first, identifier.second);
        settings.sync();

        for (QString key : settings.allKeys()) {
            values.insert(key, settings.value(key));
        }

        if (currentValues.contains(identifier)) {
            //Go through and find the differences
            QVariantMap differences;

            QVariantMap oldValues = currentValues.value(identifier);
            for (QString key : values.keys()) {
                if (!oldValues.contains(key) || oldValues.value(key) != values.value(key)) {
                    differences.insert(key, values.value(key));
                }
            }

            for (QString key : oldValues.keys()) {
                if (!values.contains(key)) {
                    differences.insert(key, QVariant());
                }
            }

            //Tell everyone about the differences
            for (auto i = differences.constBegin(); i != differences.constEnd(); i++) {
                for (tSettings* settingsObjects : availableSettingsObjects.values(identifier)) {
                    emit settingsObjects->settingChanged(i.key(), i.value());
                }
            }
        }
        currentValues.insert(identifier, values);
    }

    QMultiMap<SettingIdentifier, tSettings*> availableSettingsObjects;

    QMultiMap<SettingIdentifier, QSharedPointer<QSettings>> allSettings;
    QMultiMap<SettingIdentifier, QString> settingsPaths;
    QMap<SettingIdentifier, QFileSystemWatcher*> watchers;
    QMap<SettingIdentifier, QVariantMap> currentValues;
};

tSettingsGlobals* tSettings::g = nullptr;

struct tSettingsPrivate {
    SettingIdentifier identifier;
};

tSettings::tSettings(QObject* parent) : QObject(parent) {
    if (g == nullptr) g = new tSettingsGlobals();
    d = new tSettingsPrivate;
    d->identifier = g->defaultIdentifier();

    g->initialiseInitialSettings(d->identifier);

    g->availableSettingsObjects.insert(d->identifier, this);
}

tSettings::tSettings(QString applicationName, QObject* parent) : QObject(parent) {
    if (g == nullptr) g = new tSettingsGlobals();
    d = new tSettingsPrivate;
    d->identifier.first = qApp->organizationName();
    d->identifier.second = applicationName;

    g->initialiseInitialSettings(d->identifier);

    g->availableSettingsObjects.insert(d->identifier, this);
}

tSettings::tSettings(QString organisationName, QString applicationName, QObject* parent) : QObject(parent) {
    if (g == nullptr) g = new tSettingsGlobals();
    d = new tSettingsPrivate;
    d->identifier.first = organisationName;
    d->identifier.second = applicationName;

    g->initialiseInitialSettings(d->identifier);

    g->availableSettingsObjects.insert(d->identifier, this);
}

tSettings::~tSettings() {
    g->availableSettingsObjects.remove(d->identifier, this);
    delete d;
}

void tSettings::registerDefaults(QString filename) {
    if (g == nullptr) g = new tSettingsGlobals();
    registerDefaults(g->defaultIdentifier().first, g->defaultIdentifier().second, filename);
}

void tSettings::registerDefaults(QString applicationName, QString filename) {
    if (g == nullptr) g = new tSettingsGlobals();
    registerDefaults(g->defaultIdentifier().first, applicationName, filename);
}

void tSettings::registerDefaults(QString organisationName, QString applicationName, QString filename) {
    if (g == nullptr) g = new tSettingsGlobals();
    SettingIdentifier identifier;
    identifier.first = organisationName;
    identifier.second = applicationName;

    g->initialiseInitialSettings(identifier);

    if (!g->settingsPaths.values(identifier).contains(filename)) {
        g->settingsPaths.insert(identifier, filename);
        g->allSettings.insert(identifier, QSharedPointer<QSettings>(new QSettings(filename, QSettings::IniFormat)));
        tDebug("tSettings") << "Registered defaults for" << identifier.first << identifier.second << filename;
    }
}

void tSettings::deregisterDefaults(QString filename) {
    if (g == nullptr) g = new tSettingsGlobals();
    deregisterDefaults(g->defaultIdentifier().first, g->defaultIdentifier().second, filename);
}

void tSettings::deregisterDefaults(QString applicationName, QString filename) {
    if (g == nullptr) g = new tSettingsGlobals();
    deregisterDefaults(g->defaultIdentifier().first, applicationName, filename);
}

void tSettings::deregisterDefaults(QString organisationName, QString applicationName, QString filename) {
    if (g == nullptr) g = new tSettingsGlobals();
    SettingIdentifier identifier;
    identifier.first = organisationName;
    identifier.second = applicationName;
    if (g->settingsPaths.values(identifier).contains(filename)) {
        g->allSettings.remove(identifier, g->allSettings.values(g->defaultIdentifier()).at(g->settingsPaths.values(g->defaultIdentifier()).lastIndexOf(filename)));
        g->settingsPaths.remove(identifier, filename);
    }
}

bool tSettings::contains(QString key) {
    return g->allSettings.values(d->identifier).last()->contains(key);
}

void tSettings::setValue(QString key, QVariant value) {
    g->allSettings.values(d->identifier).last()->setValue(key, value);

    //Don't handle it here on not Windows; we'll detect when the file changes
#ifdef Q_OS_WIN
    for (tSettings* settingsObjects : g->availableSettingsObjects.values(d->identifier)) {
        emit settingsObjects->settingChanged(key, value);
    }
#endif
}

QVariant tSettings::value(QString key) {
    //Go through the list of settings in reverse order (least recent to most recent)
    QList<QSharedPointer<QSettings>> settings = g->allSettings.values(d->identifier);
    for (auto i = settings.rbegin(); i != settings.rend(); i++) {
        if (i->data()->contains(key)) return i->data()->value(key);
        if (i->data()->status() != QSettings::NoError) {
            switch (i->data()->status()) {
                case QSettings::AccessError:
                    tWarn("tSettings") << "Access error:" << i->data()->fileName();
                    break;
                case QSettings::FormatError:
                    tWarn("tSettings") << "Format error:" << i->data()->fileName();
                    break;
                default:
                    break;
            }
        }
    }

    tWarn("tSettings") << "No setting value available for key:" << key;
    return QVariant();
}

void tSettings::setDelimitedList(QString key, QStringList value) {
    if (value.count() == 0) {
        this->setValue(key, "/");
    } else {
        this->setValue(key, value.join(":"));
    }
}

QStringList tSettings::delimitedList(QString key) {
    QString value = this->value(key).toString();
    if (value == "/") return QStringList();
    return value.split(":");
}

QStringList tSettings::childGroups() {
    QSet<QString> groups;
    for (QSharedPointer<QSettings> settings : g->allSettings.values(d->identifier)) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        groups.unite(settings->childGroups().toSet());
#else
        groups.unite(QSet<QString>(settings->childGroups().begin(), settings->childGroups().end()));
#endif
    }
    return groups.values();
}

QStringList tSettings::childKeys() {
    QSet<QString> groups;
    for (QSharedPointer<QSettings> settings : g->allSettings.values(d->identifier)) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        groups.unite(settings->childKeys().toSet());
#else
        groups.unite(QSet<QString>(settings->childKeys().begin(), settings->childKeys().end()));
#endif
    }
    return groups.values();
}

QStringList tSettings::keysInGroup(QString group) {
    QSet<QString> keys;
    for (QSharedPointer<QSettings> settings : g->allSettings.values(d->identifier)) {
        settings->beginGroup(group);
        QStringList childKeys = settings->childKeys();
        settings->endGroup();

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        keys.unite(childKeys.toSet());
#else
        keys.unite(QSet<QString>(childKeys.begin(), childKeys.end()));
#endif
    }
    return keys.values();
}

QStringList tSettings::allKeys() {
    QSet<QString> keys;
    for (QSharedPointer<QSettings> settings : g->allSettings.values(d->identifier)) {
        QStringList allKeys = settings->allKeys();

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        keys.unite(allKeys.toSet());
#else
        keys.unite(QSet<QString>(allKeys.begin(), allKeys.end()));
#endif
    }
    return keys.values();
}

void tSettings::clear() {
    g->allSettings.values(d->identifier).last()->clear();
}

void tSettings::sync() {
    g->allSettings.values(d->identifier).last()->sync();
}
