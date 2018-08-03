#include "tcircularspinner.h"

tCircularSpinner::tCircularSpinner(QWidget *parent) : QWidget(parent)
{
    rotationTimer = new QTimer();
    rotationTimer->setInterval(20);
    connect(rotationTimer, &QTimer::timeout, [=] {
        this->rotation -= 16 * 5;
        this->update();
    });
    connect(this, SIGNAL(destroyed(QObject*)), rotationTimer, SLOT(stop()));
    rotationTimer->start();

    this->arcLength = -16 * 5;

    appear();
}

tCircularSpinner::~tCircularSpinner() {
    rotationTimer->deleteLater();
}

QSize tCircularSpinner::sizeHint() const {
    return QSize(32, 32);
}

void tCircularSpinner::appear() {
    rotation += this->arcLength;
    this->arcLength = -this->arcLength;

    /*//Wait 250ms
    QTimer::singleShot(250, [=] {*/
        //Increase arc length to 325 degrees
        tVariantAnimation* a = new tVariantAnimation();
        a->setStartValue(-16 * 5);
        a->setEndValue(-16 * 300);
        a->setDuration(500);
        a->setEasingCurve(QEasingCurve::InOutCubic);
        connect(a, &tVariantAnimation::valueChanged, [=](QVariant value) {
            arcLength = value.toInt();
            this->update();
        });
        connect(this, SIGNAL(destroyed(QObject*)), a, SLOT(stop()));
        connect(this, SIGNAL(destroyed(QObject*)), a, SLOT(deleteLater()));
        connect(a, SIGNAL(finished()), this, SLOT(disappear()));
        connect(a, SIGNAL(finished()), a, SLOT(deleteLater()));
        a->start();
    //});
}

void tCircularSpinner::disappear() {
    rotation += this->arcLength;
    this->arcLength = -this->arcLength;

    //Wait 250ms
    QTimer* timer = new QTimer();
    timer->setInterval(250);
    connect(timer, &QTimer::timeout, [=] {
        timer->deleteLater();

        //Decrease arc length to 15 degrees
        tVariantAnimation* a = new tVariantAnimation();
        a->setStartValue(this->arcLength);
        a->setEndValue(16 * 5);
        a->setDuration(500);
        a->setEasingCurve(QEasingCurve::InOutCubic);
        connect(a, &tVariantAnimation::valueChanged, [=](QVariant value) {
            arcLength = value.toInt();
            this->update();
        });
        connect(this, SIGNAL(destroyed(QObject*)), a, SLOT(stop()));
        connect(this, SIGNAL(destroyed(QObject*)), a, SLOT(deleteLater()));
        connect(a, SIGNAL(finished()), this, SLOT(appear()));
        connect(a, SIGNAL(finished()), a, SLOT(deleteLater()));
        a->start();
    });
    connect(this, SIGNAL(destroyed(QObject*)), timer, SLOT(stop()));
    connect(this, SIGNAL(destroyed(QObject*)), timer, SLOT(deleteLater()));
    timer->start();
}

void tCircularSpinner::paintEvent(QPaintEvent *event) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect spinnerArea;
    spinnerArea.setHeight(qMin(this->width(), this->height()));
    spinnerArea.setWidth(spinnerArea.height());
    spinnerArea.moveLeft(this->width() / 2 - spinnerArea.width() / 2);
    spinnerArea.moveTop(this->height() / 2 - spinnerArea.height() / 2);

    int width = spinnerArea.width() / 12;
    if (width < 2) width = 2;
    int adjFactor = width / 2;
    p.setPen(QPen(this->palette().brush(QPalette::WindowText), width));

    spinnerArea.adjust(adjFactor, adjFactor, -adjFactor, -adjFactor);
    //p.drawArc(spinnerArea, this->rotation, 16 * 45);
    p.drawArc(spinnerArea, this->rotation, this->arcLength);
}
