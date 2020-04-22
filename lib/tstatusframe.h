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
#ifndef TSTATUSFRAME_H
#define TSTATUSFRAME_H

#include "the-libs_global.h"
#include <QFrame>

namespace Ui {
    class tStatusFrame;
}

struct tStatusFramePrivate;
class THELIBSSHARED_EXPORT tStatusFrame : public QFrame {
        Q_OBJECT
        Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
        Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
        Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)

    public:
        explicit tStatusFrame(QWidget* parent = nullptr);
        ~tStatusFrame();

        enum State {
            NoState,
            Good,
            Warning,
            Error
        };

        QString title() const;
        void setTitle(const QString& value);

        QString text() const;
        void setText(const QString& value);

        State state() const;
        void setState(const State& state);

    signals:
        void titleChanged();
        void textChanged();
        void stateChanged();

    private:
        Ui::tStatusFrame* ui;
        tStatusFramePrivate* d;
};

#endif // TSTATUSFRAME_H
