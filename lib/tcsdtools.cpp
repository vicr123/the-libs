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
#include "tcsdtools.h"

#include <QWidget>
#include <QMouseEvent>
#include <QDebug>
#include <tcsdtools/csdsizegrip.h>
#include <tcsdtools/csdbuttonbox.h>

#ifdef HAVE_X11
#include <QX11Info>
#include <X11/Xlib.h>
#endif

struct tCsdGlobalPrivate {
    tCsdGlobal* instance = nullptr;
    bool enableCSDs;
};

struct ResizeWidget {
    QWidget* widget;
    CsdSizeGrip* sizeGrips[4];

    void setMarginsEnabled(bool marginsEnabled) {
        if (marginsEnabled) {
            int width = CsdSizeGrip::borderWidth();
            widget->setContentsMargins(width, width, width, width);
        } else {
            widget->setContentsMargins(0, 0, 0, 0);
        }
    }
};

struct tCsdToolsPrivate {
    QList<QWidget*> moveWidgets;
    QList<ResizeWidget*> resizeWidgets;
};

tCsdGlobalPrivate* tCsdGlobal::d = new tCsdGlobalPrivate();

tCsdGlobal::tCsdGlobal() : QObject(nullptr) {
    d->enableCSDs = recommendCsdForPlatform();
}

tCsdGlobal* tCsdGlobal::instance() {
    if (d->instance == nullptr) d->instance = new tCsdGlobal();
    return d->instance;
}

void tCsdGlobal::setCsdsEnabled(bool csdsEnabled) {
    tCsdGlobal::instance(); //Initialize the private struct
    d->enableCSDs = csdsEnabled;
    emit tCsdGlobal::instance()->csdsEnabledChanged(csdsEnabled);
}

bool tCsdGlobal::csdsEnabled() {
    tCsdGlobal::instance(); //Initialize the private struct
    return d->enableCSDs;
}

bool tCsdGlobal::recommendCsdForPlatform() {
#ifdef Q_OS_MAC
    return false;
#else
    return true;
#endif
}

tCsdTools::tCsdTools(QObject *parent) : QObject(parent)
{
    d = new tCsdToolsPrivate();
    connect(tCsdGlobal::instance(), &tCsdGlobal::csdsEnabledChanged, this, &tCsdTools::csdsEnabledChanged);
}

tCsdTools::~tCsdTools() {
    delete d;
}

tCsdGlobal::WindowControlSide tCsdGlobal::windowControlsEdge() {
    #ifdef HAVE_X11
        return Right;
    #elif defined(Q_OS_MAC)
        return Left;
    #elif defined(Q_OS_WIN)
        return Right;
    #else
        return Right;
    #endif
}

void tCsdTools::installMoveAction(QWidget* widget) {
    d->moveWidgets.append(widget);
    widget->installEventFilter(this);
    connect(widget, &QWidget::destroyed, this, QOverload<QObject*>::of(&tCsdTools::removeMoveAction));
}

void tCsdTools::removeMoveAction(QWidget* widget) {
    d->moveWidgets.removeAll(widget);
    widget->removeEventFilter(this);
    disconnect(widget, &QWidget::destroyed, this, QOverload<QObject*>::of(&tCsdTools::removeMoveAction));
}

void tCsdTools::removeMoveAction(QObject* widget) {
    removeMoveAction(qobject_cast<QWidget*>(widget));
}

void tCsdTools::installResizeAction(QWidget* widget) {
    connect(widget, &QWidget::destroyed, this, QOverload<QObject*>::of(&tCsdTools::removeResizeAction));
    widget->installEventFilter(this);

    if (tCsdGlobal::csdsEnabled()) {
        int border = CsdSizeGrip::borderWidth();
        widget->setContentsMargins(border, border, border, border);
        widget->setWindowFlag(Qt::FramelessWindowHint);
    }

    ResizeWidget* resize = new ResizeWidget();
    resize->widget = widget;

    for (int i = 0; i < 4; i++) {
        CsdSizeGrip* grip = new CsdSizeGrip(i, widget);
        resize->sizeGrips[i] = grip;
    }

    d->resizeWidgets.append(resize);
}

