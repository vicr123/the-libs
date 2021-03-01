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
#include "desktopfile.h"

#include <QFile>
#include <QTextStream>

struct DesktopFilePrivate {
    QStringList lines;
    QStringList outputLines;
    int currentLine = -1;

    bool lineWritten = true;

    QString currentLineText();
};

DesktopFile::DesktopFile(QString file) {
    d = new DesktopFilePrivate();

    QFile f(file);
    f.open(QFile::ReadOnly);
    d->lines = QString(f.readAll()).split('\n');
}

DesktopFile::~DesktopFile() {
    delete d;
}

bool DesktopFile::nextLine() {
    if (!d->lineWritten) {
        //Copy this line verbatim to the output if it's not a translator's comment
        if (!d->currentLineText().startsWith("#T:")) d->outputLines.append(d->currentLineText());
    }

    d->currentLine++;
    d->lineWritten = false;
    return d->lines.count() > d->currentLine;
}

bool DesktopFile::isKeyValue() {
    return d->currentLineText().contains("=");
}

QString DesktopFile::key() {
    QString key = d->currentLineText().left(d->currentLineText().indexOf("="));
    if (key.endsWith("[]")) key.chop(2);
    return key;
}

QString DesktopFile::value() {
    return d->currentLineText().mid(d->currentLineText().indexOf("=") + 1);
}

QString DesktopFile::comment() {
    if (d->currentLine == 0) return "";
    QString previousLine = d->lines.value(d->currentLine - 1);
    if (!previousLine.startsWith("#T:")) return "";
    return previousLine.mid(3);
}

bool DesktopFile::isLineTranslatable() {
    if (!isKeyValue()) return false;
    return d->currentLineText().left(d->currentLineText().indexOf("=")).endsWith("[]");
}

void DesktopFile::insert(QString key, QString value, QString locale) {
    QString line = QStringLiteral("%1%2=%3").arg(key);
    if (locale.isEmpty()) {
        line = line.arg("");
    } else {
        line = line.arg(QStringLiteral("[%1]").arg(locale));
    }
    line = line.arg(value);
    d->outputLines.append(line);
    d->lineWritten = true;
}

QString DesktopFile::outputText() {
    return d->outputLines.join("\n");
}

QString DesktopFilePrivate::currentLineText() {
    return lines.at(currentLine);
}
