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
#include "jobbutton.h"

#include "tjob.h"
#include "tjobmanager.h"
#include "jobspopover.h"
#include "tpopover.h"
#include <QPainter>
#include <tvariantanimation.h>

struct JobButtonPrivate {
    QList<tJob*> trackedJobs;
    qreal totalPercentage = 0;

    tVariantAnimation* emblemPulse;
};

JobButton::JobButton(QWidget* parent) : QToolButton(parent) {
    d = new JobButtonPrivate();

    d->emblemPulse = new tVariantAnimation();
    d->emblemPulse->setStartValue(0.0);
    d->emblemPulse->setEndValue(1.0);
    d->emblemPulse->setEasingCurve(QEasingCurve::OutCubic);
    d->emblemPulse->setDuration(2000);
    connect(d->emblemPulse, &tVariantAnimation::valueChanged, this, [ = ] {
        this->update();
    });

    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    connect(tJobManager::instance(), &tJobManager::jobAdded, this, &JobButton::trackJob);
    connect(this, &JobButton::clicked, this, [ = ] {
        JobsPopover* jp = new JobsPopover();
        tPopover* popover = new tPopover(jp);
        popover->setPopoverWidth(SC_DPI(300));
        connect(jp, &JobsPopover::done, popover, &tPopover::dismiss);
        connect(popover, &tPopover::dismissed, popover, &tPopover::deleteLater);
        connect(popover, &tPopover::dismissed, jp, &JobsPopover::deleteLater);
        popover->show(this->window());

        QList<tJob*> finishedJobs;
        for (tJob* job : d->trackedJobs) {
            //If the job is finished remove it from the tracking
            if (job->state() == tJob::Finished || job->state() == tJob::Failed) {
                finishedJobs.append(job);
            }
        }
        for (tJob* job : finishedJobs) {
            d->trackedJobs.removeOne(job);
        }

        if (d->trackedJobs.count() == 0) {
            d->emblemPulse->start();
            this->setVisible(false);
        }
    });

    this->setVisible(false);
}

JobButton::~JobButton() {
    delete d;
}

void JobButton::trackJob(tJob* job) {
    connect(job, &tJob::progressChanged, this, &JobButton::updateJobs);
    connect(job, &tJob::totalProgressChanged, this, &JobButton::updateJobs);
    connect(job, &tJob::stateChanged, this, &JobButton::updateJobs);
    d->trackedJobs.append(job);
    this->setVisible(true);
}

void JobButton::updateJobs() {
    quint64 sum = 0;
    quint64 sumTotal = 0;
    for (tJob* job : d->trackedJobs) {
        sum += job->progress();
        sumTotal += job->totalProgress();
    }

    if (sumTotal == 0) {
        d->totalPercentage = -1;
    } else {
        d->totalPercentage = static_cast<qreal>(sum) / sumTotal;
    }

    this->update();
}

void JobButton::paintEvent(QPaintEvent* event) {
    QToolButton::paintEvent(event);

    QRect iconRect;
    iconRect.setSize(SC_DPI_T(QSize(16, 16), QSize));
    iconRect.moveCenter(QPoint(this->width() / 2, this->height() / 2));

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(QRect(-8, -8, 16, 16));
    painter.setViewport(iconRect);

    QRect circleRect(-8, -8, 15, 15);
    painter.setPen(QPen(this->palette().color(QPalette::WindowText), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setOpacity(0.5);
    painter.drawEllipse(circleRect);

    painter.setOpacity(1);
    painter.setPen(Qt::transparent);
    painter.setBrush(this->palette().color(QPalette::WindowText));
    painter.drawPie(circleRect, 1440, d->totalPercentage * -5760);

    //Draw an emblem
    int emblemType = 0;
    for (tJob* job : d->trackedJobs) {
        if (job->state() == tJob::Finished && emblemType < 1) emblemType = 1;
        if (job->state() == tJob::Failed && emblemType < 2) emblemType = 2;
    }

    QColor emblemCol = Qt::transparent;
    if (emblemType == 1) { //Draw the success emblem
        emblemCol = QColor(0, 255, 0);
    } else if (emblemType == 2) {
        emblemCol = QColor(255, 150, 0);
    }

    if (emblemType != 0) {
        if (d->emblemPulse->state() != tVariantAnimation::Running) {
            d->emblemPulse->start();
        }

        QRect emblemRect(0, 0, 8, 8);
        painter.setPen(Qt::transparent);
        painter.setBrush(emblemCol);
        painter.drawEllipse(emblemRect.center(), 4, 4);

        qreal pulseRadius = 4 + d->emblemPulse->currentValue().toReal() * 4;
        painter.setPen(emblemCol);
        painter.setBrush(Qt::transparent);
        painter.setOpacity(1.0 - d->emblemPulse->currentValue().toReal());
        painter.drawEllipse(QPointF(emblemRect.center()), pulseRadius, pulseRadius);
    }
}
