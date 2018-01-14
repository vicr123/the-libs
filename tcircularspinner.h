#ifndef TCIRCULARSPINNER_H
#define TCIRCULARSPINNER_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <tvariantanimation.h>

class tCircularSpinner : public QWidget
{
        Q_OBJECT
    public:
        explicit tCircularSpinner(QWidget *parent = nullptr);
        ~tCircularSpinner();

        QSize sizeHint() const;
    signals:

    public slots:
        void appear();
        void disappear();

    private:
        void paintEvent(QPaintEvent* event);

        int rotation = 0;
        int arcLength = 16 * 15;
        QTimer* rotationTimer;
};

#endif // TCIRCULARSPINNER_H
