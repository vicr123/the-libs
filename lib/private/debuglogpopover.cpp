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
#include "debuglogpopover.h"
#include "ui_debuglogpopover.h"

#include <QFontDatabase>

DebugLogPopover::DebugLogPopover(tLogger::LogItem item, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::DebugLogPopover) {
    ui->setupUi(this);

    ui->titleLabel->setBackButtonShown(true);

    ui->textBrowser->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    QStringList description;
    description.append(tr("Timestamp").leftJustified(20) + item.timestamp.toString());
    description.append(tr("Context").leftJustified(20) + item.context);
    description.append(tr("Severity").leftJustified(20) + tLogger::severityToString(item.severity));
    description.append(tr("File").leftJustified(20) + (item.file.isEmpty() ? tr("[unavailable]") : QStringLiteral("%1:%2").arg(item.file).arg(item.line)));
    description.append(tr("Function").leftJustified(20) + (item.function.isEmpty() ? tr("[unavailable]") : item.function));
    description.append("");
    description.append(item.text);

    ui->textBrowser->setText(description.join("\n"));
}

DebugLogPopover::~DebugLogPopover() {
    delete ui;
}

void DebugLogPopover::on_titleLabel_backButtonClicked() {
    emit done();
}
