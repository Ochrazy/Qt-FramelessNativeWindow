#ifdef __APPLE__

#include "FramelessWindowConverterMacos.h"
#include "FramelessWindowConverter.h"

#import <AppKit/AppKit.h>
#import <objc/runtime.h>

#include <QDebug>

using namespace FWC;

// TrafficLightsHelper objective-c class
@interface TrafficLightsHelper : NSObject
                                 @property (assign, nonatomic) NSWindow* window;
@property (assign, nonatomic) FramelessWindowConverterMacos* fwcM;
- (id) initWithFWCMAndWindow:(FramelessWindowConverterMacos*)fwcM :(NSWindow*)inWin;
- (BOOL)_mouseInGroup:(NSButton *)button;
- (void) minimizeButtonAction:(id)sender;
- (void) closeButtonAction:(id)sender;
- (void) zoomButtonAction:(id)sender;
@end

@implementation TrafficLightsHelper
@synthesize fwcM;
@synthesize window;
- (id) initWithFWCMAndWindow:(FramelessWindowConverterMacos*)inFWCM :(NSWindow*)inWin
{
    if((self = [super init]) != nil)
    {
        fwcM = inFWCM;
        window = inWin;
        return self;
    }
    else return nil;
}

// Hack to get proper traffig light highlighting
static bool isMouseInGroup = false;
- (BOOL)_mouseInGroup:(NSButton *)button
{
    (void)button;
    return isMouseInGroup;
}

- (void) minimizeButtonAction:(id)sender
{
    (void)sender;
    self.fwcM->minimizeWindow();
}

- (void) closeButtonAction:(id)sender
{
    (void)sender;
    self.fwcM->closeWindow();
}

- (void) zoomButtonAction:(id)sender
{
    (void)sender;
    NSEventModifierFlags currentFlags = [NSEvent modifierFlags];
    if(currentFlags & NSEventModifierFlagOption)
    {
        self.fwcM->maximizeWindow(); // also minimizes
    }
    else
    {
        self.fwcM->toggleFullscreen();
    }
}

@end
// END of TrafficLightsHelper objective-c class


FramelessWindowConverterMacos::FramelessWindowConverterMacos(FramelessWindowConverter* q) : FramelessWindowConverterPrivate(q)
{

}

void FramelessWindowConverterMacos::minimizeWindow()
{
    [window miniaturize:window];
}

void FramelessWindowConverterMacos::maximizeWindow()
{  
    hideForTranslucency();
    isResizing = true;

    // The NSWindow needs to be resizable, otherwise the window will
    // not be possible to zoom back to non-zoomed state.
    const bool wasResizable = window.styleMask & NSWindowStyleMaskResizable;
    window.styleMask |= NSWindowStyleMaskResizable;

    [window zoom:window];

    if (!wasResizable)
        window.styleMask &= ~NSWindowStyleMaskResizable;

    if(q_ptr->isUsingTrafficLightsOnMacOS())
    {
        [fullScreenButton removeFromSuperview];
        [window.contentView addSubview:fullScreenButton];
        [closeButton removeFromSuperview];
        [window.contentView addSubview:closeButton];
        [minimizeButton removeFromSuperview];
        [window.contentView addSubview:minimizeButton];

        [fullScreenButton setFrameOrigin:NSMakePoint(xPos, yPos+40)];
        [closeButton setFrameOrigin:NSMakePoint(xPos, yPos+0)];
        [minimizeButton setFrameOrigin:NSMakePoint(xPos, yPos+20)];
    }

    isResizing = false;
    showForTranslucency();
}

void FramelessWindowConverterMacos::restoreWindow()
{
    // Zoom will restore window
    maximizeWindow();
}

void FramelessWindowConverterMacos::closeWindow()
{
    if([[window delegate] respondsToSelector:@selector(windowShouldClose:)])
    {
        if(![[window delegate] windowShouldClose:window]) return;
    }

    [window close];
}

void FramelessWindowConverterMacos::toggleFullscreen()
{
    isResizing = true;
    [window toggleFullScreen:window];
    isResizing = false;
}

void FramelessWindowConverterMacos::hideForTranslucency()
{
    if(q_ptr->isUsingTrafficLightsOnMacOS())
    {
        [fullScreenButton setHidden:YES];
        [closeButton setHidden:YES];
        [minimizeButton setHidden:YES];
    }
}

