#ifndef FRAMELESSWINDOWCONVERTERLINUX_H
#define FRAMELESSWINDOWCONVERTERLINUX_H

#ifdef __linux__

#include <FramelessWindowConverter_p.h>

namespace FWC
{

class FramelessWindowConverterLinux : public FramelessWindowConverterPrivate
{
public:
    FramelessWindowConverterLinux(FramelessWindowConverter* q);
    void convertToFrameless() override;
    bool filterNativeEvent(void *message, long *result) override;
    void minimizeWindow() override;
    void maximizeWindow() override;
    void restoreWindow() override;
    void closeWindow() override;

private:
    void changeCursorShape(unsigned int shape);
    int borderWidth;
    int xiOpCode; // XInput
    unsigned int lastButtonPressTime;   
    FWCRect getCurrentWindowFrame();
};

}

#endif
#endif // FRAMELESSWINDOWCONVERTERLINUX_H
