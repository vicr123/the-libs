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
#include "debuglogwindow.h"
#include "ui_debuglogwindow.h"

#include <QScrollBar>
#include <QFontDatabase>
#include <QHeaderView>
#include <QPainter>
#include "tlogger.h"

struct DebugLogModelPrivate {
    struct DebugLogLogItem {
        tLogger::LogItem base;
        quint64 repeat = 0;
    };

    QList<DebugLogLogItem> logs;
};

DebugLogWindow::DebugLogWindow(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DebugLogWindow) {
    ui->setupUi(this);

    ui->logView->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->logView->setModel(new DebugLogModel());
    ui->logView->setItemDelegate(new LogDelegate());

    ui->logView->verticalScrollBar()->setValue(ui->logView->verticalScrollBar()->maximum());

    ui->logView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->logView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->logView->header()->setSectionResizeMode(2, QHeaderView::Stretch);
}

DebugLogWindow::~DebugLogWindow() {
    delete ui;
}

void DebugLogWindow::on_clearButton_clicked() {
    tLogger::clearLog();
}


void LogDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QRect rect = option.rect;
    if (index.column() == 0) {
        QRect severityRect = rect;
        severityRect.setWidth(SC_DPI(6));
        rect.setLeft(severityRect.right());

        painter->setPen(Qt::transparent);
        switch (index.data(Qt::UserRole).toInt()) {
            case QtDebugMsg:
                painter->setBrush(QColor(0, 100, 255));
                break;
            case QtWarningMsg:
                painter->setBrush(QColor(255, 100, 0));
                break;
            case QtCriticalMsg:
                painter->setBrush(QColor(255, 0, 0));
                break;
            case QtFatalMsg:
                painter->setBrush(QColor(100, 0, 0));
                break;
            case QtInfoMsg:
                painter->setBrush(Qt::transparent);
                break;
        }
        painter->drawRect(severityRect);
    }

    QString text = index.data().toString();
    QRect textRect = option.fontMetrics.boundingRect(rect, Qt::AlignTop | Qt::AlignLeading, text);
    textRect.adjust(SC_DPI(3), SC_DPI(3), SC_DPI(3), SC_DPI(3));
    rect.setLeft(textRect.right());
    painter->setFont(option.font);
    painter->setPen(option.palette.color(QPalette::WindowText));
    painter->drawText(textRect, Qt::AlignTop | Qt::AlignLeading, text);

    if (index.column() == 2) {
        quint64 repeat = index.data(Qt::UserRole).toULongLong();
        if (repeat > 0) {
            QString text = QStringLiteral(" ×%1").arg(repeat);
            QRect repeatRect = rect;
            repeatRect.adjust(SC_DPI(3), SC_DPI(3), SC_DPI(-3), SC_DPI(-3));

            painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
            painter->drawText(repeatRect, Qt::AlignLeading | Qt::AlignVCenter, text);
        }
    }
}

QSize LogDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    switch (index.column()) {
        case 0: {
            QSize size;
            size.setHeight(option.fontMetrics.height() + SC_DPI(6));
            size.setWidth(option.fontMetrics.horizontalAdvance("[00:00:00]") + SC_DPI(12));
            return size;
        }
        case 1: {
            return QStyledItemDelegate::sizeHint(option, index);
        }
        case 2: {
            QSize size = option.fontMetrics.size(0, index.data().toString());
            size.rheight() += SC_DPI(6);
            size.rwidth() += SC_DPI(6);
            return size;
        }
    }
    return QSize();
}

DebugLogModel::DebugLogModel() {
    d = new DebugLogModelPrivate();

    QList<tLogger::LogItem> logs = tLogger::logItems();
    for (tLogger::LogItem log : logs) {
        addLogItem(log);
    }

    connect(tLogger::instance(), &tLogger::newLogItem, this, &DebugLogModel::addLogItem);
    connect(tLogger::instance(), &tLogger::logCleared, this, [ = ] {
        beginRemoveRows(QModelIndex(), 0, rowCount());
        d->logs.clear();
        endRemoveRows();
    });
}

DebugLogModel::~DebugLogModel() {
    delete d;
}

void DebugLogModel::addLogItem(tLogger::LogItem logItem) {
    if (!d->logs.isEmpty()) {
        DebugLogModelPrivate::DebugLogLogItem lastItem = d->logs.last();
        if (lastItem.base.text == logItem.text && lastItem.base.context == lastItem.base.context && lastItem.base.severity == logItem.severity) {
            ++lastItem.repeat;
            d->logs.replace(d->logs.count() - 1, lastItem);
            emit dataChanged(index(d->logs.count() - 1), index(d->logs.count(), columnCount()));
            return;
        }
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount() + 1);
    DebugLogModelPrivate::DebugLogLogItem debugLogLogItem;
    debugLogLogItem.base = logItem;
    debugLogLogItem.repeat = 0;
    d->logs.append(debugLogLogItem);
    endInsertRows();
}


int DebugLogModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return d->logs.count();
}

int DebugLogModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;

    return 3;
}

QVariant DebugLogModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid()) return QVariant();

    DebugLogModelPrivate::DebugLogLogItem log = d->logs.at(index.row());

    switch (index.column()) {
        case 0: //Timestamp
            switch (role) {
                case Qt::DisplayRole:
                    return log.base.timestamp.toString("[hh:MM:ss]");
                case Qt::UserRole:
                    return log.base.severity;
            }

            break;
        case 1: //Context
            if (role == Qt::DisplayRole) return log.base.context;
            break;
        case 2: //Text
            switch (role) {
                case Qt::DisplayRole:
                    return log.base.text;
                case Qt::UserRole:
                    return log.repeat;
            }

            break;
    }

    return QVariant();
}


QVariant DebugLogModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();
    switch (section) {
        case 0:
            return tr("Timestamp");
        case 1:
            return tr("Context");
        case 2:
            return tr("Message");
    }
    return QVariant();
}
