#include "tshortcuthud.h"
#include "ui_tshortcuthud.h"

#include <QLabel>
#include <math.h>

tShortcutHud::tShortcutHud(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tShortcutHud)
{
    ui->setupUi(this);

    this->layout()->setContentsMargins(20 * theLibsGlobal::getDPIScaling(), 1, 20 * theLibsGlobal::getDPIScaling(), 0);

    parent->setMouseTracking(true);
    parent->installEventFilter(this);
    resizeToParent();
}

tShortcutHud::~tShortcutHud()
{
    delete ui;
}

void tintImage(QImage &image, QColor tint) {
    //bool doPaint = true;
    int failNum = 0;
    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            QColor pixelCol = image.pixelColor(x, y);
            //int blue = pixelCol.blue(), green = pixelCol.green(), red = pixelCol.red();
            if ((pixelCol.blue() > pixelCol.green() - 10 && pixelCol.blue() < pixelCol.green() + 10) &&
                    (pixelCol.green() > pixelCol.red() - 10 && pixelCol.green() < pixelCol.red() + 10)) {
            } else {
                failNum++;
                //doPaint = false;
            }
        }
    }

    if (failNum < (image.size().width() * image.size().height()) / 8) {
        QPainter painter(&image);
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(0, 0, image.width(), image.height(), tint);
        painter.end();
    }
}

void tShortcutHud::newShortcut(QShortcut* shortcut, QString shortcutText, ShortcutHudSide side) {
    newShortcut(ShortcutGroup(QList<QShortcut*>() << shortcut, shortcutText, side));
}

void tShortcutHud::newShortcut(ShortcutGroup group) {
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setSpacing(10 * theLibsGlobal::getDPIScaling());

    for (QShortcut* shortcut : group.shortcuts()) {
        QKeySequence key = shortcut->key();
        QString keyText = key.toString();

        connect(shortcut, &QShortcut::activated, [=] {
            this->setVisible(true);
            this->raise();
        });

        QBoxLayout* keyLayout = new QBoxLayout(QBoxLayout::LeftToRight);
        layout->setSpacing(3 * theLibsGlobal::getDPIScaling());
        layout->addLayout(keyLayout);

        QStringList keys = keyText.split("+");
        for (QString key : keys) {
            QLabel* keyLabel = new QLabel();
            keyLabel->setPixmap(getKeyIcon(key));
            keyLayout->addWidget(keyLabel);
        }
    }

    QLabel* shortcutLabel = new QLabel();
    shortcutLabel->setText(group.shortcutText());
    layout->addWidget(shortcutLabel);

    if (group.side() == Leading) {
        ui->leadingLayout->addLayout(layout);
    } else {
        ui->trailingLayout->addLayout(layout);
    }
}

QPixmap tShortcutHud::getKeyIcon(QString key) {
    QPixmap squarePx(QSize(16, 16) * theLibsGlobal::getDPIScaling());
    squarePx.fill(Qt::transparent);

    QPainter sqPainter(&squarePx);
    sqPainter.setRenderHint(QPainter::Antialiasing);
    sqPainter.setPen(Qt::transparent);
    sqPainter.setBrush(this->palette().color(QPalette::WindowText));
    sqPainter.drawRoundedRect(QRect(QPoint(0, 0), squarePx.size()), 50, 50, Qt::RelativeSize);

    QRect squareIconRect;
    squareIconRect.setWidth(12 * theLibsGlobal::getDPIScaling());
    squareIconRect.setHeight(12 * theLibsGlobal::getDPIScaling());
    squareIconRect.moveCenter(QPoint(8, 8) * theLibsGlobal::getDPIScaling());

    if (key == "Left") {
        QImage image = QIcon::fromTheme("go-previous").pixmap(squareIconRect.size()).toImage();
        tintImage(image, this->palette().color(QPalette::Window));
        sqPainter.drawImage(squareIconRect, image);
        return squarePx;
    } else if (key == "Right") {
        QImage image = QIcon::fromTheme("go-next").pixmap(squareIconRect.size()).toImage();
        tintImage(image, this->palette().color(QPalette::Window));
        sqPainter.drawImage(squareIconRect, image);
        return squarePx;
    } else {
        QFont font = this->font();
        QFontMetrics fontMetrics(font);

        //font.setPointSizeF(floor(8));
        while (QFontMetrics(font).height() > 14 * theLibsGlobal::getDPIScaling()) {
            font.setPointSizeF(font.pointSizeF() - 0.5);
        }

        QSize pixmapSize;
        pixmapSize.setHeight(16 * theLibsGlobal::getDPIScaling());
        pixmapSize.setWidth(qMax(fontMetrics.width(key) + 6 * theLibsGlobal::getDPIScaling(), 16 * theLibsGlobal::getDPIScaling()));

        QPixmap px(pixmapSize);
        px.fill(Qt::transparent);

        QPainter painter(&px);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::transparent);
        painter.setBrush(this->palette().color(QPalette::WindowText));
        painter.drawRoundedRect(QRect(QPoint(0, 0), px.size()), 4 * theLibsGlobal::getDPIScaling(), 4 * theLibsGlobal::getDPIScaling());

        painter.setFont(font);
        painter.setPen(this->palette().color(QPalette::Window));

        QRect textRect;
        textRect.setHeight(fontMetrics.height());
        textRect.setWidth(fontMetrics.width(key));
        textRect.moveCenter(QPoint(pixmapSize.width() / 2, pixmapSize.height() / 2));

        painter.drawText(textRect, Qt::AlignCenter, key);

        painter.end();
        return px;
    }
}

bool tShortcutHud::eventFilter(QObject* watched, QEvent* event) {
    if (watched == this->parent()) {
        if (event->type() == QEvent::KeyPress) {
            this->setVisible(true);
            this->raise();
        } else if (event->type() == QEvent::MouseMove) {
            this->setVisible(false);
        } else if (event->type() == QEvent::Resize) {
            resizeToParent();
        }
    }
    return false;
}

void tShortcutHud::resizeToParent() {
    this->setFixedHeight(32 * theLibsGlobal::getDPIScaling());
    this->setFixedWidth(this->parentWidget()->width());
    this->move(0, this->parentWidget()->height() - this->height());
}

void tShortcutHud::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(this->palette().color(QPalette::WindowText));
    painter.drawLine(0, 0, this->width(), 0);
}


tShortcutHud::ShortcutGroup::ShortcutGroup() {

}

tShortcutHud::ShortcutGroup::ShortcutGroup(QList<QShortcut*> shortcuts, QString shortcutText, ShortcutHudSide side) {
    this->scs = shortcuts;
    this->st = shortcutText;
    this->sd = side;
}

void tShortcutHud::ShortcutGroup::addShortcut(QShortcut* shortcut) {
    scs.append(shortcut);
}

void tShortcutHud::ShortcutGroup::setText(QString text) {
    st = text;
}

void tShortcutHud::ShortcutGroup::setSize(ShortcutHudSide side) {
    this->sd = side;
}

QList<QShortcut*> tShortcutHud::ShortcutGroup::shortcuts() {
    return scs;
}

QString tShortcutHud::ShortcutGroup::shortcutText() {
    return st;
}

tShortcutHud::ShortcutHudSide tShortcutHud::ShortcutGroup::side() {
    return sd;
}
