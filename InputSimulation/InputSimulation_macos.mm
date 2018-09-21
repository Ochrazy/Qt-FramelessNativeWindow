#ifdef __APPLE__

#include "InputSimulation.h"

#import <AppKit/AppKit.h>

void InputSimulation::SimulateLeftClick()
{
    // Get the mouse position
    CGPoint point = [NSEvent mouseLocation];
    point.y = [[NSScreen mainScreen] frame].size.height - point.y;

    CGEventRef leftClick = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDown, point, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, leftClick);
    CGEventSetType(leftClick, kCGEventLeftMouseUp);
    CGEventPost(kCGHIDEventTap, leftClick);
    CFRelease(leftClick);
}

#endif
