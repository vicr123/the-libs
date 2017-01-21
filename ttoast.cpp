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

    announceActionWidget = new QWidget;
    announceActionWidget->setAutoFillBackground(true);
    QPalette announcePal = announceActionWidget->palette();
    announcePal.setColor(QPalette::Window, announcePal.color(QPalette::Highlight));
    announcePal.setColor(QPalette::WindowText, announcePal.color(QPalette::HighlightedText));
    announceActionWidget->setPalette(announcePal);

    QBoxLayout* announceActionLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    announceActionWidget->setLayout(announceActionLayout);

    QLabel* tickLabel = new QLabel;
    tickLabel->setPixmap(QIcon::fromTheme("dialog-ok").pixmap(16, 16));
    announceActionLayout->addWidget(tickLabel);

    announceActionLabel = new QLabel;
    announceActionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    announceActionLayout->addWidget(announceActionLabel);

    hideTimer = new tVariantAnimation();
    hideTimer->setStartValue(0);
    hideTimer->setDuration(5000);
    connect(hideTimer, &tVariantAnimation::valueChanged, [=](const QVariant &value) {
        currentAnimationValue = value.toInt();
        toastWidget->update();
    });
    hideTimer->setForceAnimation(true);
    connect(hideTimer, &tVariantAnimation::finished, [=]() {
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

    hideTimer->setEndValue(parent->width());
    hideTimer->start();
}

void tToast::dismiss() {
    QWidget* parent = toastWidget->parentWidget();
    if (parent != NULL) {
        if (!timerStopped) {
            hideTimer->stop();
            timerStopped = true;
        }
        tPropertyAnimation* anim = new tPropertyAnimation(toastWidget, "geometry");
        anim->setStartValue(toastWidget->geometry());
        anim->setEndValue(QRect(0, parent->height(), parent->width(), toastWidget->height()));
        anim->setDuration(500);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        connect(anim, SIGNAL(finished()), anim, SLOT(deleteLater()));
        connect(anim, SIGNAL(finished()), this, SIGNAL(dismissed()));
        anim->start();
    }
}

void tToast::announceAction(QString text) {
    if (canAnnounceAction) {
        announceActionLabel->setText(text);

        int width;
        if (buttons->sizeHint().width() < announceActionLabel->fontMetrics().width(text) + 30) {
            width = announceActionLabel->fontMetrics().width(text) + 30;
        } else {
            width = buttons->sizeHint().width();
        }

        width += 15;

        announceActionWidget->setGeometry(toastWidget->width(), 1, width, toastWidget->height() - 1);
        announceActionWidget->setFixedHeight(toastWidget->height() - 1);
        announceActionWidget->setParent(toastWidget);
        announceActionWidget->setVisible(true);

        tPropertyAnimation* anim = new tPropertyAnimation(announceActionWidget, "geometry");
        anim->setStartValue(announceActionWidget->geometry());
        anim->setEndValue(QRect(toastWidget->width() - width, 1, width, toastWidget->height() - 1));
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
            hideTimer->stop();
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
    return actn;
}

void tToast::setTimeout(int milliseconds) {
    hideTimer->setDuration(milliseconds);
}

int tToast::timeout() {
    return hideTimer->duration();
}

bool tToast::eventFilter(QObject *watched, QEvent *event) {
    if (watched == toastWidget) {
        if (event->type() == QEvent::Paint) {
            QPaintEvent* pEvent = (QPaintEvent*) event;
            QPainter painter(toastWidget);

            QRect rect = pEvent->rect();

            painter.setBrush(toastWidget->palette().color(QPalette::Window));
            painter.setPen(Qt::transparent);
            painter.drawRect(rect);

            QColor highlightCol = toastWidget->palette().color(QPalette::Window);
            int average = (highlightCol.red() + highlightCol.green() + highlightCol.blue()) / 3;
            if (average < 127) { //Dark color
                highlightCol = highlightCol.light(150);
            } else {
                highlightCol = highlightCol.dark(150);
            }
            painter.setBrush(highlightCol);
            painter.drawRect(rect.left(), rect.top(), currentAnimationValue, rect.height());

            painter.setPen(toastWidget->palette().color(QPalette::WindowText));
            painter.drawLine(rect.topLeft(), rect.topRight());
        }
    }
    return false;
}