void FramelessWindowConverterMacos::showForTranslucency()
{
    if(q_ptr->isUsingTrafficLightsOnMacOS())
    {
        [fullScreenButton setHidden:NO];
        [closeButton setHidden:NO];
        [minimizeButton setHidden:NO];
    }
}

void FramelessWindowConverterMacos::convertToFrameless()
{
    nativeWidgetView = reinterpret_cast<NSView *>(q_ptr->getWindowHandle());
    window = [nativeWidgetView window];

    // Title Bar invisible
    window.titleVisibility = NSWindowTitleHidden;
    window.titlebarAppearsTransparent = YES;
    window.movable = NO; // Custom Move and Resize
    //[wind setMovableByWindowBackground:YES];

    // Reset Style Mask
    window.styleMask &= ~NSWindowStyleMaskBorderless;
    window.styleMask |= NSWindowStyleMaskFullSizeContentView;
    window.styleMask |= NSWindowStyleMaskTitled;
    window.styleMask |= NSWindowStyleMaskClosable;
    window.styleMask |= NSWindowStyleMaskMiniaturizable;
    window.styleMask &= ~NSWindowStyleMaskResizable; // Custom Resize

    [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    // Enable Layer backing
    // Fixes issues with WA_TranslucentBackground and paintEvent.
    // Repainting the window would not update the translucent pixels correctly.
    // The Part of the window where the system buttons would be located are updating correctly for some reason
    // The rest of the window would be more opaque with every repaint.
    nativeWidgetView.wantsLayer = YES;

    if(q_ptr->isUsingTrafficLightsOnMacOS())
    {
        xPos = 10;
        yPos = 25;

        // Traffic lights
        [[window standardWindowButton:NSWindowCloseButton] setHidden:NO];
        [[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:NO];
        [[window standardWindowButton:NSWindowZoomButton] setHidden:NO];

        TrafficLightsHelper* tlHelper = [[TrafficLightsHelper alloc] initWithFWCMAndWindow:this :window];

        fullScreenButton = [window standardWindowButton:NSWindowZoomButton];
        [fullScreenButton.superview willRemoveSubview:fullScreenButton];
        [fullScreenButton removeFromSuperview];
        [fullScreenButton viewWillMoveToSuperview:[window contentView]];
        [window.contentView addSubview:fullScreenButton];
        [fullScreenButton viewDidMoveToSuperview];
        [fullScreenButton setTarget:tlHelper];
        [fullScreenButton setAction:@selector(zoomButtonAction:)];

        closeButton = [window standardWindowButton:NSWindowCloseButton];
        [closeButton.superview willRemoveSubview:closeButton];
        [closeButton removeFromSuperview];
        [closeButton viewWillMoveToSuperview:[window contentView]];
        [window.contentView addSubview:closeButton];
        [closeButton viewDidMoveToSuperview];
        [closeButton setTarget:tlHelper];
        [closeButton setAction:@selector(closeButtonAction:)];

        minimizeButton = [window standardWindowButton:NSWindowMiniaturizeButton];
        [minimizeButton.superview willRemoveSubview:minimizeButton];
        [minimizeButton removeFromSuperview];
        [minimizeButton viewWillMoveToSuperview:[window contentView]];
        [window.contentView addSubview:minimizeButton];
        [minimizeButton viewDidMoveToSuperview];
        [minimizeButton setTarget:tlHelper];
        [minimizeButton setAction:@selector(minimizeButtonAction:)];

        [fullScreenButton setFrameOrigin:NSMakePoint(xPos, yPos+40)];
        [closeButton setFrameOrigin:NSMakePoint(xPos, yPos+0)];
        [minimizeButton setFrameOrigin:NSMakePoint(xPos, yPos+20)];

        // Replace _mouseInGroup method in contentView to enable proper highlighting for traffic lights
        SEL swizzledSelector = @selector(_mouseInGroup:);
        Method swizzledMethod = class_getInstanceMethod([tlHelper class], swizzledSelector);
        class_replaceMethod([nativeWidgetView class],
                swizzledSelector,
                method_getImplementation(swizzledMethod),
                method_getTypeEncoding(swizzledMethod));
    }
    else
    {
        fullScreenButton = [window standardWindowButton:NSWindowZoomButton];
        closeButton = [window standardWindowButton:NSWindowCloseButton];
        minimizeButton = [window standardWindowButton:NSWindowMiniaturizeButton];

        [closeButton setHidden:YES];
        [minimizeButton setHidden:YES];
        [fullScreenButton setHidden:YES];
    }

    // Exiting fullscreen mode messes up everything, so fix it here
    [[NSNotificationCenter defaultCenter]
            addObserverForName:NSWindowDidExitFullScreenNotification object:window queue:nil usingBlock:^(NSNotification *){
        convertToFrameless();
    }];

    // Exiting fullscreen mode messes up everything, so fix it here
    [[NSNotificationCenter defaultCenter]
            addObserverForName:NSWindowDidResizeNotification object:window queue:nil usingBlock:^(NSNotification *){
        // Catch resizing events not caught by this class directly
        if(!isResizing)
        {
            [fullScreenButton setFrameOrigin:NSMakePoint(xPos, yPos+40)];
            [closeButton setFrameOrigin:NSMakePoint(xPos, yPos+0)];
            [minimizeButton setFrameOrigin:NSMakePoint(xPos, yPos+20)];
        }
    }];

    // Control Cursor shape ourselves
    [window disableCursorRects];
    [window setHasShadow:NO];
    [window setOpaque:NO];
    [window setBackgroundColor:[NSColor clearColor]];
}

void FramelessWindowConverterMacos::showCursorByHitResult(FWCBorderHitTestResult inBorderHitResult)
{
    switch(inBorderHitResult)
    {
    case FWCBorderHitTestResult::LEFT:
        // Fallthrough
    case FWCBorderHitTestResult::RIGHT:
    {
        SEL selector = NSSelectorFromString(@"_windowResizeEastWestCursor");
        NSCursor* newCursor = [NSCursor performSelector:selector];
        [newCursor set];
        break;
    }
    case FWCBorderHitTestResult::TOP:
        // Fallthrough
    case FWCBorderHitTestResult::BOTTOM:
    {
        SEL selector = NSSelectorFromString(@"_windowResizeNorthSouthCursor");
        NSCursor* newCursor = [NSCursor performSelector:selector];
        [newCursor set];
        break;
    }
    case FWCBorderHitTestResult::TOP_RIGHT:
        // Fallthrough
    case FWCBorderHitTestResult::BOTTOM_LEFT:
    {
        SEL selector = NSSelectorFromString(@"_windowResizeNorthEastSouthWestCursor");
        NSCursor* newCursor = [NSCursor performSelector:selector];
        [newCursor set];
        break;
    }
    case FWCBorderHitTestResult::BOTTOM_RIGHT:
        // Fallthrough
    case FWCBorderHitTestResult::TOP_LEFT:
    {
        SEL selector = NSSelectorFromString(@"_windowResizeNorthWestSouthEastCursor");
        NSCursor* newCursor = [NSCursor performSelector:selector];
        [newCursor set];
        break;
    }
    case FWCBorderHitTestResult::CLIENT:
        if(isResizing == false)
        {
            [[NSCursor arrowCursor] set];
        }
        break;
    default:
        // do nothing
        break;
    }
}

void FramelessWindowConverterMacos::showAppropriateCursor()
{
    // Convert NSPoint to QPoint (different origin of window)
    NSPoint localPos = [[window contentView] convertPoint: [window mouseLocationOutsideOfEventStream] fromView: nil];
    FWCPoint mousePos(static_cast<int>(localPos.x), static_cast<int>(localPos.y));
    FWCRect rect(0, 0, static_cast<int>(window.frame.size.width), static_cast<int>(window.frame.size.height));

    showCursorByHitResult(doBorderHitTest(rect, mousePos, q_ptr->getBorderWidth()));
}

void FramelessWindowConverterMacos::resizeWindow(FWCFloatingPoint mouseLocationInWindow)
{
    NSRect oldFrame = [window frame];
    NSRect newFrame = oldFrame;

    // New Structure for the Hit Result
    bool bHitTop = false;
    bool bHitBottom = false;
    bool bHitRight = false;
    bool bHitLeft = false;

    switch(currentBHTR)
    {
    case FWCBorderHitTestResult::RIGHT:
        bHitRight = true;
        break;
    case FWCBorderHitTestResult::LEFT:
        bHitLeft = true;
        break;
    case FWCBorderHitTestResult::TOP:
        bHitTop = true;
        break;
    case FWCBorderHitTestResult::BOTTOM:
        bHitBottom = true;
        break;
    case FWCBorderHitTestResult::TOP_LEFT:
        bHitTop = true;
        bHitLeft = true;
        break;
    case FWCBorderHitTestResult::TOP_RIGHT:
        bHitTop = true;
        bHitRight = true;
        break;
    case FWCBorderHitTestResult::BOTTOM_LEFT:
        bHitBottom = true;
        bHitLeft = true;
        break;
    case FWCBorderHitTestResult::BOTTOM_RIGHT:
        bHitBottom = true;
        bHitRight = true;
        break;
    default:
        break;
    }

    if(bHitTop)
    {
        newFrame.size.height = mouseLocationInWindow.y + startDiffCursorFrameLocs.y;
    }
    else if(bHitBottom)
    {
        newFrame.origin.y += mouseLocationInWindow.y - startDiffCursorFrameLocs.y;
        newFrame.size.height = newFrame.size.height - mouseLocationInWindow.y + startDiffCursorFrameLocs.y;
    }

    if(bHitRight)
    {
        newFrame.size.width = mouseLocationInWindow.x + startDiffCursorFrameLocs.x;
    }
    else if(bHitLeft)
    {
        newFrame.origin.x += mouseLocationInWindow.x - startDiffCursorFrameLocs.x;
        newFrame.size.width = newFrame.size.width - mouseLocationInWindow.x + startDiffCursorFrameLocs.x;
    }

    // Limit the resizing of the window
    int limit = 0;
    if(((limit = q_ptr->getMinimumWindowWidth()) > newFrame.size.width) ||
            (limit = q_ptr->getMaximumWindowWidth()) < newFrame.size.width)
    {
        if(bHitLeft)
            newFrame.origin.x += (newFrame.size.width - limit);
        newFrame.size.width = limit;
    }
    if(((limit = q_ptr->getMinimumWindowHeight()) > newFrame.size.height) ||
            (limit = q_ptr->getMaximumWindowHeight()) < newFrame.size.height)
    {
        if(bHitBottom)
            newFrame.origin.y += (newFrame.size.height - limit);
        newFrame.size.height = limit;
    }

    [window setFrame:newFrame display:YES];
}

bool FramelessWindowConverterMacos::filterNativeEvent(void *message, long *result)
{
    result = nullptr; // not used on mac

    NSEvent *event = static_cast<NSEvent *>(message);

    if ([event type] == NSEventTypeLeftMouseDown)
    {
        // Convert NSPoint to QPoint (different origin of window)
        NSPoint localPos = [[window contentView] convertPoint: [event locationInWindow] fromView: nil];
        FWCPoint mousePos(static_cast<int>(localPos.x), static_cast<int>(localPos.y));

        // Only this widget is used for dragging.
        bool shouldDrag = q_ptr->getShouldPerformWindowDrag()(mousePos.x, mousePos.y);

        // Double Click
        if([event clickCount] == 2)
        {
            if(shouldDrag)
                maximizeWindow();
            return false;
        }

        NSPoint globalMousePos = [NSEvent mouseLocation];
        NSRect currentFrame = [window frame];
        FWCRect rect(0, 0, static_cast<int>(window.frame.size.width), static_cast<int>(window.frame.size.height));

        if((currentBHTR = doBorderHitTest(rect, mousePos, q_ptr->getBorderWidth())) != FWCBorderHitTestResult::CLIENT) // Resize
        {
            // Save the difference between the frame border and the current mouse location
            if(currentBHTR == FWCBorderHitTestResult::LEFT || currentBHTR == FWCBorderHitTestResult::BOTTOM_LEFT ||
                    currentBHTR == FWCBorderHitTestResult::BOTTOM)
            {
                startDiffCursorFrameLocs.x = globalMousePos.x - currentFrame.origin.x;
                startDiffCursorFrameLocs.y = globalMousePos.y - currentFrame.origin.y;
            }
            else if(currentBHTR == FWCBorderHitTestResult::TOP_LEFT)
            {
                startDiffCursorFrameLocs.x = globalMousePos.x - currentFrame.origin.x;
                startDiffCursorFrameLocs.y = currentFrame.size.height - (globalMousePos.y - currentFrame.origin.y);
            }
            else if(currentBHTR == FWCBorderHitTestResult::RIGHT || currentBHTR == FWCBorderHitTestResult::BOTTOM_RIGHT)
            {
                startDiffCursorFrameLocs.x = currentFrame.size.width - (globalMousePos.x - currentFrame.origin.x);
                startDiffCursorFrameLocs.y = globalMousePos.y - currentFrame.origin.y;
            }
            else //if(currentBHTR == FWCBorderHitTestResult::TOP || currentBHTR == FWCBorderHitTestResult::TOP_RIGHT)
            {
                startDiffCursorFrameLocs.x = currentFrame.size.width - (globalMousePos.x - currentFrame.origin.x);
                startDiffCursorFrameLocs.y = currentFrame.size.height - (globalMousePos.y - currentFrame.origin.y);
            }

            isResizing = true;
            // Every resize traffic lights get reset to "preferred" position
            // just hide them while resizing
            [closeButton setHidden:YES];
            [minimizeButton setHidden:YES];
            [fullScreenButton setHidden:YES];
            return false;
        }
        else if(shouldDrag) // Move
        {
            startDiffCursorFrameLocs.x = globalMousePos.x - currentFrame.origin.x;
            startDiffCursorFrameLocs.y = globalMousePos.y - currentFrame.origin.y;
            isMoving = true;
            return false;
        }
    }

    if([event type] == NSEventTypeLeftMouseUp)
    {
        if(isResizing)
        {
            isResizing = !isResizing;
            showCursorByHitResult(FWCBorderHitTestResult::CLIENT);

            // Every resize macos sets the position of the original buttons to the "preferred" state,
            // so set the position manually here
            [fullScreenButton setFrameOrigin:NSMakePoint(xPos, yPos+40)];
            [closeButton setFrameOrigin:NSMakePoint(xPos, yPos+0)];
            [minimizeButton setFrameOrigin:NSMakePoint(xPos, yPos+20)];

            [closeButton setHidden:NO];
            [minimizeButton setHidden:NO];
            [fullScreenButton setHidden:NO];
            return false;
        }
        else if(isMoving)
        {
            isMoving = !isMoving;
        }
    }
    if ([event type] == NSEventTypeLeftMouseDragged)
    {
        if(isResizing == true)
        {
            // The cursor can go outside of the application and thus changes shape.
            // As soon as the cursor is inside of the application again, the appropriate cursor is set.
            //showAppropriateCursor();
            showCursorByHitResult(currentBHTR);

            NSPoint mouseLocInWindow = [event locationInWindow];
            resizeWindow(FWCFloatingPoint(mouseLocInWindow.x, mouseLocInWindow.y));
        }
        else if(isMoving)
        {
            NSRect newFrame = [window frame];
            newFrame.origin.x = [NSEvent mouseLocation].x - startDiffCursorFrameLocs.x;
            newFrame.origin.y = [NSEvent mouseLocation].y - startDiffCursorFrameLocs.y;

            [window setFrame:newFrame display:YES];
        }
        return false;
    }
    if([event type] == NSEventTypeFlagsChanged)
    {
        if(q_ptr->isUsingTrafficLightsOnMacOS())
        {
            if(event.keyCode == 58) // option key pressed or released
                [fullScreenButton setNeedsDisplay:YES]; // Properly show fullscreen or zoom button
        }
    }
    if([event type] == NSEventTypeMouseMoved)
    {
        showAppropriateCursor();

        if(q_ptr->isUsingTrafficLightsOnMacOS())
        {
            // Determine if Mouse Cursor is inside of traffic light area
            NSPoint localPos = [[window contentView] convertPoint: [window mouseLocationOutsideOfEventStream] fromView: nil];
            bool isInside = NSPointInRect(localPos, NSMakeRect(closeButton.frame.origin.x, closeButton.frame.origin.y,
                                                               (fullScreenButton.frame.origin.x + fullScreenButton.frame.size.width) - closeButton.frame.origin.x,
                                                               (fullScreenButton.frame.origin.y + fullScreenButton.frame.size.height) - closeButton.frame.origin.y));
            if(isInside != isMouseInGroup)
            {
                isMouseInGroup = isInside;
                [fullScreenButton setNeedsDisplay:YES];
                [closeButton setNeedsDisplay:YES];
                [minimizeButton setNeedsDisplay:YES];
            }
        }

        return false;
    }

    return false;
}

#endif

