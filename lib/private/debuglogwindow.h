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
#ifndef DEBUGLOGWINDOW_H
#define DEBUGLOGWINDOW_H

#include <QDialog>
#include <QStyledItemDelegate>
#include <QAbstractListModel>
#include <tlogger.h>

namespace Ui {
    class DebugLogWindow;
}

class DebugLogWindow : public QDialog {
        Q_OBJECT

    public:
        explicit DebugLogWindow(QWidget* parent = nullptr);
        ~DebugLogWindow();

    private slots:
        void on_clearButton_clicked();

    private:
        Ui::DebugLogWindow* ui;
};

struct DebugLogModelPrivate;
class DebugLogModel : public QAbstractListModel {
        Q_OBJECT

    public:
        DebugLogModel();
        ~DebugLogModel();

    private:
        DebugLogModelPrivate* d;

        void addLogItem(tLogger::LogItem logItem);

        // QAbstractItemModel interface
    public:
        int rowCount(const QModelIndex& parent = QModelIndex()) const;
        int columnCount(const QModelIndex& parent = QModelIndex()) const;
        QVariant data(const QModelIndex& index, int role) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

};

class LogDelegate : public QStyledItemDelegate {

        Q_OBJECT

        // QAbstractItemDelegate interface
    public:
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // DEBUGLOGWINDOW_H
