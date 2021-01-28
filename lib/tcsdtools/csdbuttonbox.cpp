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
#include "csdbuttonbox.h"
#include "ui_csdbuttonbox.h"

#include "the-libs_global.h"
#include <QPainter>
#include "tcsdtools.h"

CsdButtonBox::CsdButtonBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CsdButtonBox)
{
    ui->setupUi(this);

    parentWidget = parent;
    parent->installEventFilter(this);

    ui->minButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->maxButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->closeButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        static_cast<QBoxLayout*>(this->layout())->setDirection(QBoxLayout::RightToLeft);
    }

    connect(tCsdGlobal::instance(), &tCsdGlobal::csdsEnabledChanged, this, &CsdButtonBox::csdsEnabledChanged);
    csdsEnabledChanged(tCsdGlobal::csdsEnabled());

#ifdef Q_OS_MAC
    setupMacOs();
#endif
}

CsdButtonBox::~CsdButtonBox()
{
    delete ui;
}

bool CsdButtonBox::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (parentWidget->window()->isMaximized()) {
            ui->maxButton->setIcon(QIcon(":/tcsdtools/res.svg"));
        } else {
            ui->maxButton->setIcon(QIcon(":/tcsdtools/max.svg"));
        }
    }
    return false;
}

void CsdButtonBox::on_closeButton_clicked()
{
    parentWidget->window()->close();
}

void CsdButtonBox::on_maxButton_clicked()
{
    if (parentWidget->window()->isMaximized()) {
        parentWidget->window()->showNormal();
    } else {
        parentWidget->window()->showMaximized();
    }
}

void CsdButtonBox::on_minButton_clicked()
{
    parentWidget->window()->showMinimized();
}

void CsdButtonBox::csdsEnabledChanged(bool enabled) {
    this->setVisible(enabled);
}
