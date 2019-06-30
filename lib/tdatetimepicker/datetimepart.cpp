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
#include "datetimepart.h"

#include <QPainter>
#include <QLocale>
#include <QPushButton>
#include <QWheelEvent>
#include "tvariantanimation.h"
#include "datetimepartbutton.h"

struct DateTimePartPrivate {
    int value;
    int drawOffset = 0;
    QChar valueType = 'm';
    int sizeWidth;
    int currentWheelDelta = 0;

    int maxValueUser = -1;

    DateTimePartButton *upButton, *downButton;

    QLocale locale;

    QString textForValue(int value) {
        switch (valueType.unicode()) {
            case 'y':
                return QString::number(value);
            case 'd':
            case 'h':
            case 'H':
            case 'm':
            case 's':
                return locale.toString(value).rightJustified(2, locale.zeroDigit());
            case 'M':
                return locale.monthName(value, QLocale::ShortFormat);
            case 'a':
                if (value == 0) {
                    return locale.amText();
                } else {
                    return locale.pmText();
                }
        }

        return "(invalid)";
    }

    int minValue() {
        switch (valueType.unicode()) {
            case 'y':
                return 1980;
            case 'd':
            case 'M':
            case 'h':
                return 1;
            case 'H':
            case 'm':
            case 's':
            case 'a':
                return 0;
        }
        return 0;
    }

    int maxValue() {
        if (maxValueUser != -1) return maxValueUser;
        switch (valueType.unicode()) {
            case 'y':
                return 2099;
            case 'd':
                return 30; //TODO: set depending on month
            case 'h':
                return 12;
            case 'H':
                return 23;
            case 'm':
            case 's':
                return 59;
            case 'M':
                return 12;
            case 'a':
                return 1;
        }
        return 0;
    }

    bool wrapAroundValue() {
        switch (valueType.unicode()) {
            case 'h':
            case 'H':
            case 'm':
            case 's':
            case 'a':
                return true;
        }
        return false;
    }

    QString altText;
    bool performOppositeAnimation = false;
};

DateTimePart::DateTimePart(QWidget *parent) : QLabel(parent)
{
    d = new DateTimePartPrivate();

    this->setMouseTracking(true);
    this->setMargin(3);
    this->setFocusPolicy(Qt::StrongFocus);

    d->upButton = new DateTimePartButton(parent->parentWidget());
    d->upButton->setIsTopSide(true);
    d->upButton->setIcon(QIcon::fromTheme("go-up"));
    d->upButton->setFlat(true);
    d->upButton->setVisible(false);
    d->upButton->setAutoRepeat(true);
    d->upButton->installEventFilter(this);
    d->upButton->setFocusProxy(this);
    connect(d->upButton, &QPushButton::clicked, this, &DateTimePart::increment);

    d->downButton = new DateTimePartButton(parent->parentWidget());
    d->downButton->setIsTopSide(false);
    d->downButton->setIcon(QIcon::fromTheme("go-down"));
    d->downButton->setFlat(true);
    d->downButton->setVisible(false);
    d->downButton->setAutoRepeat(true);
    d->downButton->installEventFilter(this);
    d->downButton->setFocusProxy(this);
    connect(d->downButton, &QPushButton::clicked, this, &DateTimePart::decrement);
}

DateTimePart::~DateTimePart() {
    d->upButton->deleteLater();
    d->downButton->deleteLater();
    delete d;
}

void DateTimePart::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setFont(this->font());
    painter.setPen(this->palette().color(QPalette::WindowText));

    painter.drawText(QRect(0, d->drawOffset - this->height(), this->width(), this->height()), Qt::AlignCenter, d->altText);
    painter.drawText(QRect(0, d->drawOffset, this->width(), this->height()), Qt::AlignCenter, this->text());
    painter.drawText(QRect(0, d->drawOffset + this->height(), this->width(), this->height()), Qt::AlignCenter, d->altText);

    if (d->upButton->isVisible()) {
        //Draw borders
        painter.drawLine(0, 0, 0, this->height());
        painter.drawLine(this->width() - 1, 0, this->width() - 1, this->height());
    }
}

