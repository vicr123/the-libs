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
#include "tinputdialog.h"
#include "ui_tinputdialog.h"

#include <QMainWindow>
#include <QGraphicsOpacityEffect>
#include <QShortcut>
#include "tcsdtools.h"
#include "tscrim.h"
#include "tvariantanimation.h"

struct tInputDialogPrivate {
    QWidget* parentWidget;
    QGraphicsOpacityEffect* opacity;
};

tInputDialog::tInputDialog(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::tInputDialog) {
    ui->setupUi(this);
    d = new tInputDialogPrivate();

    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parent);
    if (mainWindow && tCsdTools::csdsInstalled(mainWindow)) {
        d->parentWidget = mainWindow->centralWidget();
    } else {
        d->parentWidget = parent;
    }

    d->opacity = new QGraphicsOpacityEffect(this);
    d->opacity->setOpacity(0);
    this->setGraphicsEffect(d->opacity);

    QShortcut* cancelShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(cancelShortcut, &QShortcut::activated, ui->cancelButton, &QPushButton::click);
    connect(cancelShortcut, &QShortcut::activatedAmbiguously, ui->cancelButton, &QPushButton::click);

    ui->cancelButton->setProperty("type", "destructive");
}

tInputDialog::~tInputDialog() {
    delete d;
    delete ui;
}

QString tInputDialog::getText(QWidget* parent, const QString& title, const QString& label, QLineEdit::EchoMode mode, const QString& text, bool* ok, Qt::InputMethodHints inputMethodHints) {
    QEventLoop* loop = new QEventLoop();
    tInputDialog* dialog = new tInputDialog(parent);
    dialog->setTitle(title);
    dialog->setMessage(label);
    dialog->setEchoMode(mode);
    dialog->setText(text);
    dialog->setInputMethodHints(inputMethodHints);
    connect(dialog, &tInputDialog::accepted, loop, [ = ] {
        if (ok) *ok = true;
        loop->exit();
    });
    connect(dialog, &tInputDialog::rejected, loop, [ = ] {
        if (ok) *ok = false;
        loop->exit();
    });
    dialog->show();

    loop->exec();
    loop->deleteLater();

    return dialog->text();
}

void tInputDialog::setTitle(QString title) {
    ui->titleLabel->setText(title.toUpper());
}

void tInputDialog::setMessage(QString message) {
    ui->messageLabel->setText(message);
}

void tInputDialog::setText(QString text) {
    ui->inputEdit->setText(text);
}

void tInputDialog::setEchoMode(QLineEdit::EchoMode echoMode) {
    ui->inputEdit->setEchoMode(echoMode);
}

void tInputDialog::setInputMethodHints(Qt::InputMethodHints hints) {
    ui->inputEdit->setInputMethodHints(hints);
}

QString tInputDialog::text() {
    return ui->inputEdit->text();
}

void tInputDialog::show() {
    tScrim::scrimForWidget(d->parentWidget)->show();

    QWidget::show();
    this->raise();

    d->opacity->setEnabled(true);
    d->opacity->setOpacity(0);

    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setDuration(400);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        d->opacity->setOpacity(value.toReal());

        QRect geometry = preferredGeometry();
        geometry.moveTop(geometry.top() - SC_DPI(10) * (1.0 - value.toReal()));
        this->setFixedSize(QSize(0, 0));
        this->setFixedSize(geometry.size());
        this->setGeometry(geometry);
    });
    connect(anim, &tVariantAnimation::finished, this, [ = ] {
        d->opacity->setEnabled(false);
        anim->deleteLater();
        d->parentWidget->installEventFilter(this);
    });
    anim->start();

    ui->inputEdit->setFocus();
}

void tInputDialog::hide() {
    d->parentWidget->removeEventFilter(this);
    d->opacity->setEnabled(true);

    tVariantAnimation* anim = new tVariantAnimation(this);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setDuration(400);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(anim, &tVariantAnimation::valueChanged, this, [ = ](QVariant value) {
        d->opacity->setOpacity(value.toReal());

        QRect geometry = preferredGeometry();
        geometry.moveTop(geometry.top() + SC_DPI(10) * (1.0 - value.toReal()));
        this->setFixedSize(QSize(0, 0));
        this->setFixedSize(geometry.size());
        this->setGeometry(geometry);
    });
    connect(anim, &tVariantAnimation::finished, this, [ = ] {
        anim->deleteLater();
        QWidget::hide();
    });
    anim->start();

    tScrim::scrimForWidget(d->parentWidget)->hide();
}

void tInputDialog::on_cancelButton_clicked() {
    this->hide();
    emit rejected();
}

void tInputDialog::on_okButton_clicked() {
    this->hide();
    emit accepted();
}

bool tInputDialog::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::Resize) {
        this->setGeometry(preferredGeometry());
    }
    return false;
}

QRect tInputDialog::preferredGeometry() {
    QRect geom;
    geom.setSize(this->sizeHint());
    if (geom.width() < SC_DPI(400)) geom.setWidth(SC_DPI(400));
    if (geom.width() > d->parentWidget->width()) geom.setWidth(d->parentWidget->width());
    geom.moveCenter(QPoint(d->parentWidget->geometry().width() / 2, d->parentWidget->geometry().height() / 2));
    return geom;
}
