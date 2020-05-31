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
#include "ttitlelabel.h"

#include <QPainter>
#include <QToolButton>
#include <QApplication>

struct tTitleLabelPrivate {
    QToolButton* backButton;
    bool backButtonShown = false;
    bool backButtonIsMenu = false;
};

tTitleLabel::tTitleLabel(QWidget* parent) : QLabel(parent) {
    d = new tTitleLabelPrivate();

    d->backButton = new QToolButton(this);
    d->backButton->setIcon(QIcon::fromTheme("go-previous"));
    d->backButton->setAutoRaise(true);
    d->backButton->move(0, 0);
    d->backButton->setFixedHeight(this->height() - 1);
    d->backButton->setFixedWidth(d->backButton->sizeHint().width());
    d->backButton->setVisible(false);
    connect(d->backButton, &QToolButton::clicked, this, &tTitleLabel::backButtonClicked);

    connect(qApp, &QApplication::fontChanged, this, &tTitleLabel::updateFont);
    updateFont();

    this->setMargin(9);
}

tTitleLabel::~tTitleLabel() {
    delete d;
}

QSize tTitleLabel::sizeHint() const {
    //TODO: Account for actions
    QSize labelSize = QLabel::sizeHint();
    labelSize.setHeight(qMax(labelSize.height(), d->backButton->sizeHint().height()));
    labelSize.rheight()++;
    return labelSize;
}

bool tTitleLabel::backButtonShown() {
    return d->backButtonShown;
}

void tTitleLabel::setBackButtonShown(bool backButtonShown) {
    d->backButtonShown = backButtonShown;

    d->backButton->setVisible(backButtonShown);
    if (this->layoutDirection() == Qt::RightToLeft) {
        this->setContentsMargins(0, 0, backButtonShown ? d->backButton->width() : 0, 0);
    } else {
        this->setContentsMargins(backButtonShown ? d->backButton->width() : 0, 0, 0, 0);
    }

    emit backButtonShownChanged(backButtonShown);
}

bool tTitleLabel::backButtonIsMenu() {
    return d->backButtonIsMenu;
}

void tTitleLabel::setBackButtonIsMenu(bool backButtonIsMenu) {
    d->backButtonIsMenu = backButtonIsMenu;
    if (backButtonIsMenu) {
        d->backButton->setIcon(QIcon::fromTheme("application-menu"));
    } else {
        d->backButton->setIcon(QIcon::fromTheme("go-previous"));
    }

    emit backButtonIsMenuChanged(backButtonIsMenu);
}

void tTitleLabel::updateFont() {
    QFont font = qApp->font(this);
    font.setPointSizeF(font.pointSizeF() * 1.5);
    this->setFont(font);
}

void tTitleLabel::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event);
    QPainter painter(this);
    painter.setPen(theLibsGlobal::lineColor(this->palette().color(QPalette::WindowText)));
    painter.drawLine(0, this->height() - 1, this->width(), this->height() - 1);
}

void tTitleLabel::resizeEvent(QResizeEvent* event) {
    d->backButton->setFixedHeight(this->height() - 1);

    if (this->layoutDirection() == Qt::RightToLeft) {
        d->backButton->move(this->width() - d->backButton->width(), 0);
    }
}
