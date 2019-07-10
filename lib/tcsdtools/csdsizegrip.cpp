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
#include "csdsizegrip.h"

#include <QMouseEvent>
#include <QEvent>
#include <QPainter>
#include <QDebug>
#include "tcsdtools.h"

#ifdef HAVE_X11
#include <QX11Info>
#include <X11/Xlib.h>
#endif

CsdSizeGrip::CsdSizeGrip(int side, QWidget *parent) : QWidget(parent)
{
    connect(tCsdGlobal::instance(), &tCsdGlobal::csdsEnabledChanged, this, &CsdSizeGrip::csdsEnabledChanged);
    this->side = side;
    this->parentWidget = parent;
    this->setMouseTracking(true);

    parent->installEventFilter(this);

    resizeGrip();
    this->setVisible(tCsdGlobal::csdsEnabled());
}

void CsdSizeGrip::resizeGrip() {
    if (!tCsdGlobal::csdsEnabled()) return;
    if (this->parentWidget->isMaximized() || this->parentWidget->isFullScreen()) {
        //Disable the size grips while maximised
        this->setVisible(false);
    } else {
        this->setVisible(true);
        QRect geometry;
        switch (this->side) {
            case 0: //Top
                geometry = QRect(0, 0, parentWidget->width(), borderWidth());
                break;
            case 1: //Left
                geometry = QRect(0, 0, borderWidth(), parentWidget->height());
                break;
            case 2: //Bottom
                geometry = QRect(0, parentWidget->height() - borderWidth(), parentWidget->width(), borderWidth());
                break;
            case 3: //Right
                geometry = QRect(parentWidget->width() - borderWidth(), 0, borderWidth(), parentWidget->height());
        }
        this->setGeometry(geometry);
    }
}

bool CsdSizeGrip::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::Resize || event->type() == QEvent::WindowStateChange) {
        resizeGrip();
    }
    return false;
}

void CsdSizeGrip::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setBrush(Qt::transparent);
    painter.setBrush(this->palette().color(QPalette::WindowText));
    switch (this->side) {
        case 0: //Top
            painter.drawLine(borderWidth() - 1, this->height() - 1, this->width() - borderWidth() * 2 + 2, this->height() - 1);
            break;
        case 1: //Left
            painter.drawLine(this->width() - 1, borderWidth() - 1, this->width() - 1, this->height() - borderWidth() * 2 + 2);
            break;
        case 2: //Bottom
            painter.drawLine(borderWidth() - 1, 0, this->width() - borderWidth() * 2 + 3, 0);
            break;
        case 3: //Right
            painter.drawLine(0, borderWidth() - 1, 0, this->height() - borderWidth() * 2 + 2);
    }
}

void CsdSizeGrip::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
        //Resize this window
        #ifdef HAVE_X11
        if (QX11Info::isPlatformX11()) {
            XEvent event;
            event.xclient.type = ClientMessage;
            event.xclient.message_type = XInternAtom(QX11Info::display(), "_NET_WM_MOVERESIZE", False);
            event.xclient.display = QX11Info::display();
            event.xclient.window = this->window()->winId(); //Move the parent window of the widget
            event.xclient.format = 32;
            event.xclient.data.l[0] = e->globalPos().x();
            event.xclient.data.l[1] = e->globalPos().y();

            switch (hitTest(e->pos())) {
                case 0: //Top
                    event.xclient.data.l[2] = 1;
                    break;
                case 1: //Left
                    event.xclient.data.l[2] = 7;
                    break;
                case 2: //Bottom
                    event.xclient.data.l[2] = 5;
                    break;
                case 3: //Right
                    event.xclient.data.l[2] = 3;
                    break;
                case 4: //Upper right
                    event.xclient.data.l[2] = 2;
                    break;
                case 5: //Upper left
                    event.xclient.data.l[2] = 0;
                    break;
                case 6: //Lower left
                    event.xclient.data.l[2] = 6;
                    break;
                case 7: //Lower right
                    event.xclient.data.l[2] = 4;
                    break;
            }
            event.xclient.data.l[3] = Button1;
            event.xclient.data.l[4] = 0;

            XUngrabPointer(QX11Info::display(), CurrentTime);
            XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
            return;
        }
        #endif

        startPoint = e->pos();
        moving = true;
        qWarning() << "No method to initiate a window move.";
    }
}

void CsdSizeGrip::mouseMoveEvent(QMouseEvent *e) {
    if (moving) {
        QRect geometry = parentWidget->window()->geometry();
        switch (hitTest(startPoint)) {
            case 0: //Top
                geometry.setTop(e->globalY() + startPoint.y());
                break;
            case 1: //Left
                geometry.setLeft(e->globalX() + startPoint.x());
                break;
            case 2: //Bottom
                geometry.setBottom(e->globalY() - startPoint.y());
                break;
            case 3: //Right
                geometry.setRight(e->globalX() - startPoint.x());
                break;
            case 4: //Upper right
                geometry.setTopRight(e->globalPos());
                break;
            case 5: //Upper left
                geometry.setTopLeft(e->globalPos());
                break;
            case 6: //Lower left
                geometry.setBottomLeft(e->globalPos());
                break;
            case 7: //Lower right
                geometry.setBottomRight(e->globalPos());
                break;
        }
        parentWidget->window()->setGeometry(geometry);
    } else {
        //Set the cursor appropriately
        switch (hitTest(e->pos())) {
            case 0:
            case 2:
                this->setCursor(QCursor(Qt::SizeVerCursor));
                break;
            case 1:
            case 3:
                this->setCursor(QCursor(Qt::SizeHorCursor));
                break;
            case 4:
            case 6:
                this->setCursor(QCursor(Qt::SizeBDiagCursor));
                break;
            case 5:
            case 7:
                this->setCursor(QCursor(Qt::SizeFDiagCursor));

        }
    }
}

void CsdSizeGrip::mouseReleaseEvent(QMouseEvent *e) {
    moving = false;
}

int CsdSizeGrip::borderWidth() {
    return 3;
}

int CsdSizeGrip::hitTest(QPoint pos) {
    switch (side) {
        case 0: //Top
            if (pos.x() < borderWidth() * 2) return 5; //Upper left
            if (pos.x() > this->width() - borderWidth() * 2) return 4; //Upper right
            return 0;
        case 1: //Left
            if (pos.y() < borderWidth() * 2) return 5; //Upper left
            if (pos.y() > this->height() - borderWidth() * 2) return 6; //Lower left
            return 1;
        case 2: //Bottom
            if (pos.x() < borderWidth() * 2) return 6; //Lower left
            if (pos.x() > this->width() - borderWidth() * 2) return 7; //Lower right
            return 2;
        case 3: //Right
            if (pos.y() < borderWidth() * 2) return 4; //Upper right
            if (pos.y() > this->height() - borderWidth() * 2) return 7; //Lower right
            return 3;
    }
    return 0;
}

void CsdSizeGrip::csdsEnabledChanged(bool enabled) {
    this->setVisible(enabled);
    resizeGrip();
}
