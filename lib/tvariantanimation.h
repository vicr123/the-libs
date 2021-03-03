#ifndef TVARIANTANIMATION_H
#define TVARIANTANIMATION_H

#include <QVariantAnimation>
#include <QDebug>
#include <functional>
#include "the-libs_global.h"

class THELIBSSHARED_EXPORT tVariantAnimation : public QVariantAnimation {
        Q_OBJECT

    public:
        tVariantAnimation(QObject* parent = nullptr);
        ~tVariantAnimation();

        typedef std::function<void(QVariant)> ValueChangedFunction;
        typedef std::function<void()> FinishedFunction;

        void setForceAnimation(bool force);
        bool forceAnimation();

        static tVariantAnimation* singleShot(QObject* parent, QVariant start, QVariant end, int duration, QEasingCurve easingCurve, ValueChangedFunction valueChangedCallback, FinishedFunction finishedCallback);
        static tVariantAnimation* singleShot(QObject* parent, QVariant start, QVariant end, int duration, QEasingCurve easingCurve, ValueChangedFunction valueChangedCallback);
        static tVariantAnimation* singleShot(QObject* parent, QVariant start, QVariant end, int duration, ValueChangedFunction valueChangedCallback, FinishedFunction finishedCallback);
        static tVariantAnimation* singleShot(QObject* parent, QVariant start, QVariant end, int duration, ValueChangedFunction valueChangedCallback);

    public slots:
        void start(QAbstractAnimation::DeletionPolicy policy = KeepWhenStopped);

    private:
        bool forceAnim = false;
};

#endif // TVARIANTANIMATION_H
