/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/

#include "tpromise.h"

template<> void tPromise<void>::callNextFunction()
{
    if (d->resolvedValue.error != "") {
        d->state = tPromisePrivate<void>::Errored;
        if (d->functionSetToRunAfterFailure) {
            d->fnAfterFailure(d->resolvedValue.error);
        }
    } else {
        d->state = tPromisePrivate<void>::Resolved;
        if (d->functionSetToRunAfterSuccess) {
            d->fnAfterSuccess();
        }
    }

    if (d->deleteAfter) this->deleteLater();
}

template<> tPromise<void>* tPromise<void>::runOnSameThread(typename tPromisePrivate<void>::RunAsyncFunction functionToRun)
{
    tPromise<void>* promise = new tPromise<void>;

    typename tPromiseFunctions<void>::SuccessFunction successFunction = [=]() {
        QTimer::singleShot(0, [=] {
            promise->callNextFunction();
        });
    };
    typename tPromiseFunctions<void>::FailureFunction failureFunction = [=](QString error) {
        promise->d->resolvedValue.error.swap(error);
        QTimer::singleShot(0, [=] {
            promise->callNextFunction();
        });

    };
    functionToRun(successFunction, failureFunction);

    //Don't watch because this runs synchronously

    return promise;
}
