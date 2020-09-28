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
#include "jobspopover.h"
#include "ui_jobspopover.h"

#include "tjob.h"
#include "tjobmanager.h"

struct JobsPopoverPrivate {

};

JobsPopover::JobsPopover(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::JobsPopover) {
    ui->setupUi(this);

    d = new JobsPopoverPrivate();

    ui->titleLabel->setBackButtonShown(true);

    for (tJob* job : tJobManager::jobs()) {
        addJob(job);
    }
}

JobsPopover::~JobsPopover() {
    delete d;
    delete ui;
}

void JobsPopover::on_titleLabel_backButtonClicked() {
    emit done();
}

void JobsPopover::addJob(tJob* job) {
    QWidget* progressWidget = job->makeProgressWidget();

    if (job->isTransient()) {
        if (job->state() == tJob::Finished) return;

        ui->jobsLayout->addWidget(progressWidget);
        connect(job, &tJob::stateChanged, this, [ = ](tJob::State state) {
            if (state == tJob::Finished) {
                ui->jobsLayout->removeWidget(progressWidget);
            }
        });
    } else {
        ui->jobsLayout->addWidget(progressWidget);
    }

    QFrame* frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFixedHeight(1);
    ui->jobsLayout->addWidget(frame);
}
