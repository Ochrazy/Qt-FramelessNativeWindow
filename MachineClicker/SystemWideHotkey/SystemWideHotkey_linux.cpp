#ifdef __linux__

#include "SystemWideHotkey.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <QtX11Extras/qx11info_x11.h>
#include <QKeySequence>

void SystemWideHotkey::initialize()
{
    registerNewHotkey(Qt::Key_Space);
}

quint32 SystemWideHotkey::nativeKeycode(Qt::Key key)
{
    QString keyString = QKeySequence(key).toString(QKeySequence::NativeText);

    KeySym keysym = XStringToKeysym(keyString.toLatin1().constData());
    if (keysym == NoSymbol) {
        //not found -> just use the key
        if(key <= 0xFFFF)
            keysym = key;
        else
            return 0;
    }

    Display *display = QX11Info::display();
    if(display) {
        auto res = XKeysymToKeycode(QX11Info::display(), keysym);
        return res;
    } else
        return 0;
}

void SystemWideHotkey::registerNewHotkey(Qt::Key key)
{
    unregisterHotkey();

    unsigned int keycode = nativeKeycode(key);
    unsigned int    modifiers       = AnyModifier;//ControlMask | ShiftMask;
    Window          grab_window     =  QX11Info::appRootWindow(QX11Info::appScreen());
    Bool            owner_events    = True;
    int             pointer_mode    = GrabModeAsync;
    int             keyboard_mode   = GrabModeAsync;

    XGrabKey(QX11Info::display(), static_cast<int>(keycode), modifiers, grab_window, owner_events, pointer_mode,
             keyboard_mode);

    currentKeycode = keycode;
}

void SystemWideHotkey::unregisterHotkey()
{
    XUngrabKey(QX11Info::display(), static_cast<int>(currentKeycode), AnyModifier, QX11Info::appRootWindow(QX11Info::appScreen()));
}

bool SystemWideHotkey::nativeEventFilter(const QByteArray & eventType,
                                         void * message, long * result)
{
    Q_UNUSED(result);

    xcb_key_press_event_t *kev = nullptr;
    if (eventType == "xcb_generic_event_t")
    {
        xcb_generic_event_t* ev = static_cast<xcb_generic_event_t *>(message);

        switch( ev->response_type & ~0x80)
        {
        case XCB_KEY_PRESS:
            kev = static_cast<xcb_key_press_event_t *>(message);
            break;
        }

    }

    if (kev != nullptr)
    {
        unsigned int keycode = kev->detail;

        if(keycode == currentKeycode)
        {
            emit hotkeyPressed();
        }
    }

    return false;
}

#endif
