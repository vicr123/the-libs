#include "ttoast.h"

struct tToastPrivate {
    QMap<QString, QString> actn;

    QWidget *toastWidget, *announceActionWidget;
    QLabel *titleLabel, *textLabel, *announceActionLabel;
    QBoxLayout* buttons;

    tVariantAnimation* hideTimer;
    int currentAnimationValue;
};

tToast::tToast(QObject *parent) : QObject(parent)
{
    d = new tToastPrivate();
    d->toastWidget = new QWidget;
    d->toastWidget->installEventFilter(this);
    d->toastWidget->setAttribute(Qt::WA_TranslucentBackground);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    d->toastWidget->setLayout(layout);

    d->titleLabel = new QLabel;

    QFont titleFont = d->titleLabel->font();
    titleFont.setBold(true);
    d->titleLabel->setFont(titleFont);
    layout->addWidget(d->titleLabel);

    d->textLabel = new QLabel;
    d->textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(d->textLabel);

    d->buttons = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->addLayout(d->buttons);

    d->announceActionWidget = new QWidget;
    d->announceActionWidget->setAutoFillBackground(true);
    QPalette announcePal = d->announceActionWidget->palette();
    announcePal.setColor(QPalette::Window, announcePal.color(QPalette::Highlight));
    announcePal.setColor(QPalette::WindowText, announcePal.color(QPalette::HighlightedText));
    d->announceActionWidget->setPalette(announcePal);

    QBoxLayout* announceActionLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    d->announceActionWidget->setLayout(announceActionLayout);

    QLabel* tickLabel = new QLabel;
    tickLabel->setPixmap(QIcon::fromTheme("dialog-ok").pixmap(16, 16));
    announceActionLayout->addWidget(tickLabel);

    d->announceActionLabel = new QLabel;
    d->announceActionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    announceActionLayout->addWidget(d->announceActionLabel);

    d->hideTimer = new tVariantAnimation();
    d->hideTimer->setStartValue(0);
    d->hideTimer->setDuration(5000);
    connect(d->hideTimer, &tVariantAnimation::valueChanged, [=](const QVariant &value) {
        d->currentAnimationValue = value.toInt();
        d->toastWidget->update();
    });
    d->hideTimer->setForceAnimation(true);
    connect(d->hideTimer, &tVariantAnimation::finished, [=]() {
        timerStopped = true;

        canAnnounceAction = true;
        emit doDefaultOption();
        canAnnounceAction = false;

        if (!announcingAction) {
            this->dismiss();
        }
    });
}

tToast::~tToast() {
    d->textLabel->deleteLater();
    d->titleLabel->deleteLater();
    d->toastWidget->deleteLater();
    delete d;
}

void tToast::show(QWidget *parent) {
    int height = d->toastWidget->sizeHint().height();
    d->toastWidget->setFixedHeight(height);
    d->toastWidget->setParent(parent);
    d->toastWidget->setVisible(true);
    d->toastWidget->raise();
    updateToastGeometry();

    parent->installEventFilter(this);

    tPropertyAnimation* anim = new tPropertyAnimation(d->toastWidget, "geometry");
    anim->setStartValue(d->toastWidget->geometry());
    anim->setEndValue(QRect(0, parent->height() - height, parent->width(), height));
    anim->setDuration(750);
    anim->setEasingCurve(QEasingCurve::OutBounce);
    connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
    anim->start();

    d->hideTimer->setEndValue(parent->width());
    d->hideTimer->start();
}

void tToast::dismiss() {
    QWidget* parent = d->toastWidget->parentWidget();
    if (parent != NULL) {
        if (!timerStopped) {
            d->hideTimer->stop();
            timerStopped = true;
        }
        tPropertyAnimation* anim = new tPropertyAnimation(d->toastWidget, "geometry");
        anim->setStartValue(d->toastWidget->geometry());
        anim->setEndValue(QRect(0, parent->height(), parent->width(), d->toastWidget->height()));
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
        connect(anim, SIGNAL(finished()), this, SIGNAL(dismissed()));
        anim->start();
        parent->removeEventFilter(this);
    }
}

