#include "taboutdialog.h"
#include "ui_taboutdialog.h"

#include <QToolButton>
#include <QScroller>
#include "tapplication.h"

tAboutDialog::tAboutDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::tAboutDialog) {
    ui->setupUi(this);

    this->setWindowTitle((tr("About %1").arg(tApplication::applicationName())));
    ui->titleLabel->setText(tr("About %1").arg(tApplication::applicationName()));
    ui->applicationTitle->setText(tApplication::applicationName() + " " + tApplication::applicationVersion());
    ui->genericName->setText(tApplication::genericName());
    ui->verticalSpacer->changeSize(0, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);

    QPixmap splashGraphic = tApplication::aboutDialogSplashGraphic();
    bool graphicAvailable;
    if (splashGraphic.isNull()) {
        ui->splashGraphicLabel->setVisible(false);
        this->setFixedSize(SC_DPI_T(QSize(430, 340), QSize));
        graphicAvailable = false;
    } else {
        ui->splashGraphicLabel->setPixmap(splashGraphic);
        this->setFixedSize(SC_DPI_T(QSize(530, 340), QSize));
        graphicAvailable = true;
    }

    QList<QPair<QString, QString>> softwareVersions = tApplication::versions();
    for (int i = 0; i < softwareVersions.count(); i++) {
        QLabel* label1 = new QLabel(this);
        label1->setText(softwareVersions.at(i).first);
        ui->versionsLayout->addWidget(label1, i + 1, 0);

        QLabel* label2 = new QLabel(this);
        label2->setText(softwareVersions.at(i).second);
        label2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        ui->versionsLayout->addWidget(label2, i + 1, 1);
    }

    QFont copyrightFont = this->font();
    if (copyrightFont.pixelSize() == -1) {
        copyrightFont.setPointSizeF(copyrightFont.pointSizeF() * 0.75);
    } else {
        copyrightFont.setPixelSize(copyrightFont.pixelSize() * 0.75);
    }
    QStringList copyrightLines = tApplication::copyrightLines();
    for (int i = 0; i < copyrightLines.count(); i++) {
        QLabel* label = new QLabel(this);
        label->setText(copyrightLines.at(i));
        label->setWordWrap(true);
        label->setFont(copyrightFont);
        ui->copyrightLayout->addWidget(label);
    }

#ifndef Q_OS_MAC
    QToolButton* backButton = new QToolButton(this);
    backButton->setAutoRaise(true);
    backButton->setIcon(QIcon::fromTheme("go-previous", QIcon(":/the-libs/icons/go-previous.svg")));
    if (graphicAvailable) {
        backButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        backButton->setParent(ui->splashGraphicLabel);

        if (this->layoutDirection() == Qt::LeftToRight) {
            backButton->move(0, 0);
        } else {
            backButton->move(ui->splashGraphicLabel->width() - backButton->width(), 0);
        }
    } else {
        backButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        ui->titleLayout->insertWidget(0, backButton);
    }
    connect(backButton, &QToolButton::clicked, this, &tAboutDialog::on_okButton_clicked);

    ui->macOkPanel->setVisible(false);
#endif

    QScroller::grabGesture(ui->scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
}

tAboutDialog::~tAboutDialog() {
    delete ui;
}

void tAboutDialog::on_okButton_clicked() {
    this->close();
}
