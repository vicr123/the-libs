#ifndef TPROMISE_H
#define TPROMISE_H

#include <QObject>
#include <functional>
#include <QtConcurrent/QtConcurrent>
#include <the-libs_global.h>

#define TPROMISE_CALL_NO_ARGS(fn) QMetaObject::invokeMethod(this, fn, Qt::QueuedConnection)
#define TPROMISE_CALL(fn, ...) QMetaObject::invokeMethod(this, fn, Qt::QueuedConnection, __VA_ARGS__)

template <typename T> class THELIBSSHARED_EXPORT tPromise
{
    public:
        explicit tPromise(std::function<T(QString&)> functionToRun);

        tPromise<T>* then(std::function<void(T)> functionToRunAfterSuccess);
        tPromise<T>* error(std::function<void(QString)> functionToRunOnFailure);

        bool isResolved();
        bool isErrored();
        bool isPending();

    private:
        bool resolved = false;
        bool errored = false;
        bool functionSetToRunAfterSuccess = false;
        bool functionSetToRunAfterFailure = false;
        T resolvedValue;
        QFuture<T> runFuture;
        QString errorString = "";

        std::function<void (T)> fnAfterSuccess;
        std::function<void (QString)> fnAfterFailure;
        QFutureWatcher<T>* runFutureWatcher;
};

#endif // TPROMISE_H