void tToast::announceAction(QString text) {
    if (canAnnounceAction) {
        d->announceActionLabel->setText(text);

        int width;
        if (d->buttons->sizeHint().width() < d->announceActionLabel->fontMetrics().width(text) + 30) {
            width = d->announceActionLabel->fontMetrics().width(text) + 30;
        } else {
            width = d->buttons->sizeHint().width();
        }

        width += 15;

        d->announceActionWidget->setGeometry(d->toastWidget->width(), 1, width, d->toastWidget->height() - 1);
        d->announceActionWidget->setFixedHeight(d->toastWidget->height() - 1);
        d->announceActionWidget->setParent(d->toastWidget);
        d->announceActionWidget->setVisible(true);

        tPropertyAnimation* anim = new tPropertyAnimation(d->announceActionWidget, "geometry");
        anim->setStartValue(d->announceActionWidget->geometry());
        anim->setEndValue(QRect(d->toastWidget->width() - width, 1, width, d->toastWidget->height() - 1));
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
        anim->start();

        QTimer::singleShot(3000, [=]() {
            /*announceActionWidget->setVisible(false);
            announceActionWidget->setParent(NULL);*/
            announcingAction = false;
            this->dismiss();
        });

        announcingAction = true;
    }
}

void tToast::setTitle(QString title) {
    d->titleLabel->setText(title);
}

QString tToast::title() {
    return d->titleLabel->text();
}

void tToast::setText(QString text) {
    d->textLabel->setText(text);
}

QString tToast::text() {
    return d->textLabel->text();
}

void tToast::setActions(QMap<QString, QString> actions) {
    d->actn = actions;

    QLayoutItem* deleteButton;
    deleteButton = d->buttons->takeAt(0);
    while (deleteButton != NULL) {
        deleteButton->widget()->deleteLater();
        deleteButton = d->buttons->takeAt(0);
    }

    for (QString key : actions.keys()) {
        QString text = actions.value(key);

        QPushButton* button = new QPushButton;
        button->setText(text);
        d->buttons->addWidget(button);

        connect(button, &QPushButton::clicked, [=]() {
            d->hideTimer->stop();
            timerStopped = true;

            canAnnounceAction = true;
            emit actionClicked(key);
            canAnnounceAction = false;

            if (!announcingAction) {
                this->dismiss();
            }
        });
    }
}

QMap<QString, QString> tToast::actions() {
    return d->actn;
}

void tToast::setTimeout(int milliseconds) {
    d->hideTimer->setDuration(milliseconds);
}

int tToast::timeout() {
    return d->hideTimer->duration();
}

bool tToast::eventFilter(QObject *watched, QEvent *event) {
    if (watched == d->toastWidget) {
        if (event->type() == QEvent::Paint) {
            QPaintEvent* pEvent = (QPaintEvent*) event;
            QPainter painter(d->toastWidget);


#ifdef Q_OS_MAC
            QRect rect = pEvent->rect().adjusted(5, 10, -5, -10);
            QColor bgCol = d->toastWidget->palette().color(QPalette::Window);
            bgCol.setAlpha(200);
            painter.setBrush(bgCol);
            painter.setPen(Qt::transparent);
            painter.drawRoundedRect(rect, 5, 5);
#else
            QRect rect = pEvent->rect();

            painter.setBrush(d->toastWidget->palette().color(QPalette::Window));
            painter.setPen(Qt::transparent);
            painter.drawRect(rect);

            QColor highlightCol = d->toastWidget->palette().color(QPalette::Window);
            int average = (highlightCol.red() + highlightCol.green() + highlightCol.blue()) / 3;
            if (average < 127) { //Dark color
                highlightCol = highlightCol.light(150);
            } else {
                highlightCol = highlightCol.dark(150);
            }
            painter.setBrush(highlightCol);
            painter.drawRect(rect.left(), rect.top(), d->currentAnimationValue, rect.height());

            painter.setPen(d->toastWidget->palette().color(QPalette::WindowText));
            painter.drawLine(rect.topLeft(), rect.topRight());
#endif
        }
    } else if (watched == d->toastWidget->parentWidget()) {
        if (event->type() == QEvent::Resize) {
            updateToastGeometry();
        }
    }
    return false;
}

void tToast::updateToastGeometry() {
    d->toastWidget->setGeometry(0, d->toastWidget->parentWidget()->height() - d->toastWidget->sizeHint().height(), d->toastWidget->parentWidget()->width(), d->toastWidget->sizeHint().height());
}
