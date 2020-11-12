#ifndef TPROMISE_H
#define TPROMISE_H

#include <QObject>
#include <functional>
#include <QtConcurrent/QtConcurrent>
#include <the-libs_global.h>
#include <QFuture>

#define TPROMISE_CALL_NO_ARGS(fn) QMetaObject::invokeMethod(this, fn, Qt::QueuedConnection)
#define TPROMISE_CALL(fn, ...) QMetaObject::invokeMethod(this, fn, Qt::QueuedConnection, __VA_ARGS__)

#define TPROMISE_CREATE_SAME_THREAD_WITH_CALLBACK_NAMES(type, successCallback, failureCallback, fn) tPromise<type>::runOnSameThread([=](tPromiseFunctions<type>::SuccessFunction successCallback, tPromiseFunctions<type>::FailureFunction failureCallback) fn)
#define TPROMISE_CREATE_SAME_THREAD(type, fn) TPROMISE_CREATE_SAME_THREAD_WITH_CALLBACK_NAMES(type, res, rej, fn)

#define TPROMISE_CREATE_NEW_THREAD_WITH_CALLBACK_NAMES(type, successCallback, failureCallback, fn) tPromise<type>::runOnNewThread([=](tPromiseFunctions<type>::SuccessFunction successCallback, tPromiseFunctions<type>::FailureFunction failureCallback) fn)
#define TPROMISE_CREATE_NEW_THREAD(type, fn) TPROMISE_CREATE_NEW_THREAD_WITH_CALLBACK_NAMES(type, res, rej, fn)

template <typename T> class tPromise;

template<typename T> struct tPromiseResults {
    T result;
    QString error = "";
};

template<> struct tPromiseResults<void> {
    QString error = "";
};

template<typename T> struct tPromiseFunctions {
    using SuccessFunction = std::function<void(T)>;
    using FailureFunction = std::function<void(QString)>;
};

template<> struct tPromiseFunctions<void> {
    using SuccessFunction = std::function<void()>;
    using FailureFunction = std::function<void(QString)>;
};

template <typename T> struct tPromisePrivate {
    enum State {
        Pending,
        Resolved,
        Errored
    };

    using RunFunction = std::function<T(QString&)>;
    using RunAsyncFunction = std::function<void(typename tPromiseFunctions<T>::SuccessFunction, typename tPromiseFunctions<T>::FailureFunction)>;

    State state = Pending;
    bool functionSetToRunAfterSuccess = false;
    bool functionSetToRunAfterFailure = false;
    bool deleteAfter = false;
    tPromiseResults<T> resolvedValue;
    QFuture<void> runFuture;

    typename tPromiseFunctions<T>::SuccessFunction fnAfterSuccess;
    typename tPromiseFunctions<T>::FailureFunction fnAfterFailure;

    void callSuccessFunction() {
        fnAfterSuccess(resolvedValue.result);
    }

    void callFailureFunction() {
        fnAfterFailure(resolvedValue.error);
    }
};

template<> struct tPromisePrivate<void> {
    enum State {
        Pending,
        Resolved,
        Errored
    };

    using RunFunction = std::function<void(QString&)>;
    using RunAsyncFunction = std::function<void(tPromiseFunctions<void>::SuccessFunction, tPromiseFunctions<void>::FailureFunction)>;

    State state = Pending;
    bool functionSetToRunAfterSuccess = false;
    bool functionSetToRunAfterFailure = false;
    bool deleteAfter = false;
    tPromiseResults<void> resolvedValue;
    QFuture<void> runFuture;

    tPromiseFunctions<void>::SuccessFunction fnAfterSuccess;
    tPromiseFunctions<void>::FailureFunction fnAfterFailure;

    void callSuccessFunction() {
        fnAfterSuccess();
    }

    void callFailureFunction() {
        fnAfterFailure(resolvedValue.error);
    }
};

template<typename T> class tPromise {
    public:
        explicit tPromise(typename tPromisePrivate<T>::RunFunction functionToRun);
        explicit tPromise(typename tPromisePrivate<T>::RunAsyncFunction functionToRun);
        ~tPromise();

        static tPromise<T>* runOnNewThread(typename tPromisePrivate<T>::RunAsyncFunction functionToRun);
        static tPromise<T>* runOnSameThread(typename tPromisePrivate<T>::RunAsyncFunction functionToRun);

        tPromise<T>* then(typename tPromiseFunctions<T>::SuccessFunction functionToRunAfterSuccess);
        tPromise<T>* error(typename tPromiseFunctions<T>::FailureFunction functionToRunOnFailure);
        tPromiseResults<T> await();

        bool isResolved() {
            return d->state == tPromisePrivate<T>::Resolved;
        }

        bool isErrored() {
            return d->state == tPromisePrivate<T>::Errored;
        }

        bool isPending() {
            return d->state == tPromisePrivate<T>::Pending;
        }

        void setDeleteAfter(bool deleteAfter) {
            d->deleteAfter = deleteAfter;
        }

        void deleteLater() {
            QTimer::singleShot(0, [ = ] {
                delete this;
            });
        }

    private:
        explicit tPromise();
        tPromisePrivate<T>* d;

        void callNextFunction();
        void watch();
};

template<typename T> void tPromise<T>::callNextFunction() {
    if (d->resolvedValue.error != "") {
        d->state = tPromisePrivate<T>::Errored;
        if (d->functionSetToRunAfterFailure) {
            d->fnAfterFailure(d->resolvedValue.error);
        }
    } else {
        d->state = tPromisePrivate<T>::Resolved;
        if (d->functionSetToRunAfterSuccess) {
            d->fnAfterSuccess(d->resolvedValue.result);
        }
    }

    if (d->deleteAfter) this->deleteLater();
}

template<> void tPromise<void>::callNextFunction();


