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
#include "jobdbus.h"

#include "job_adaptor.h"
#include "tjob.h"

struct JobDbusPrivate {
    tJob* job;
};

JobDbus::JobDbus(QString path, tJob* job, QObject* parent) : QObject(parent) {
    d = new JobDbusPrivate();
    d->job = job;

    connect(job, &tJob::progressChanged, this, &JobDbus::ProgressChanged);
    connect(job, &tJob::totalProgressChanged, this, &JobDbus::TotalProgressChanged);
    connect(job, &tJob::stateChanged, this, [ = ] {
        emit StateChanged(this->State());
    });

    new JobAdaptor(this);
    QDBusConnection::sessionBus().registerObject(path, this);
}

JobDbus::~JobDbus() {
    delete d;
}

quint64 JobDbus::Progress() {
    return d->job->progress();
}

quint64 JobDbus::TotalProgress() {
    return d->job->totalProgress();
}

QString JobDbus::State() {
    switch (d->job->state()) {
        case tJob::Processing:
            return QStringLiteral("Processing");
        case tJob::Finished:
            return QStringLiteral("Finished");
        case tJob::Failed:
            return QStringLiteral("Failed");
        case tJob::RequiresAttention:
            return QStringLiteral("RequiresAttention");

    }
}
