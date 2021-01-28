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
#include "thelpmenu.h"

#include "tapplication.h"
#include "taboutdialog.h"
#include "tlogger.h"
#include <QDesktopServices>
#include <QUrl>

struct tHelpMenuPrivate {
    QAction* helpContentsAction;
    QAction* fileBugAction;
    QAction* sourcesAction;
    QAction* debugLogAction;
    QAction* aboutAction;

    QAction* userActionsSeperator;
};

tHelpMenu::tHelpMenu(QWidget* parent) : QMenu(parent) {
    d = new tHelpMenuPrivate();
    d->helpContentsAction = new QAction(QIcon::fromTheme("help-contents"), tr("%1 Help").arg(tApplication::applicationDisplayName()), this);
    d->helpContentsAction->setShortcut(QKeySequence(Qt::Key_F1));

    d->fileBugAction = new QAction(QIcon::fromTheme("tools-report-bug"), tr("File Bug"), this);
    d->sourcesAction = new QAction(QIcon::fromTheme("commit"), tr("Sources"), this);
    d->debugLogAction = new QAction(tr("View Debug Log"), this);
    d->aboutAction = new QAction(QIcon::fromTheme("help-about"), tApplication::translate("MAC_APPLICATION_MENU", "About %1").arg(tApplication::applicationDisplayName()), this);

    connect(d->helpContentsAction, &QAction::triggered, this, [ = ] {
        QDesktopServices::openUrl(tApplication::applicationUrl(tApplication::HelpContents));
    });
    connect(d->fileBugAction, &QAction::triggered, this, [ = ] {
        QDesktopServices::openUrl(tApplication::applicationUrl(tApplication::FileBug));
    });
    connect(d->sourcesAction, &QAction::triggered, this, [ = ] {
        QDesktopServices::openUrl(tApplication::applicationUrl(tApplication::Sources));
    });
    connect(d->debugLogAction, &QAction::triggered, this, [ = ] {
        tLogger::openDebugLogWindow();
    });
    connect(d->aboutAction, &QAction::triggered, this, [ = ] {
        tAboutDialog dialog(parent);
        dialog.exec();
    });

    d->helpContentsAction->setVisible(tApplication::haveApplicationUrl(tApplication::HelpContents));
    d->fileBugAction->setVisible(tApplication::haveApplicationUrl(tApplication::FileBug));
    d->sourcesAction->setVisible(tApplication::haveApplicationUrl(tApplication::Sources));

    d->aboutAction->setMenuRole(QAction::AboutRole);

    qInfo() << "";

    this->setTitle(tr("Help"));
    this->setIcon(QIcon::fromTheme("help-contents"));

    this->QMenu::addAction(d->helpContentsAction);
    this->addSeparator();
    d->userActionsSeperator = this->addSeparator();
    this->QMenu::addAction(d->fileBugAction);
    this->QMenu::addAction(d->sourcesAction);
    this->QMenu::addAction(d->debugLogAction);
    this->addSeparator();
    this->QMenu::addAction(d->aboutAction);
}

tHelpMenu::~tHelpMenu() {
    delete d;
}

void tHelpMenu::addAction(QAction* action) {
    this->insertAction(d->userActionsSeperator, action);
}
