#ifdef __linux__

#include "InputSimulation.h"

//Solution using Xlib for those who use Linux
#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

void InputSimulation::SimulateLeftClick()
{
    Display *display = XOpenDisplay(nullptr);

        XEvent event;

        if(display == nullptr)
        {
            fprintf(stderr, "Cannot initialize the display\n");
            return;
        }

        memset(&event, 0x00, sizeof(event));

        event.type = ButtonPress;
        event.xbutton.button = Button1;
        event.xbutton.same_screen = True;

        XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

        event.xbutton.subwindow = event.xbutton.window;

        while(event.xbutton.subwindow)
        {
            event.xbutton.window = event.xbutton.subwindow;

            XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
        }

        if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");

        XFlush(display);

        event.type = ButtonRelease;
        event.xbutton.state = 0x100;

        if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Error\n");

        XFlush(display);

        XCloseDisplay(display);
}

#endif
