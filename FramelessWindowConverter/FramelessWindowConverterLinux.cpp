#ifdef __linux__

#include "FramelessWindowConverterLinux.h"
#include "FramelessWindowConverter.h"

#include <QtX11Extras/qx11info_x11.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include <X11/extensions/XI2proto.h>
#include <string.h>// memset

using namespace FWC;

FramelessWindowConverterLinux::FramelessWindowConverterLinux(FramelessWindowConverter* q) : FramelessWindowConverterPrivate(q), borderWidth(8), lastButtonPressTime(0)
{

}

void FramelessWindowConverterLinux::convertToFrameless()
{

}

void FramelessWindowConverterLinux::changeCursorShape(unsigned int shape)
{
    Cursor cursor = XCreateFontCursor(QX11Info::display(), shape);
    XDefineCursor(QX11Info::display(), q_ptr->getWindowHandle(), cursor);
    XFlush(QX11Info::display());
}

void FramelessWindowConverterLinux::minimizeWindow()
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = q_ptr->getWindowHandle();
    event.type = XInternAtom(QX11Info::display(), "WM_CHANGE_STATE", False);
    event.data.data32[0] = 3; // IconicState
    event.data.data32[1] = 0;
    event.data.data32[2] = 0;
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(QX11Info::connection(), 0, QX11Info::appRootWindow(QX11Info::appScreen()),
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   (const char *)&event);
}

void FramelessWindowConverterLinux::maximizeWindow()
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = q_ptr->getWindowHandle();
    event.type = XInternAtom(QX11Info::display(), "_NET_WM_STATE", False);
    event.data.data32[0] = 1; //_NET_WM_STATE_ADD
    event.data.data32[1] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    event.data.data32[2] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_VERT", False);
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(QX11Info::connection(), 0, QX11Info::appRootWindow(QX11Info::appScreen()),
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   (const char *)&event);
}

void FramelessWindowConverterLinux::restoreWindow()
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = q_ptr->getWindowHandle();
    event.type = XInternAtom(QX11Info::display(), "_NET_WM_STATE", False);
    event.data.data32[0] = 0; //_NET_WM_STATE_REMOVE
    event.data.data32[1] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_HORZ", False);
    event.data.data32[2] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_VERT", False);
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(QX11Info::connection(), 0, QX11Info::appRootWindow(QX11Info::appScreen()),
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   (const char *)&event);
}

void FramelessWindowConverterLinux::closeWindow()
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = q_ptr->getWindowHandle();
    event.type = XInternAtom(QX11Info::display(), "WM_PROTOCOLS", true);
    event.data.data32[0] = XInternAtom(QX11Info::display(), "WM_DELETE_WINDOW", false);
    event.data.data32[1] = CurrentTime;
    event.data.data32[2] = 0;
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(QX11Info::connection(), 0, QX11Info::appRootWindow(QX11Info::appScreen()),
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   (const char *)&event);
}

bool isXIEvent(xcb_generic_event_t *event, int opCode)
{
    xcb_ge_event_t *e = reinterpret_cast<xcb_ge_event_t*>(event);
    return e->pad0 == opCode;
}

bool isXIType(xcb_generic_event_t *event, int opCode, uint16_t type)
{
    if (!isXIEvent(event, opCode))
        return false;

    xXIGenericDeviceEvent *xiEvent = reinterpret_cast<xXIGenericDeviceEvent *>(event);
    return xiEvent->evtype == type;

}

qreal fixed1616ToInt(FP1616 val)
{
    return qreal(val) / 0x10000;
}

FWCRect FramelessWindowConverterLinux::getCurrentWindowFrame()
{
    xcb_get_geometry_cookie_t geom_cookie = xcb_get_geometry (QX11Info::connection(), q_ptr->getWindowHandle());
    xcb_get_geometry_reply_t* frame_geometry = xcb_get_geometry_reply(QX11Info::connection(), geom_cookie, nullptr);
    return FWCRect(0, 0,
                   frame_geometry->width, frame_geometry->height);
}

