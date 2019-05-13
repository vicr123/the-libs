#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H

#include <QAbstractNativeEventFilter>
#include <QObject>

class NativeEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
    public:
        explicit NativeEventFilter(QObject *parent = nullptr);

        bool nativeEventFilter(const QByteArray& eventType, void* message, long* result);
    signals:
        void powerStretchChanged(bool powerStretchOn);

    public slots:
};

#endif // NATIVEEVENTFILTER_H
