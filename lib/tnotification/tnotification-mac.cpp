#include "tnotification.h"

struct tNotificationPrivateByOS {

};

void tNotification::post(bool deleteWhenDone) {

}

void tNotification::dismiss() {

}

void tNotification::initialize() {
    dd = new tNotificationPrivateByOS();
}

void tNotification::destroy() {
    delete dd;
}
