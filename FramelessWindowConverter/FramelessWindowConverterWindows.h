#ifndef WINDOWSNATIVEWINDOWEVENTFILTER_P_H
#define WINDOWSNATIVEWINDOWEVENTFILTER_P_H

#ifdef WIN32

#include "FramelessWindowConverter_p.h"
#include <qt_windows.h>

namespace FWC
{

class FramelessWindowConverterWindows : public FramelessWindowConverterPrivate
{
public:
    FramelessWindowConverterWindows(FramelessWindowConverter* q);
    bool filterNativeEvent(void *message, long *result) override;
    void convertToFrameless() override;
    void convertToWindowWithFrame() override;
    void minimizeWindow() override;
    void maximizeWindow() override;
    void restoreWindow() override;
    void closeWindow() override;

private:
    FWCRect getCurrentClientRect();
    FWCRect getCurrentWindowRect();
    FWCPoint getCurrentMousePos(LPARAM lParam);
    HWND handle;
    void set_borderless(bool enabled);
    bool borderless        = true; // is the window currently borderless
    bool drag = false;
    class FramelessWindowConverterPrivate* d_ptr;
};

}

#endif
#endif // WINDOWSNATIVEWINDOWEVENTFILTER_P_H
