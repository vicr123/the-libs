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
#ifndef TCSDTOOLS_H
#define TCSDTOOLS_H

#include <QObject>
#include "the-libs_global.h"

class tPopover;
struct tCsdGlobalPrivate;
class THELIBSSHARED_EXPORT tCsdGlobal : public QObject
{
    Q_OBJECT
    public:
        enum WindowControlSide {
            Left,
            Right
        };

        static WindowControlSide windowControlsEdge();

        static void setCsdsEnabled(bool csdsEnabled);
        static bool csdsEnabled();

        static bool recommendCsdForPlatform();

        static tCsdGlobal* instance();

    signals:
        void csdsEnabledChanged(bool enabled);

    private:
        explicit tCsdGlobal();
        static tCsdGlobalPrivate* d;
};

struct tCsdToolsPrivate;
struct ResizeWidget;
class THELIBSSHARED_EXPORT tCsdTools : public QObject
{
        Q_OBJECT
    public:
        explicit tCsdTools(QObject *parent = nullptr);
        ~tCsdTools() override;

        QWidget* csdBoxForWidget(QWidget* widget);

    signals:

    public slots:
        void installMoveAction(QWidget* widget);
        void removeMoveAction(QWidget* widget);

        void installResizeAction(QWidget* widget);
        void removeResizeAction(QWidget* widget);

    private slots:
        void removeMoveAction(QObject* widget);
        void removeResizeAction(QObject* widget);
        void csdsEnabledChanged(bool enabled);

    protected:
        friend tPopover;
        static QWidget* widgetForPopover(QWidget* selected);

    private:
        tCsdToolsPrivate* d;
        ResizeWidget* getResizeWidget(QWidget* widget);

        bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // TCSDTOOLS_H
