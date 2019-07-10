#include "tnotification.h"

#include <QDebug>
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

@interface AppNotificationDelegate : NSResponder<NSUserNotificationCenterDelegate>

@end

@implementation AppNotificationDelegate
- (BOOL)userNotificationCenter:(NSUserNotificationCenter*)center shouldPresentNotification:(NSUserNotification*)notification {
    return YES;
}

- setDelegateForUserNotificationCenter:(NSUserNotificationCenter*) center {
    center.delegate = self;
}
@end

struct tNotificationPrivateByOS {
    static bool registeredAppDelegate;
    static void registerDelegate();
};
bool tNotificationPrivateByOS::registeredAppDelegate = false;

void tNotificationPrivateByOS::registerDelegate() {
    AppNotificationDelegate* appDelegate = [[AppNotificationDelegate alloc] init];

    [appDelegate setDelegateForUserNotificationCenter:[NSUserNotificationCenter defaultUserNotificationCenter]];
    tNotificationPrivateByOS::registeredAppDelegate = true;
}

void tNotification::post(bool deleteWhenDone) {
    NSUserNotification* notification = [NSUserNotification new];
    notification.title = sum.toNSString();
    notification.informativeText = txt.toNSString();

    if (!this->noSound) {
        notification.soundName = NSUserNotificationDefaultSoundName;
    }

    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
    [notification autorelease];

    if (deleteWhenDone) {
        this->deleteLater();
    }
}

void tNotification::initialize() {
    dd = new tNotificationPrivateByOS();
    if (!dd->registeredAppDelegate) dd->registerDelegate();
}

void tNotification::destroy() {
    delete dd;
}
