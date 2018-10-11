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
    void toggleFullscreen() override;

private:
    FWCRect getCurrentClientRect();
    FWCRect getCurrentWindowRect();
    FWCPoint getCurrentMousePos(LPARAM lParam);
    HWND handle;

    // Not using WS_CAPTION in borderless, since it messes with translucent Qt-Windows.
    enum class Style : DWORD {
        windowed         = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        aero_borderless  = WS_POPUP | WS_THICKFRAME /*| WS_CAPTION*/ | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX
    };
    void setFrameless(bool enabled);
    class FramelessWindowConverterPrivate* d_ptr;
};

}

#endif
#endif // WINDOWSNATIVEWINDOWEVENTFILTER_P_H
