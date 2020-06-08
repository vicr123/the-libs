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
#include "tstatusframe.h"
#include "ui_tstatusframe.h"

struct tStatusFramePrivate {
    QString title;
    QString text;

    tStatusFrame::State state = tStatusFrame::NoState;
};

tStatusFrame::tStatusFrame(QWidget* parent) :
    QFrame(parent),
    ui(new Ui::tStatusFrame) {
    ui->setupUi(this);

    d = new tStatusFramePrivate();
}

tStatusFrame::~tStatusFrame() {
    delete ui;
    delete d;
}

QString tStatusFrame::title() const {
    return d->title;
}

void tStatusFrame::setTitle(const QString& value) {
    d->title = value;
    ui->titleLabel->setText(value.toUpper());
    emit titleChanged();
}

QString tStatusFrame::text() const {
    return d->text;
}

void tStatusFrame::setText(const QString& value) {
    d->text = value;
    ui->textLabel->setText(value);
    emit textChanged();
}

tStatusFrame::State tStatusFrame::state() const {
    return d->state;
}

void tStatusFrame::setState(const tStatusFrame::State& state) {
    d->state = state;

    QPalette pal = QApplication::palette(this);

    switch (state) {
        case NoState:
            break;
        case Good:
            pal.setColor(QPalette::Window, QColor(0, 100, 0));
            pal.setColor(QPalette::WindowText, Qt::white);
            break;
        case Warning:
            pal.setColor(QPalette::Window, QColor(200, 100, 0));
            pal.setColor(QPalette::WindowText, Qt::white);
            break;
        case Error:
            pal.setColor(QPalette::Window, QColor(100, 0, 0));
            pal.setColor(QPalette::WindowText, Qt::white);
    }

    this->setPalette(pal);
    emit stateChanged();
}