template<typename T> inline void tPromise<T>::watch() {
    QFutureWatcher<void>* runFutureWatcher = new QFutureWatcher<void>();
    runFutureWatcher->setFuture(d->runFuture);
    QObject::connect(runFutureWatcher, &QFutureWatcher<T>::finished, [ = ] {
        runFutureWatcher->deleteLater();
        callNextFunction();
    });
}

template<typename T> inline tPromise<T>::tPromise(typename tPromisePrivate<T>::RunFunction functionToRun) {
    d = new tPromisePrivate<T>;
    d->runFuture = QtConcurrent::run([ = ]() -> void {
        QString err;
        d->resolvedValue.result = functionToRun(err);
        d->resolvedValue.error.swap(err);
    });

    watch();
}

template<> inline tPromise<void>::tPromise(typename tPromisePrivate<void>::RunFunction functionToRun) {
    d = new tPromisePrivate<void>;
    d->runFuture = QtConcurrent::run([ = ]() -> void {
        QString err;
        functionToRun(err);
        d->resolvedValue.error.swap(err);
    });

    watch();
}

template<typename T> inline tPromise<T>::tPromise(typename tPromisePrivate<T>::RunAsyncFunction functionToRun) {
    d = new tPromisePrivate<T>;
    d->runFuture = QtConcurrent::run([ = ]() -> void {
        QEventLoop* loop = new QEventLoop;

        typename tPromiseFunctions<T>::SuccessFunction successFunction = [ = ](T retVal) {
            d->resolvedValue.result = retVal;
            d->resolvedValue.error = "";
            QTimer::singleShot(0, loop, &QEventLoop::quit);
        };
        typename tPromiseFunctions<T>::FailureFunction failureFunction = [ = ](QString error) {
            d->resolvedValue.error.swap(error);
            QTimer::singleShot(0, loop, &QEventLoop::quit);
        };

        functionToRun(successFunction, failureFunction);
        loop->exec();

        loop->deleteLater();
    });

    watch();
}

template<> inline tPromise<void>::tPromise(typename tPromisePrivate<void>::RunAsyncFunction functionToRun) {
    d = new tPromisePrivate<void>;
    d->runFuture = QtConcurrent::run([ = ]() -> void {
        QEventLoop* loop = new QEventLoop;

        typename tPromiseFunctions<void>::SuccessFunction successFunction = [ = ]() {
            d->resolvedValue.error = "";
            QTimer::singleShot(0, loop, &QEventLoop::quit);
        };
        typename tPromiseFunctions<void>::FailureFunction failureFunction = [ = ](QString error) {
            d->resolvedValue.error.swap(error);
            QTimer::singleShot(0, loop, &QEventLoop::quit);
        };

        functionToRun(successFunction, failureFunction);
        loop->exec();

        loop->deleteLater();
    });

    watch();
}

template<typename T> inline tPromise<T>::~tPromise() {
    delete d;
}

template<typename T> tPromise<T>* tPromise<T>::runOnNewThread(typename tPromisePrivate<T>::RunAsyncFunction functionToRun) {
    return new tPromise<T>(functionToRun);
}

template<typename T> tPromise<T>* tPromise<T>::runOnSameThread(typename tPromisePrivate<T>::RunAsyncFunction functionToRun) {
    tPromise<T>* promise = new tPromise<T>;

    typename tPromiseFunctions<T>::SuccessFunction successFunction = [ = ](T retVal) {
        promise->d->resolvedValue.result = retVal;
        promise->d->resolvedValue.error = "";
        QTimer::singleShot(0, [ = ] {
            promise->callNextFunction();
        });
    };
    typename tPromiseFunctions<T>::FailureFunction failureFunction = [ = ](QString error) {
        promise->d->resolvedValue.error.swap(error);
        QTimer::singleShot(0, [ = ] {
            promise->callNextFunction();
        });

    };
    functionToRun(successFunction, failureFunction);

    //Don't watch because this runs synchronously

    return promise;
}

template<> tPromise<void>* tPromise<void>::runOnSameThread(typename tPromisePrivate<void>::RunAsyncFunction functionToRun);

template<typename T> inline tPromise<T>* tPromise<T>::then(typename tPromiseFunctions<T>::SuccessFunction functionToRunAfterSuccess) {
    Q_ASSERT(!d->functionSetToRunAfterSuccess);
    if (d->functionSetToRunAfterSuccess) return this;

    d->fnAfterSuccess = functionToRunAfterSuccess;
    d->functionSetToRunAfterSuccess = true;
    if (isResolved()) {
        d->callSuccessFunction();
    } else {
        d->deleteAfter = true;
    }
    return this;
}

template<typename T> inline tPromise<T>* tPromise<T>::error(typename tPromiseFunctions<T>::FailureFunction functionToRunOnFailure) {
    Q_ASSERT(!d->functionSetToRunAfterFailure);
    if (d->functionSetToRunAfterFailure) return this;

    d->fnAfterFailure = functionToRunOnFailure;
    d->functionSetToRunAfterFailure = true;
    if (isResolved()) {
        d->callFailureFunction();
    } else {
        d->deleteAfter = true;
    }
    return this;
}

template<typename T> inline tPromiseResults<T> tPromise<T>::await() {
    if (this->isPending()) {
        QEventLoop* loop = new QEventLoop();
        this->then(std::bind(&QEventLoop::quit, loop));
        this->error(std::bind(&QEventLoop::quit, loop));
        this->setDeleteAfter(false);
        loop->exec();
        loop->deleteLater();
    }

    tPromiseResults<T> retval = d->resolvedValue;
    this->deleteLater();
    return retval;
}

template<typename T> tPromise<T>::tPromise() {
    d = new tPromisePrivate<T>;
}

#endif // TPROMISE_H
