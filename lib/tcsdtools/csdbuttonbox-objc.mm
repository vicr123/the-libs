#include "csdbuttonbox.h"
#include "csdbuttonbox_p.h"
#include "ui_csdbuttonbox.h"

#include <QWindow>
#import <AppKit/AppKit.h>

void CsdButtonBoxPrivate::windowHidden() {
    box->ui->macWindowControlsLayout->removeWidget(closeWidget);
    box->ui->macWindowControlsLayout->removeWidget(minWidget);
    box->ui->macWindowControlsLayout->removeWidget(fsWidget);

    closeWidget->deleteLater();
    minWidget->deleteLater();
    fsWidget->deleteLater();
}

void CsdButtonBoxPrivate::windowShown() {
    NSWindowStyleMask styleMask = NSWindowStyleMaskFullSizeContentView|NSWindowStyleMaskResizable|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable;

    NSButton* closeButton = [NSWindow standardWindowButton:NSWindowCloseButton forStyleMask:styleMask];
    closeWidget = QWidget::createWindowContainer(QWindow::fromWinId(reinterpret_cast<WId>(closeButton)), box);
    closeWidget->setFixedSize(14, 16);
    box->ui->macWindowControlsLayout->addWidget(closeWidget);

    NSButton* minButton = [NSWindow standardWindowButton:NSWindowMiniaturizeButton forStyleMask:styleMask];
    minWidget = QWidget::createWindowContainer(QWindow::fromWinId(reinterpret_cast<WId>(minButton)), box);
    minWidget->setFixedSize(14, 16);
    box->ui->macWindowControlsLayout->addWidget(minWidget);

    NSButton* fsButton = [NSWindow standardWindowButton:NSWindowZoomButton forStyleMask:styleMask];
    fsWidget = QWidget::createWindowContainer(QWindow::fromWinId(reinterpret_cast<WId>(fsButton)), box);
    fsWidget->setFixedSize(14, 16);
    box->ui->macWindowControlsLayout->addWidget(fsWidget);
}

void CsdButtonBox::setupMacOs() {
    ui->closeButton->setVisible(false);
    ui->maxButton->setVisible(false);
    ui->minButton->setVisible(false);

    ui->macWindowControlsLayout->setContentsMargins(6, 0, 6, 0);
    ui->macWindowControlsLayout->setSpacing(6);
}
