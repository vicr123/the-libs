/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2022 Victor Tran
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
#ifndef TPRINTPOPOVER_H
#define TPRINTPOPOVER_H

#include "the-libs_global.h"
#include <QWidget>

namespace Ui {
    class tPrintPopover;
}

class THELIBSSHARED_EXPORT tPrintPopoverCustomPrintSettingsWidget : public QWidget {
        Q_OBJECT

    public:
        explicit tPrintPopoverCustomPrintSettingsWidget(QWidget* parent = nullptr);
        ~tPrintPopoverCustomPrintSettingsWidget();

    signals:
        void requestPaintUpdate();
};

class QPrinter;
struct tPrintPopoverPrivate;
class THELIBSSHARED_EXPORT tPrintPopover : public QWidget {
        Q_OBJECT

    public:
        explicit tPrintPopover(QPrinter* printer, QWidget* parent = nullptr);
        ~tPrintPopover();

        void setTitle(QString title);
        QString title();

        void setCustomPrintSettingsWidget(tPrintPopoverCustomPrintSettingsWidget* customWidget);
        tPrintPopoverCustomPrintSettingsWidget* customPrintSettingsWidget();

    signals:
        void paintRequested(QPrinter* printer);
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_printerBox_currentIndexChanged(int index);

        void on_printButton_clicked();

        void on_pageSizeBox_currentIndexChanged(int index);

        void on_grayscaleBox_toggled(bool checked);

        void on_duplexBox_toggled(bool checked);

    private:
        Ui::tPrintPopover* ui;
        tPrintPopoverPrivate* d;

        void updatePageSizes();

        // QWidget interface
    protected:
        void showEvent(QShowEvent* event);
};

#endif // TPRINTPOPOVER_H
