#include "../tcsdtools.h"

#include <QWidget>
#import <AppKit/AppKit.h>

void tCsdTools::macHandleDrag(QPoint screenPos, QWidget* dragWindow) {
    CGEventRef cgEvent = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDown, screenPos.toCGPoint(), kCGMouseButtonLeft);
    NSEvent* nsEvent = [NSEvent eventWithCGEvent:cgEvent];

    NSView *view = reinterpret_cast<NSView *>(dragWindow->winId());
    [view.window performWindowDragWithEvent:nsEvent];
    CFRelease(cgEvent);
}
