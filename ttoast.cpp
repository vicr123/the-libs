#include "ttoast.h"

tToast::tToast(QObject *parent) : QObject(parent)
{
    widget = new QWidget;
    widget->installEventFilter(this);
    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    widget->setLayout(layout);

    titleLabel = new QLabel;
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    textLabel = new QLabel;
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(textLabel);

    buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->addLayout(buttonLayout);

    timer = new tVariantAnimation;
    timer->setStartValue(0);
    timer->setDuration(5000);
    timer->setForceAnimation(true);
    connect(timer, &tVariantAnimation::valueChanged, [=](QVariant value) {
        timerProg = value.toInt();
        widget->update();
    });
    connect(timer, SIGNAL(finished()), this, SLOT(dismiss()));
}

tToast::~tToast() {
    widget->deleteLater();
    timer->deleteLater();
}

void tToast::setTitle(QString title) {
    this->titleLabel->setText(title);
}

void tToast::setText(QString text) {
    this->textLabel->setText(text);
}

void tToast::setActions(QMap<QString, QString> actions) {
    this->actn = actions;

    QLayoutItem* w = buttonLayout->takeAt(0);
    while (w != NULL) {
        w->widget()->deleteLater();
        w = buttonLayout->takeAt(0);
    }

    for (QString key : actions.keys()) {
        QString value = actions.value(key);

        QPushButton* button = new QPushButton;
        button->setText(value);
        connect(button, &QPushButton::clicked, [=]() {
            emit actionClicked(key);
            dismiss();
        });
        buttonLayout->addWidget(button);
    }
}

void tToast::setTimeout(int timeout) {
    timer->setDuration(timeout);
}

void tToast::show(QWidget* parent) {
    this->parent = parent;
    widget->setParent(parent);

    int height = widget->sizeHint().height();
    widget->setGeometry(0, parent->height(), parent->width(), height);
    widget->setVisible(true);
    widget->setFixedHeight(height);
    widget->setFixedWidth(parent->width());

    tPropertyAnimation* anim = new tPropertyAnimation(widget, "geometry");
    anim->setStartValue(widget->geometry());
    anim->setEndValue(QRect(0, parent->height() - height, parent->width(), height));
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::OutBounce);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();

    timer->setEndValue(parent->width());
    timer->start();
}

void tToast::announceAction(QString action) {

}

void tToast::dismiss() {
    timer->stop();

    tPropertyAnimation* anim = new tPropertyAnimation(widget, "geometry");
    anim->setStartValue(widget->geometry());
    anim->setEndValue(QRect(0, parent->height(), parent->width(), widget->height()));
    anim->setDuration(500);
    anim->setEasingCurve(QEasingCurve::InCubic);
    connect(anim, &tPropertyAnimation::finished, [=]() {
        widget->setVisible(false);
        emit dismissed();
    });
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();
}

bool tToast::eventFilter(QObject *watched, QEvent *event) {
    if (watched == widget) {
        if (event->type() == QEvent::Paint) {
            QPainter painter(widget);
            QPaintEvent* ev = (QPaintEvent*) event;

            QRect rect = ev->rect();

            QPalette pal = widget->palette();
            painter.setPen(Qt::transparent);
            painter.setBrush(pal.color(QPalette::Window));
            painter.drawRect(rect);

            painter.setBrush(pal.color(QPalette::Window).lighter(150));
            painter.drawRect(rect.left(), rect.top(), timerProg, rect.height());

            painter.setPen(pal.color(QPalette::WindowText));
            painter.drawLine(rect.topLeft(), rect.topRight());
            return false;
        }
    }
    return false;
}
