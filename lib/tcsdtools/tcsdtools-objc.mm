#include "../tcsdtools.h"

#include <QWidget>
#import <AppKit/AppKit.h>

void tCsdTools::macHandleDrag(QPoint screenPos, QWidget* dragWindow) {
    CGEventRef cgEvent = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDown, screenPos.toCGPoint(), kCGMouseButtonLeft);
    NSEvent* nsEvent = [NSEvent eventWithCGEvent:cgEvent];

    NSView* view = reinterpret_cast<NSView*>(dragWindow->winId());
    [view.window performWindowDragWithEvent:nsEvent];
    CFRelease(cgEvent);
}

void tCsdTools::macInstallResizeAction(QWidget* widget) {
    NSView* view = reinterpret_cast<NSView*>(widget->winId());

    NSWindowStyleMask styleMask = NSWindowStyleMaskFullSizeContentView | NSWindowStyleMaskUnifiedTitleAndToolbar | NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    [view.window setStyleMask:styleMask];
}

void tCsdTools::macRemoveResizeAction(QWidget* widget) {
    NSView* view = reinterpret_cast<NSView*>(widget->winId());

    NSWindowStyleMask styleMask = NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
    [view.window setStyleMask:styleMask];
}
