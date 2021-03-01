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
#include "jsonfile.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

struct JsonFilePrivate {
    uint refcount = 0;
    QJsonObject rootObject;
};

JsonFile::JsonFile() {
    d = new JsonFilePrivate();
    d->refcount++;
}

JsonFile::JsonFile(QString file) {
    d = new JsonFilePrivate();
    d->refcount++;

    QFile f(file);
    f.open(QFile::ReadOnly);
    d->rootObject = QJsonDocument::fromJson(f.readAll()).object();
}

JsonFile::JsonFile(const JsonFile& other) {
    this->d = other.d;
    d->refcount++;
}

JsonFile::~JsonFile() {
    d->refcount--;
    if (d->refcount == 0) delete d;
}

void JsonFile::insertString(QString key, QString message, QString description) {
    QJsonObject stringObject;
    stringObject.insert("message", message);
    if (!description.isEmpty()) stringObject.insert("description", description);
    d->rootObject.insert(key, stringObject);
}

QString JsonFile::message(QString key) const {
    QJsonValue stringValue = d->rootObject.value(key);
    if (!stringValue.isObject()) return "";

    return stringValue.toObject().value("message").toString();
}

QByteArray JsonFile::output() {
    return QJsonDocument(d->rootObject).toJson();
}

JsonFile& JsonFile::operator =(const JsonFile& other) {
    d->refcount--;
    if (d->refcount == 0) delete d;

    d = other.d;
    d->refcount++;
    return *this;
}
