#include "csdbuttonbox.h"
#include "ui_csdbuttonbox.h"

#include <QWindow>
#import <AppKit/AppKit.h>

void CsdButtonBox::setupMacOs() {
    ui->closeButton->setVisible(false);
    ui->maxButton->setVisible(false);
    ui->minButton->setVisible(false);

    ui->macWindowControlsLayout->setContentsMargins(6, 0, 6, 0);
    ui->macWindowControlsLayout->setSpacing(6);

    NSWindowStyleMask styleMask = NSWindowStyleMaskFullSizeContentView|NSWindowStyleMaskResizable|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable;

    NSButton* closeButton = [NSWindow standardWindowButton:NSWindowCloseButton forStyleMask:styleMask];
    QWidget* closeWidget = QWidget::createWindowContainer(QWindow::fromWinId(reinterpret_cast<WId>(closeButton)), this);
    closeWidget->setFixedSize(14, 16);
    ui->macWindowControlsLayout->addWidget(closeWidget);

    NSButton* minButton = [NSWindow standardWindowButton:NSWindowMiniaturizeButton forStyleMask:styleMask];
    QWidget* minWidget = QWidget::createWindowContainer(QWindow::fromWinId(reinterpret_cast<WId>(minButton)), this);
    minWidget->setFixedSize(14, 16);
    ui->macWindowControlsLayout->addWidget(minWidget);

    NSButton* fsButton = [NSWindow standardWindowButton:NSWindowZoomButton forStyleMask:styleMask];
    QWidget* fsWidget = QWidget::createWindowContainer(QWindow::fromWinId(reinterpret_cast<WId>(fsButton)), this);
    fsWidget->setFixedSize(14, 16);
    ui->macWindowControlsLayout->addWidget(fsWidget);
}