bool FramelessWindowConverterLinux::filterNativeEvent(void *message, long *result)
{
    Q_UNUSED(result)

    xcb_generic_event_t* ev = static_cast<xcb_generic_event_t *>(message);

    if ((ev->response_type & ~0x80) == XCB_GE_GENERIC)
    {
        int xiEventBase, xiErrorBase;
        XQueryExtension(QX11Info::display(), "XInputExtension", &xiOpCode, &xiEventBase, &xiErrorBase);

        if (isXIType(ev, xiOpCode, XI_Motion))
        {
            if(isSystemOpRunning)
            {
                // Send button release event not sent after _NET_WM_MOVERESIZE, fixes some bugs with Qt
                xXIGenericDeviceEvent *xiEvent = reinterpret_cast<xXIGenericDeviceEvent *>(ev);
                xiEvent->evtype = XI_ButtonRelease;
                XSendEvent(QX11Info::display(), q_ptr->getWindowHandle(), False, 0xfff, (XEvent*)xiEvent);

                isSystemOpRunning = false;
            }

            xXIDeviceEvent* xev = reinterpret_cast<xXIDeviceEvent*>(ev);
            const int x = fixed1616ToInt(xev->event_x);
            const int y = fixed1616ToInt(xev->event_y);
            FWCPoint mousePos(x, y);

            switch (doBorderHitTest(getCurrentWindowFrame(), mousePos, borderWidth))
            {
            case FWCBorderHitTestResult::LEFT:
                changeCursorShape( XC_left_side);
                break;
            case FWCBorderHitTestResult::RIGHT:
                changeCursorShape( XC_right_side);
                break;
            case FWCBorderHitTestResult::TOP:
                changeCursorShape( XC_top_side);
                break;
            case FWCBorderHitTestResult::BOTTOM:
                changeCursorShape( XC_bottom_side);
                break;
            case FWCBorderHitTestResult::BOTTOM_LEFT:
                changeCursorShape( XC_bottom_left_corner);
                break;
            case FWCBorderHitTestResult::BOTTOM_RIGHT:
                changeCursorShape( XC_bottom_right_corner);
                break;
            case FWCBorderHitTestResult::TOP_LEFT:
                changeCursorShape( XC_top_left_corner);
                break;
            case FWCBorderHitTestResult::TOP_RIGHT:
                changeCursorShape( XC_top_right_corner);
                break;
            default:
                changeCursorShape( XC_arrow);
                break;
            }

            return false;
        }
        if (isXIType(ev, xiOpCode, XI_ButtonPress))
        {
            xXIDeviceEvent* deviceEvent = reinterpret_cast<xXIDeviceEvent*>(ev);

            if(deviceEvent->detail != 1) return false;

            const int x = fixed1616ToInt(deviceEvent->event_x);
            const int y = fixed1616ToInt(deviceEvent->event_y);
            FWCPoint mousePos(x, y);
            FWCRect windowFrame(getCurrentWindowFrame());

            // Only this widget is used for dragging.
            if (!q_ptr->getShouldPerformWindowDrag()(mousePos.x, mousePos.y))
            {
                return false;
            }

            // Double Click
            if(lastButtonPressTime != 0 && (deviceEvent->time - lastButtonPressTime) < 400)
            {
                xcb_get_property_cookie_t cookie = xcb_get_property(QX11Info::connection(), false, q_ptr->getWindowHandle(), XInternAtom(QX11Info::display(), "_NET_WM_STATE", true), XCB_ATOM_ATOM, 0, 32);
                xcb_get_property_reply_t* reply = xcb_get_property_reply(QX11Info::connection(), cookie, nullptr);
                xcb_atom_t* isMaximized = reinterpret_cast<xcb_atom_t*>(xcb_get_property_value(reply));

                xcb_client_message_event_t event;
                event.response_type = XCB_CLIENT_MESSAGE;
                event.format = 32;
                event.sequence = 0;
                event.window = q_ptr->getWindowHandle();
                event.type = XInternAtom(QX11Info::display(), "_NET_WM_STATE", true);
                event.data.data32[1] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_HORZ", False);
                event.data.data32[2] = XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_VERT", False);
                event.data.data32[3] = 0;
                event.data.data32[4] = 0;

                if((*isMaximized == XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_VERT", False)) ||
                        (*isMaximized == XInternAtom(QX11Info::display(), "_NET_WM_STATE_MAXIMIZED_HORZ", False)))
                {
                    event.data.data32[0] = 0; //_NET_WM_STATE_REMOVE
                }
                else
                {
                    event.data.data32[0] = 1; //_NET_WM_STATE_ADD
                }

                xcb_send_event(QX11Info::connection(), 0, QX11Info::appRootWindow(QX11Info::appScreen()),
                               XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                               (const char *)&event);

                return false;
            }
            lastButtonPressTime = deviceEvent->time;

            //            xcb_client_message_event_t event;
            //            event.response_type = XCB_CLIENT_MESSAGE;
            //            event.format = 32;
            //            event.sequence = 0;
            //            event.window = q_ptr->getWindowHandle();
            //            event.type = xcb_intern_atom_reply(QX11Info::connection(), xcb_intern_atom(QX11Info::connection(), 0, 18, "_NET_WM_MOVERESIZE"), nullptr)->atom;
            //            event.data.data32[0] = fixed1616ToInt(deviceEvent->root_x);
            //            event.data.data32[1] = fixed1616ToInt(deviceEvent->root_y);
            //            // Either one of the different resize events or a Move event
            //            event.data.data32[2] = static_cast<int>(doBorderHitTest(getCurrentWindowFrame(), mousePos, borderWidth));
            //            event.data.data32[3] = XCB_BUTTON_INDEX_1;
            //            event.data.data32[4] = 0;

            //            XUngrabPointer(QX11Info::display(), XCB_CURRENT_TIME);
            //            xcb_send_event(QX11Info::connection(), 0, QX11Info::appRootWindow(QX11Info::appScreen()),
            //                           XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
            //                           (const char *)&event);
            //            XFlush(QX11Info::display());

            XEvent xev;
            // Set type of event
            // Either one of the different resize events or a Move event
            xev.xclient.data.l[2] = static_cast<int>(doBorderHitTest(getCurrentWindowFrame(), mousePos, borderWidth));

            Atom netMoveResize = XInternAtom(QX11Info::display(), "_NET_WM_MOVERESIZE", False);
            xev.xclient.type = ClientMessage;
            xev.xclient.message_type = netMoveResize;
            xev.xclient.display = QX11Info::display();
            xev.xclient.window = q_ptr->getWindowHandle();
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = fixed1616ToInt(deviceEvent->root_x);
            xev.xclient.data.l[1] = fixed1616ToInt(deviceEvent->root_y);

            xev.xclient.data.l[3] = Button1;
            xev.xclient.data.l[4] = 0;

            XUngrabPointer(QX11Info::display(), XCB_CURRENT_TIME);
            XSendEvent(QX11Info::display(), QX11Info::appRootWindow(QX11Info::appScreen()), False,
                       SubstructureRedirectMask | SubstructureNotifyMask, &xev);

            XFlush(QX11Info::display());

            isSystemOpRunning = true;

            return false;
        }
    }

    return false;
}

#endif
