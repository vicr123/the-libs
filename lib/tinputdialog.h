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
#ifndef TINPUTDIALOG_H
#define TINPUTDIALOG_H

#include "the-libs_global.h"
#include <QLineEdit>

namespace Ui {
    class tInputDialog;
}

struct tInputDialogPrivate;
class THELIBSSHARED_EXPORT tInputDialog : public QWidget {
        Q_OBJECT

    public:
        explicit tInputDialog(QWidget* parent = nullptr);
        ~tInputDialog();

        static QString getText(QWidget* parent, const QString& title, const QString& label, QLineEdit::EchoMode mode = QLineEdit::Normal, const QString& text = QString(), bool* ok = nullptr, Qt::InputMethodHints inputMethodHints = Qt::ImhNone);

        void setTitle(QString title);
        void setMessage(QString message);
        void setText(QString text);
        void setEchoMode(QLineEdit::EchoMode echoMode);
        void setInputMethodHints(Qt::InputMethodHints hints);

        QString text();

        void show();
        void hide();

    private slots:
        void on_cancelButton_clicked();

        void on_okButton_clicked();

    signals:
        void accepted();
        void rejected();

    private:
        Ui::tInputDialog* ui;
        tInputDialogPrivate* d;

        bool eventFilter(QObject* watched, QEvent* event);

        QRect preferredGeometry();
};

#endif // TINPUTDIALOG_H
