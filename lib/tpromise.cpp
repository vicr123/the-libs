#include "tpromise.h"

template <typename T> tPromise<T>::tPromise(std::function<T (QString&)> functionToRun) {
    runFuture = QtConcurrent::run([=]() -> T {
        return functionToRun(errorString);
    });

    runFutureWatcher = new QFutureWatcher<T>();
    runFutureWatcher->setFuture(runFuture);
    connect(runFutureWatcher, &QFutureWatcher<T>::finished, [=] {
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

template <typename T> tPromise<T>* tPromise<T>::then(std::function<void (T)> functionToRunAfterSuccess) {
    if (!resolved && !functionSetToRunAfterSuccess) {
        this->fnAfterSuccess = functionToRunAfterSuccess;
        functionSetToRunAfterSuccess = true;
    }
    return this;
}

template <typename T> tPromise<T>* tPromise<T>::error(std::function<void (QString)> functionToRunAfterFailure) {
    if (!resolved && !functionSetToRunAfterFailure) {
        this->fnAfterFailure = functionToRunAfterFailure;
        functionSetToRunAfterFailure = true;
    }
    return this;
}

template <typename T> bool tPromise<T>::isPending() {
    return !resolved && !errored;
}

template <typename T> bool tPromise<T>::isResolved() {
    return resolved;
}

template <typename T> bool tPromise<T>::isErrored() {
    return errored;
}
