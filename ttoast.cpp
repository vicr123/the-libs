#include "ttoast.h"

tToast::tToast(QObject *parent) : QObject(parent)
{
    toastWidget = new QWidget;
    toastWidget->installEventFilter(this);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    toastWidget->setLayout(layout);

    titleLabel = new QLabel;

    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    layout->addWidget(titleLabel);

    textLabel = new QLabel;
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(textLabel);

    buttons = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->addLayout(buttons);

    hideTimer = new QTimer();
    hideTimer->setInterval(5000);
    hideTimer->setSingleShot(true);
    connect(hideTimer, SIGNAL(timeout()), this, SLOT(dismiss()));
}

tToast::~tToast() {
    textLabel->deleteLater();
    titleLabel->deleteLater();
    toastWidget->deleteLater();
}

void tToast::show(QWidget *parent) {
    int height = toastWidget->sizeHint().height();
    toastWidget->setGeometry(0, parent->height(), parent->width(), height);
    toastWidget->setFixedHeight(height);
    toastWidget->setParent(parent);
    toastWidget->setVisible(true);
    toastWidget->raise();

    tPropertyAnimation* anim = new tPropertyAnimation(toastWidget, "geometry");
    anim->setStartValue(toastWidget->geometry());
    anim->setEndValue(QRect(0, parent->height() - height, parent->width(), height));
    anim->setDuration(750);
    anim->setEasingCurve(QEasingCurve::OutBounce);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();

    hideTimer->start();
}

void tToast::dismiss() {
    QWidget* parent = toastWidget->parentWidget();
    if (parent != NULL) {
        hideTimer->stop();
        tPropertyAnimation* anim = new tPropertyAnimation(toastWidget, "geometry");
        anim->setStartValue(toastWidget->geometry());
        anim->setEndValue(QRect(0, parent->height(), parent->width(), toastWidget->height()));
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::InCubic);
        connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
        connect(anim, SIGNAL(finished()), this, SIGNAL(dismissed()));
        anim->start();
    }
}

void tToast::setTitle(QString title) {
    this->titleLabel->setText(title);
}

QString tToast::title() {
    return this->titleLabel->text();
}

void tToast::setText(QString text) {
    this->textLabel->setText(text);
}

QString tToast::text() {
    return this->textLabel->text();
}

void tToast::setActions(QMap<QString, QString> actions) {
    actn = actions;

    QLayoutItem* deleteButton;
    deleteButton = buttons->takeAt(0);
    while (deleteButton != NULL) {
        deleteButton->widget()->deleteLater();
        deleteButton = buttons->takeAt(0);
    }

    for (QString key : actions.keys()) {
        QString text = actions.value(key);

        QPushButton* button = new QPushButton;
        button->setText(text);
        buttons->addWidget(button);

        connect(button, &QPushButton::clicked, [=]() {
            this->dismiss();
            emit actionClicked(key);
        });
    }


}

QMap<QString, QString> tToast::actions() {
    return actn;
}

bool tToast::eventFilter(QObject *watched, QEvent *event) {
    if (watched == toastWidget) {
        if (event->type() == QEvent::Paint) {
            QPaintEvent* pEvent = (QPaintEvent*) event;
            QPainter painter(toastWidget);
            painter.setBrush(toastWidget->palette().color(QPalette::Window));
            painter.setPen(Qt::transparent);
            painter.drawRect(pEvent->rect());
            painter.setPen(toastWidget->palette().color(QPalette::WindowText));
            painter.drawLine(pEvent->rect().topLeft(), pEvent->rect().topRight());
        }
    }
    return false;
}
