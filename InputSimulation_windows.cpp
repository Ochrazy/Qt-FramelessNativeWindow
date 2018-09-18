#ifdef WIN32

#include "InputSimulation.h"
#include <qt_windows.h>

void InputSimulation::SimulateLeftClick()
{
    INPUT ip = {};

    // Set up a generic keyboard event.
    ip.type = INPUT_MOUSE;
    // left down
    ip.type      = INPUT_MOUSE;
    ip.mi.dwFlags  = MOUSEEVENTF_LEFTDOWN;
    SendInput(1,&ip,sizeof(INPUT));

    // left up
    ZeroMemory(&ip,sizeof(ip));
    ip.type = INPUT_MOUSE;
    ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1,&ip,sizeof(INPUT));
}

#endif
