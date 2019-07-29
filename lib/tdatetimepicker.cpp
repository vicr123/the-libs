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
#include "tdatetimepicker.h"

#include <QDateTime>
#include <QBoxLayout>
#include "tdatetimepicker/datetimepart.h"

struct tDateTimePickerPrivate {
    QDateTime dateTime;

    QWidget* datePart;
    QWidget* timePart;

    QList<DateTimePart*> dateTimeParts;
    QLocale locale;

    int valueForPart(QChar part) {
        if (part == 'd') return dateTime.date().day();
        if (part == 'M') return dateTime.date().month();
        if (part == 'y') return dateTime.date().year();
        if (part == 'H') return dateTime.time().hour();
        if (part == 'm') return dateTime.time().minute();
        if (part == 's') return dateTime.time().second();
        if (part == 'a') return dateTime.time().hour() >= 12 ? 1 : 0;

        if (part == 'h') {
            int hour = dateTime.time().hour() % 12;
            if (hour == 0) hour = 12;
            return hour;
        }
        return 0;
    }
};

tDateTimePicker::tDateTimePicker(QWidget *parent) : QWidget(parent)
{
    d = new tDateTimePickerPrivate();

    d->datePart = new QWidget(this);
    d->timePart = new QWidget(this);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    layout->addStretch(0);
    layout->addWidget(d->datePart);
    layout->addWidget(d->timePart);
    layout->addStretch(0);
    this->setLayout(layout);

    QString dateFormat = d->locale.dateFormat(QLocale::ShortFormat);
    QString foundChars;
    for (QChar c : dateFormat) {
        if (!foundChars.contains(c) && QStringLiteral("dMy").contains(c)) {
            foundChars.append(c);
        }
    }
    foundChars.append("h:m:s a");

    QBoxLayout* dateLayout = new QBoxLayout(QBoxLayout::LeftToRight, d->datePart);
    QBoxLayout* timeLayout = new QBoxLayout(QBoxLayout::LeftToRight, d->timePart);
    for (QChar c : foundChars) {
        QLayout* layout;

        if (QStringLiteral("dMy").contains(c)) {
            layout = dateLayout;
        } else {
            layout = timeLayout;
        }

        QWidget* widgetToAdd;
        if (QStringLiteral("dMyhHmsa").contains(c)) {
            DateTimePart* part = new DateTimePart(this);
            part->setValueType(c);
            connect(this, &tDateTimePicker::dateTimeChanged, part, [=](QDateTime dateTime) {
                part->blockSignals(true);
                part->setValue(d->valueForPart(c));
                part->blockSignals(false);

                if (c == 'd') {
                    part->setMaxValue(dateTime.date().daysInMonth());
                }
            });
            connect(part, &DateTimePart::valueChanged, this, [=](int value) {
                QDate newDate = d->dateTime.date();
                QTime newTime = d->dateTime.time();
                switch (c.unicode()) {
                    case 'd':
                        newDate.setDate(newDate.year(), newDate.month(), value);
                        break;
                    case 'M': {
                        int day = newDate.day();
                        int daysInMonth = QDate(newDate.year(), value, 1).daysInMonth();
                        if (daysInMonth < day) day = daysInMonth;
                        newDate.setDate(newDate.year(), value, day);
                        break;
                    }
                    case 'y': {
                        int day = newDate.day();
                        int daysInMonth = QDate(value, newDate.month(), 1).daysInMonth();
                        if (daysInMonth < day) day = daysInMonth;
                        newDate.setDate(value, newDate.month(), day);
                        break;
                    }
                    case 'H':
                        newTime.setHMS(value, newTime.minute(), newTime.second());
                        break;
                    case 'm':
                        newTime.setHMS(newTime.hour(), value, newTime.second());
                        break;
                    case 's':
                        newTime.setHMS(newTime.hour(), newTime.minute(), value);
                        break;
                    case 'a': {
                        int baseHour = newTime.hour() % 12;
                        if (value == 1) {
                            //PM
                            newTime.setHMS(baseHour + 12, newTime.minute(), newTime.second());
                        } else {
                            //AM
                            newTime.setHMS(baseHour, newTime.minute(), newTime.second());
                        }
                        break;
                    }
                    case 'h':
                        if (newTime.hour() / 12 == 1) {
                            //PM
                            newTime.setHMS(value + 12, newTime.minute(), newTime.second());
                        } else {
                            //AM
                            newTime.setHMS(value, newTime.minute(), newTime.second());
                        }
                        break;
                }
                this->setDateTime(QDateTime(newDate, newTime));
            });
            d->dateTimeParts.append(part);
            widgetToAdd = part;
        } else {
            QLabel* label = new QLabel(parent);
            label->setText(c);
            widgetToAdd = label;
        }

        layout->addWidget(widgetToAdd);
    }
    d->datePart->setLayout(dateLayout);
    d->timePart->setLayout(timeLayout);

    this->setDateTime(QDateTime::currentDateTime());
}

tDateTimePicker::~tDateTimePicker() {
    delete d;
}

void tDateTimePicker::setDateTime(QDateTime dateTime) {
    d->dateTime = dateTime;
    emit dateTimeChanged(d->dateTime);
}

QDateTime tDateTimePicker::currentDateTime() {
    return d->dateTime;
}

void tDateTimePicker::setPickOptions(PickOptions options) {
    d->datePart->setVisible(options & PickDate);
    d->timePart->setVisible(options & PickTime);
}
