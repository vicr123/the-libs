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
#include "tlogger.h"

#include <QTextStream>
#include <QMutex>
#include <QDebug>
#include "private/debuglogwindow.h"

struct tLoggerPrivate {
    QMutex logMutex;
    QList<tLogger::LogItem> logs;
};

struct tLogWriterPrivate {
    tLogger::LogItem templ;
    QStringList bits;
};

tLogger::tLogger(QObject* parent) : QObject(parent) {
    d = new tLoggerPrivate();
    d->logs.reserve(10000);
}

void tLogger::appendToLog(QString logItem) {

}

tLogger::~tLogger() {
    delete d;
}

tLogger* tLogger::instance() {
    static tLogger* logger = new tLogger();
    return logger;
}

void tLogger::log(QtMsgType severity, QString context, QString text, QString file, int line, QString function) {
    LogItem item;
    item.severity = severity;
    item.context = context;
    item.text = text;
    item.file = file;
    item.line = line;
    item.function = function;

    tLogger::log(item);

    QString qlogEntry = item.text;

    QTextStream(stderr) << qlogEntry;
}

void tLogger::log(tLogger::LogItem item) {
    item.timestamp = QDateTime::currentDateTime();

    instance()->d->logMutex.lock();
    instance()->d->logs.append(item);
    while (instance()->d->logs.count() > 10000) {
        instance()->d->logs.takeFirst();
    }
    instance()->d->logMutex.unlock();

    emit instance()->newLogItem(item);
}

QList<tLogger::LogItem> tLogger::logItems() {
    QMutexLocker locker(&instance()->d->logMutex);
    return instance()->d->logs;
}

void tLogger::clearLog() {
    instance()->d->logMutex.lock();
    instance()->d->logs.clear();
    instance()->d->logMutex.unlock();

    emit instance()->logCleared();
}

void tLogger::openDebugLogWindow() {
    DebugLogWindow* window = new DebugLogWindow();
    connect(window, &DebugLogWindow::finished, window, &QDialog::deleteLater);
    window->show();
}

QString tLogger::severityToString(QtMsgType severity) {
    switch (severity) {
        case QtDebugMsg:
            return tr("Debug", "Severity Level");
        case QtWarningMsg:
            return tr("Warning", "Severity Level");
        case QtCriticalMsg:
            return tr("Critical", "Severity Level");
        case QtFatalMsg:
            return tr("Fatal", "Severity Level");
        case QtInfoMsg:
            return tr("Info", "Severity Level");
    }
    return "";
}


tLogWriter::tLogWriter(QString context, QtMsgType severity, QString file, int line, QString function) {
    d = new tLogWriterPrivate();
    d->templ.context = context;
    d->templ.severity = severity;
    d->templ.file = file;
    d->templ.line = line;
    d->templ.function = function;
}

tLogWriter::~tLogWriter() {
    d->templ.text = d->bits.join(" ");
    tLogger::log(d->templ);

    QTextStream output(stderr);

    QString severity;
    switch (d->templ.severity) {
        case QtDebugMsg:
            severity = "[ D ]";
            break;
        case QtWarningMsg:
            severity = "[ ! ]";
            break;
        case QtCriticalMsg:
            severity = "[! !]";
            break;
        case QtFatalMsg:
            severity = "[!!!]";
            break;
        case QtInfoMsg:
            severity = "[ i ]";
    }

    output << QStringLiteral("%1 %2 %3\n").arg(QDateTime::currentDateTime().toString("[hh:mm:ss]"), severity, d->templ.text);

    delete d;
}

tLogWriter& tLogWriter::operator<<(const char* str) {
    d->bits.append(QString::fromLocal8Bit(str).trimmed());
    return *this;
}

tLogWriter& tLogWriter::operator<<(QString str) {
    d->bits.append(str.trimmed());
    return *this;
}

tLogWriter& tLogWriter::operator<<(QStringList str) {
    d->bits.append("[" + str.join(",") + "]");
    return *this;
}

tLogWriter& tLogWriter::operator<<(int str) {
    d->bits.append(QStringLiteral("%1").arg(str));
    return *this;
}

tLogWriter& tLogWriter::operator<<(uint str) {
    d->bits.append(QStringLiteral("%1").arg(str));
    return *this;
}

tLogWriter& tLogWriter::operator<<(qint64 str) {
    d->bits.append(QStringLiteral("%1").arg(str));
    return *this;
}

tLogWriter& tLogWriter::operator<<(quint64 str) {
    d->bits.append(QStringLiteral("%1").arg(str));
    return *this;
}

tLogWriter& tLogWriter::operator<<(float str) {
    d->bits.append(QStringLiteral("%1").arg(str));
    return *this;
}

tLogWriter& tLogWriter::operator<<(double str) {
    d->bits.append(QStringLiteral("%1").arg(str));
    return *this;
}

tLogWriter& tLogWriter::operator<<(bool str) {
    d->bits.append(str ? "true" : "false");
    return *this;
}

tLogWriter::tLogWriter(const tLogWriter& other) {
    d = new tLogWriterPrivate();
    d->templ = other.d->templ;
    d->bits = other.d->bits;
}
