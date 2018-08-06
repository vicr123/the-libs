#ifndef TPROMISE_H
#define TPROMISE_H

#include <QObject>
#include <functional>
#include <QtConcurrent/QtConcurrent>
#include <the-libs_global.h>

#define TPROMISE_CALL_NO_ARGS(fn) QMetaObject::invokeMethod(this, fn, Qt::QueuedConnection)
#define TPROMISE_CALL(fn, ...) QMetaObject::invokeMethod(this, fn, Qt::QueuedConnection, __VA_ARGS__)

template <typename T> class tPromise
{
    public:
        explicit tPromise(std::function<T(QString&)> functionToRun) {
            runFuture = QtConcurrent::run([=]() -> T {
                return functionToRun(errorString);
            });

            runFutureWatcher = new QFutureWatcher<T>();
            runFutureWatcher->setFuture(runFuture);
            QObject::connect(runFutureWatcher, &QFutureWatcher<T>::finished, [=] {
                if (errorString != "") {
                    errored = true;
                    if (functionSetToRunAfterFailure) {
                        fnAfterFailure(errorString);
                        delete this;
                    }
                } else {
                    resolved = true;
                    if (functionSetToRunAfterSuccess) {
                        fnAfterSuccess(runFuture.result());
                        delete this;
                    }
                }
            });
        }

        tPromise<T>* then(std::function<void(T)> functionToRunAfterSuccess) {
            if (!resolved && !functionSetToRunAfterSuccess) {
                this->fnAfterSuccess = functionToRunAfterSuccess;
                functionSetToRunAfterSuccess = true;
            }
            return this;
        }
        tPromise<T>* error(std::function<void(QString)> functionToRunOnFailure) {
            if (!resolved && !functionSetToRunAfterFailure) {
                this->fnAfterFailure = functionToRunOnFailure;
                functionSetToRunAfterFailure = true;
            }
            return this;
        }

        bool isResolved() {
            return resolved;
        }

        bool isErrored() {
            return errored;
        }

        bool isPending() {
            return !resolved && !errored;
        }

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