void tCsdTools::removeResizeAction(QWidget* widget) {
    ResizeWidget* rw = getResizeWidget(widget);
    Q_ASSERT(rw);

    widget->removeEventFilter(this);
    disconnect(widget, &QWidget::destroyed, this, QOverload<QObject*>::of(&tCsdTools::removeResizeAction));

    for (int i = 0; i < 4; i++) {
        rw->sizeGrips[i]->deleteLater();
    }

    d->resizeWidgets.removeOne(rw);
    delete rw;
}

void tCsdTools::removeResizeAction(QObject* widget) {
    removeResizeAction(qobject_cast<QWidget*>(widget));
}

ResizeWidget* tCsdTools::getResizeWidget(QWidget *widget) {
    for (ResizeWidget* rw : d->resizeWidgets) {
        if (rw->widget == widget) {
            return rw;
        }
    }
    return nullptr;
}

QWidget* tCsdTools::csdBoxForWidget(QWidget *widget) {
    return new CsdButtonBox(widget);
}

void tCsdTools::csdsEnabledChanged(bool enabled) {
    for (ResizeWidget* rw : d->resizeWidgets) {
        bool showing = rw->widget->isVisible();
        //Qt::WindowStates states = rw->widget->windowState();
        if (enabled) {
            int border = CsdSizeGrip::borderWidth();
            rw->widget->setContentsMargins(border, border, border, border);
            rw->widget->setWindowFlag(Qt::FramelessWindowHint);
        } else {
            rw->widget->setContentsMargins(0, 0, 0, 0);
            rw->widget->setWindowFlag(Qt::FramelessWindowHint, false);
        }
        //rw->widget->setWindowState(states);
        if (showing) rw->widget->show();
    }
}

bool tCsdTools::eventFilter(QObject *watched, QEvent *event) {
    //Ignore all handling if CSDs are disabled
    if (!tCsdGlobal::csdsEnabled()) return false;

    if (event->type() == QEvent::MouseButtonPress) {
        QWidget* widget = qobject_cast<QWidget*>(watched);
        Q_ASSERT(widget);

        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        if (d->moveWidgets.contains(widget) && e->button() == Qt::LeftButton) {
            //Move this window
            #ifdef HAVE_X11
            if (QX11Info::isPlatformX11()) {
                XEvent event;
                event.xclient.type = ClientMessage;
                event.xclient.message_type = XInternAtom(QX11Info::display(), "_NET_WM_MOVERESIZE", False);
                event.xclient.display = QX11Info::display();
                event.xclient.window = widget->window()->winId(); //Move the parent window of the widget
                event.xclient.format = 32;
                event.xclient.data.l[0] = e->globalPos().x();
                event.xclient.data.l[1] = e->globalPos().y();
                event.xclient.data.l[2] = 8;
                event.xclient.data.l[3] = Button1;
                event.xclient.data.l[4] = 0;

                XUngrabPointer(QX11Info::display(), CurrentTime);
                XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
                return true;
            }
            #endif

            //Move window using Qt methods
            qWarning() << "Unsupported platform; moving window manually.";
            widget->setProperty("tcsdtools_action", "move");
            widget->setProperty("tcsdtools_mousepoint", widget->window()->mapFromGlobal(e->globalPos()));

            return true; //Prevent further handling of this event
        }
    } else if (event->type() == QEvent::MouseMove) {
        QWidget* widget = qobject_cast<QWidget*>(watched);
        Q_ASSERT(widget);

        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        if (d->moveWidgets.contains(widget) && widget->property("tcsdtools_action").toString() == "move") {
            //Move this window using Qt methods
            QPoint oldPos = widget->property("tcsdtools_mousepoint").toPoint();
            widget->window()->move(e->globalPos() - oldPos);
            return true; //Prevent further handling of this event
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QWidget* widget = qobject_cast<QWidget*>(watched);
        Q_ASSERT(widget);

        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        if (d->moveWidgets.contains(widget) && widget->property("tcsdtools_action").toString() == "move") {
            //Stop moving the window
            widget->setProperty("tcsdtools_action", "none");
        }
    } else if (event->type() == QEvent::Resize) {
        QWidget* widget = qobject_cast<QWidget*>(watched);
        Q_ASSERT(widget);

        ResizeWidget* rw = getResizeWidget(widget);
        if (rw != nullptr) {
            bool marginsEnabled = true;
            if (widget->isMaximized()) marginsEnabled = false;
            rw->setMarginsEnabled(marginsEnabled);
        }
    }
    return false;
}
