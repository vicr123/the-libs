#ifndef TSTACKEDWIDGET_H
#define TSTACKEDWIDGET_H

#include <QStackedWidget>
#include <tpropertyanimation.h>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QSettings>
#include <QWindow>
#include <QDebug>
#include <QApplication>

class tStackedWidgetPrivate;

class THELIBSSHARED_EXPORT tStackedWidget : public QStackedWidget {
        Q_OBJECT
        Q_PROPERTY(Animation CurrentAnimation READ CurrentAnimation WRITE setCurrentAnimation NOTIFY CurrentAnimationChanged)

    public:
        explicit tStackedWidget(QWidget* parent = nullptr);
        ~tStackedWidget();

        enum Animation {
            None,
            SlideHorizontal,
            SlideVertical,
            Fade,
            Lift
        };
        Animation CurrentAnimation();

        int addWidget(QWidget* w);
        int insertWidget(int index, QWidget* w);

    signals:
        void switchingFrame(int switchTo);
        void CurrentAnimationChanged(Animation currentAnimation);
        void resized();
        void widgetAdded();

    public slots:
        void setCurrentIndex(int index, bool doAnimation = true);
        void setCurrentWidget(QWidget* w, bool doAnimation = true);
        void setCurrentAnimation(Animation animation);

    private:
        tStackedWidgetPrivate* d;

        void resizeEvent(QResizeEvent* event);

        void doSetCurrentIndex(int index);
};

#endif // ANIMATEDSTACKEDWIDGET_H
