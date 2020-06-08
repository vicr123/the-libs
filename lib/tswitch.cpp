/****************************************
 *
 *   the-libs - Libraries for the* Apps
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

#include "tswitch.h"
#include <tvariantanimation.h>
#include <QPaintEvent>
#include <QPainter>
#include <QStaticText>

tSwitch::tSwitch(QWidget* parent) : QPushButton(parent) {
    this->setCheckable(true);
    this->setFixedSize(this->sizeHint());

    innerRect = QRect(0, 0, this->height(), this->width());
}

QPalette::ColorGroup tSwitch::IsActiveColorRole() {
    if (this->isEnabled()) {
        return QPalette::Active;
    } else {
        return QPalette::Disabled;
    }
}

void tSwitch::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    painter.setFont(this->font());
    QFontMetrics metrics(this->font());

    painter.setBrush(this->palette().brush(IsActiveColorRole(), QPalette::Highlight));
    painter.setPen(this->palette().color(IsActiveColorRole(), QPalette::WindowText));
    painter.drawRect(0, 0, innerRect.left(), this->height());

    painter.setBrush(this->palette().brush(IsActiveColorRole(), QPalette::HighlightedText));
    painter.drawRect(innerRect);

    if (iText == "") {
        painter.drawPixmap(innerRect.left() - 10 - this->height() / 2, (this->height() / 2 - 16 / 2), 16, 16, this->iIcon.pixmap(16, 16));
    } else {
        painter.drawStaticText(innerRect.left() - metrics.horizontalAdvance(iText) - this->height() / 2, (this->height() / 2 - metrics.height() / 2), QStaticText(iText));
    }
    painter.setBrush(this->palette().brush(IsActiveColorRole(), QPalette::WindowText));
    painter.drawStaticText(innerRect.right() + this->height() / 2, (this->height() / 2 - metrics.height() / 2), QStaticText(oText));

    painter.setPen(this->palette().color(IsActiveColorRole(), QPalette::WindowText));
    painter.setBrush(QBrush(Qt::transparent));
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);
}

void tSwitch::mousePressEvent(QMouseEvent* event) {
    mouseClickPoint = event->localPos().toPoint().x();
    initialPoint = mouseClickPoint;
}

void tSwitch::mouseMoveEvent(QMouseEvent* event) {
    if (event->localPos().toPoint().x() < mouseClickPoint) {
        mouseMovedLeft = true;
    } else {
        mouseMovedLeft = false;
    }

    innerRect.translate(event->localPos().toPoint().x() - mouseClickPoint, 0);
    if (innerRect.x() <= 0) {
        innerRect.moveTo(0, 0);
    } else if (innerRect.right() >= this->width()) {
        innerRect.moveTo(this->width() - innerRect.width(), 0);
    }

    mouseClickPoint = event->localPos().toPoint().x();
    this->repaint();
}

void tSwitch::mouseReleaseEvent(QMouseEvent* event) {
    if (initialPoint - 2 < mouseClickPoint && initialPoint + 2 > mouseClickPoint) {
        bool checked = !this->isChecked();
        this->setChecked(checked);
    } else {
        if (mouseMovedLeft) {
            this->setChecked(false);
        } else {
            this->setChecked(true);
        }
    }
}

void tSwitch::checkChanging(bool checked) {
    tVariantAnimation* animation = new tVariantAnimation(this);
    animation->setStartValue(innerRect);
    animation->setDuration(250);

    if (checked) {
        animation->setEndValue(QRect(this->width() - this->height(), 0, this->height(), this->height()));
    } else {
        animation->setEndValue(QRect(0, 0, this->height(), this->height()));
    }

    animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(animation, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        innerRect = value.toRect();
        this->update();
    });
    connect(animation, &tVariantAnimation::finished, this, [ = ] {
        innerRect = animation->endValue().toRect();
        this->update();
    });
    connect(animation, &tVariantAnimation::finished, animation, &QObject::deleteLater);
    animation->start();
}

QSize tSwitch::sizeHint() const {
    QFontMetrics metrics(this->font());
    int width = SC_DPI(33);
    if (iText == "") {
        width += SC_DPI(8);
    } else {
        width += metrics.horizontalAdvance(iText);
    }

    width += metrics.horizontalAdvance(oText);
    return QSize(width, SC_DPI(22));
}

QString tSwitch::OnText() {
    return iText;
}

QString tSwitch::OffText() {
    return oText;
}

void tSwitch::setOnText(QString text) {
    iText = text;
    this->iIcon = QIcon();
    this->setFixedSize(this->sizeHint());
    this->update();
}

void tSwitch::setOffText(QString text) {
    oText = text;
    this->setFixedSize(this->sizeHint());
    this->update();
}

void tSwitch::setOnIcon(QIcon icon) {
    this->iIcon = icon;
    iText = "";
    this->setFixedSize(this->sizeHint());
    this->update();
}

QIcon tSwitch::OnIcon() {
    return this->iIcon;
}

void tSwitch::setChecked(bool checked) {
    QPushButton::setChecked(checked);
    this->checkChanging(this->isChecked());
}
