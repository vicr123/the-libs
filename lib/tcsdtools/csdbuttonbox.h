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
#ifndef CSDBUTTONBOX_H
#define CSDBUTTONBOX_H

#include <QWidget>

namespace Ui {
    class CsdButtonBox;
}

class CsdButtonBox : public QWidget
{
        Q_OBJECT

    public:
        explicit CsdButtonBox(QWidget *parent);
        ~CsdButtonBox() override;

    private slots:
        void on_closeButton_clicked();

        void on_maxButton_clicked();

        void on_minButton_clicked();

        void csdsEnabledChanged(bool enabled);

    private:
        Ui::CsdButtonBox *ui;

#ifdef Q_OS_MAC
        void setupMacOs();
#endif

        QWidget* parentWidget;
        bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // CSDBUTTONBOX_H
