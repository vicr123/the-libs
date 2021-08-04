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
#include "tdocumenttabber.h"
#include "ui_tdocumenttabber.h"

#include <QPushButton>
#include "tstackedwidget.h"

struct tDocumentTabberPrivate {
    tStackedWidget* stackedWidget = nullptr;
    QMap<QWidget*, QPushButton*> buttons;
};

tDocumentTabber::tDocumentTabber(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::tDocumentTabber) {
    ui->setupUi(this);
    d = new tDocumentTabberPrivate();
}

tDocumentTabber::~tDocumentTabber() {
    delete d;
    delete ui;
}

void tDocumentTabber::setStackedWidget(tStackedWidget* stackedWidget) {
    d->stackedWidget = stackedWidget;
    connect(stackedWidget, &tStackedWidget::widgetAdded, this, [ = ] {
        updateTabs();
    });
    updateTabs();
}

void tDocumentTabber::updateTabs() {
    QList<QWidget*> gone = d->buttons.keys();
    for (int i = 0; i < d->stackedWidget->count(); i++) {
        QWidget* w = d->stackedWidget->widget(i);
        if (!d->buttons.contains(w)) {
            //TODO: Check for an interface

            QPushButton* button = new QPushButton();
            button->setText("Document");
            button->setCheckable(true);
            button->setAutoExclusive(true);
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            connect(d->stackedWidget, &tStackedWidget::switchingFrame, this, [ = ](int index) {
                if (d->stackedWidget->widget(index) == w) button->setChecked(true);
            });
            connect(button, &QPushButton::clicked, this, [ = ] {
                d->stackedWidget->setCurrentWidget(w);
                w->setFocus();
            });
            if (d->stackedWidget->currentWidget() == w) button->setChecked(true);

            if (tDocumentTabberDocument* document = qobject_cast<tDocumentTabberDocument*>(w)) {
                connect(document, &tDocumentTabberDocument::tabTitleChanged, this, [ = ](QString tabTitle) {
                    button->setText(tabTitle);
                });
                button->setText(document->tabTitle());
            }

            ui->tabLayout->addWidget(button);
            d->buttons.insert(w, button);
        }
        gone.removeOne(w);
    }

    for (QWidget* toRemove : gone) {
        QPushButton* button = d->buttons.take(toRemove);
        ui->tabLayout->removeWidget(button);
        button->deleteLater();
    }
}

tDocumentTabberDocument::tDocumentTabberDocument(QWidget* parent) : QWidget(parent) {

}
