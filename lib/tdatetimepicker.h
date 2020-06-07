/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
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
#ifndef TDATETIMEPICKER_H
#define TDATETIMEPICKER_H

#include <QWidget>
#include <QDateTime>
#include "the-libs_global.h"

struct tDateTimePickerPrivate;
class THELIBSSHARED_EXPORT tDateTimePicker : public QWidget {
        Q_OBJECT
    public:
        explicit tDateTimePicker(QWidget* parent = nullptr);
        explicit tDateTimePicker(QString format, QWidget* parent = nullptr);
        ~tDateTimePicker();

        enum PickOption {
            PickDate = 0x1,
            PickTime = 0x2
        };
        typedef QFlags<PickOption> PickOptions;

        QDateTime currentDateTime();
        void setDateTime(QDateTime dateTime);

        void setPickOptions(PickOptions options);
    signals:
        void dateTimeChanged(QDateTime dateTime);

    public slots:

    private:
        tDateTimePickerPrivate* d;

        void init(QString format);
};

#endif // TDATETIMEPICKER_H