void DateTimePart::setValue(int value, bool animate) {
    int oldValue = d->value;
    d->altText = this->text();

    d->value = value;

    emit valueChanged(value);

    if (animate) {
        if (oldValue < value) {
            if (d->performOppositeAnimation) {
                decrementAnimation();
            } else {
                incrementAnimation();
            }
        } else if (oldValue > value) {
            if (d->performOppositeAnimation) {
                incrementAnimation();
            } else {
                decrementAnimation();
            }
        }
        d->performOppositeAnimation = false;
    }

    this->setText(d->textForValue(value));
}

int DateTimePart::value() {
    return d->value;
}

void DateTimePart::setValueType(QChar valueType) {
    d->valueType = valueType;

    //Find the correct width
    int maxWidth = 0;
    for (int i = d->minValue(); i <= d->maxValue(); i++) {
        maxWidth = qMax(maxWidth, this->fontMetrics().width(d->textForValue(i)));
    }
    d->sizeWidth = maxWidth + 6; //margin

    this->updateGeometry();
}

void DateTimePart::enterEvent(QEvent *event) {
    int buttonHeight = SC_DPI(32);
    QSize buttonSize = QSize(this->width(), buttonHeight);
    QPoint topLeft = d->upButton->parentWidget()->mapFromGlobal(this->mapToGlobal(QPoint(0, 0)));

    d->upButton->setGeometry(QRect(topLeft - QPoint(0, buttonHeight), buttonSize));
    d->downButton->setGeometry(QRect(topLeft + QPoint(0, this->height()), buttonSize));

    d->upButton->setVisible(true);
    d->downButton->setVisible(true);

    d->upButton->raise();
    d->downButton->raise();

    this->update();
}

void DateTimePart::leaveEvent(QEvent *event) {
    calculateLeave();
}

bool DateTimePart::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::Leave) {
        calculateLeave();
    }
    return false;
}

void DateTimePart::calculateLeave() {
    QPoint pos = d->upButton->parentWidget()->mapFromGlobal(QCursor::pos());
    if (!d->upButton->geometry().adjusted(0, 0, 0, 3).contains(pos) &&
        !d->downButton->geometry().adjusted(0, -3, 0, 0).contains(pos) &&
        !this->geometry().contains(this->parentWidget()->mapFromGlobal(QCursor::pos()))) {
        d->upButton->setVisible(false);
        d->downButton->setVisible(false);

        this->update();
    }
}

void DateTimePart::increment() {
    int val = d->value + 1;
    if (d->maxValue() < val) {
        if (d->wrapAroundValue()) {
            val = d->minValue();
            d->performOppositeAnimation = true;
        } else {
            return; //Don't do anything
        }
    }
    this->setValue(val);
}

void DateTimePart::decrement() {
    int val = d->value - 1;
    if (d->minValue() > val) {
        if (d->wrapAroundValue()) {
            val = d->maxValue();
            d->performOppositeAnimation = true;
        } else {
            return; //Don't do anything
        }
    }
    this->setValue(val);
}

void DateTimePart::incrementAnimation() {
    tVariantAnimation* anim = new tVariantAnimation();
    anim->setStartValue(this->height());
    anim->setEndValue(0);
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        d->drawOffset = value.toInt();
        this->update();
    });
    connect(anim, &tVariantAnimation::finished, this, [=] {
        anim->deleteLater();
    });
    anim->start();
}

void DateTimePart::decrementAnimation() {
    tVariantAnimation* anim = new tVariantAnimation();
    anim->setStartValue(-this->height());
    anim->setEndValue(0);
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [=](QVariant value) {
        d->drawOffset = value.toInt();
        this->update();
    });
    connect(anim, &tVariantAnimation::finished, this, [=] {
        anim->deleteLater();
    });
    anim->start();
}

QSize DateTimePart::sizeHint() const {
    QSize sz = QLabel::sizeHint();
    sz.setWidth(d->sizeWidth);
    return sz;
}

void DateTimePart::wheelEvent(QWheelEvent *event) {
    event->accept();
    d->currentWheelDelta += event->angleDelta().y();

    while (d->currentWheelDelta > 60) {
        increment();
        d->currentWheelDelta -= 120;
    }
    while (d->currentWheelDelta < -60) {
        decrement();
        d->currentWheelDelta += 120;
    }
}

void DateTimePart::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up) {
        increment();
    } else if (event->key() == Qt::Key_Down) {
        decrement();
    }
}

void DateTimePart::setMaxValue(int maxValue) {
    d->maxValueUser = maxValue;
    if (maxValue < d->value) {
        setValue(maxValue);
    }
}
