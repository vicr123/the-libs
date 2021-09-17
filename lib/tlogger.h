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
#ifndef TLOGGER_H
#define TLOGGER_H

#include "the-libs_global.h"
#include <QObject>
#include <QDateTime>
#include <QDebug>

struct tLoggerPrivate;
struct tLogWriterPrivate;
class THELIBSSHARED_EXPORT tLogger : public QObject {
        Q_OBJECT
    public:
        ~tLogger();

        struct LogItem {
            QDateTime timestamp;
            QString text;
            QString context;
            QString file;
            int line;
            QString function;
            QtMsgType severity;
        };

        static tLogger* instance();

        static void log(QtMsgType severity, QString context, QString text, QString file = "", int line = -1, QString function = "");
        static void log(LogItem item);
        static QList<LogItem> logItems();
        static void clearLog();
        static void openDebugLogWindow();

        static QString severityToString(QtMsgType severity);

    signals:
        void newLogItem(LogItem item);
        void logCleared();

    private:
        explicit tLogger(QObject* parent = nullptr);

        void appendToLog(QString logItem);

        tLoggerPrivate* d;
};

class THELIBSSHARED_EXPORT tLogWriter {
    public:
        tLogWriter(QString context, QtMsgType severity, QString file, int line, QString function);
        ~tLogWriter();

        tLogWriter& operator<<(const char* str);
        tLogWriter& operator<<(QString str);
        tLogWriter& operator<<(QStringList str);
        tLogWriter& operator<<(int str);
        tLogWriter& operator<<(uint str);
        tLogWriter& operator<<(qint64 str);
        tLogWriter& operator<<(quint64 str);
        tLogWriter& operator<<(float str);
        tLogWriter& operator<<(double str);
        tLogWriter& operator<<(bool str);

    private:
        tLogWriter(const tLogWriter& other);
        tLogWriterPrivate* d;
};

#define tDebug(context) (tLogWriter(context, QtDebugMsg, static_cast<const char*>(__FILE__), __LINE__, static_cast<const char*>(Q_FUNC_INFO)))
#define tInfo(context) (tLogWriter(context, QtInfoMsg, static_cast<const char*>(__FILE__), __LINE__, static_cast<const char*>(Q_FUNC_INFO)))
#define tWarn(context) (tLogWriter(context, QtWarningMsg, static_cast<const char*>(__FILE__), __LINE__, static_cast<const char*>(Q_FUNC_INFO)))
#define tCritical(context) (tLogWriter(context, QtCriticalMsg, static_cast<const char*>(__FILE__), __LINE__, static_cast<const char*>(Q_FUNC_INFO)))

Q_DECLARE_METATYPE(tLogger::LogItem);

#endif // TDEBUG_H
