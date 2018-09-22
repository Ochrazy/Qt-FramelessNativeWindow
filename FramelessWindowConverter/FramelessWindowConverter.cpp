#include "FramelessWindowConverter.h"
#include "FramelessWindowConverter_p.h"

using namespace FWC;

FramelessWindowConverter::FramelessWindowConverter() : d_ptr(FramelessWindowConverterPrivate::create(this)),
    windowHandle(-1ull), minimumWindowWidth(-1), minimumWindowHeight(-1) ,maximumWindowWidth(-1), maximumWindowHeight(-1)
{

}

FramelessWindowConverter::~FramelessWindowConverter()
{

}

void FramelessWindowConverter::convertWindowToFrameless(unsigned long long inWindowHandle, std::function<void(void)> inReleaseMouseGrab, std::function<bool(int,int)> inShouldPerformWindowDrag)
{
    windowHandle = inWindowHandle;
    setShouldPerformWindowDrag(inShouldPerformWindowDrag);
    setReleaseMouseGrab(inReleaseMouseGrab);
    d_ptr->convertToFrameless();
}

void FramelessWindowConverter::convertWindowToFrameless(const FWCPARAMS& fwcParams)
{
    convertWindowToFrameless(fwcParams.windowHandle, fwcParams.releaseMouseGrab, fwcParams.shouldPerformWindowDrag);
}


bool FramelessWindowConverter::filterNativeEvents(void *message, long *result)
{
    return d_ptr->filterNativeEvent(message, result);
}

void FramelessWindowConverter::setShouldPerformWindowDrag(std::function<bool(int,int)> inShouldPerformWindowDrag)
{
    if(inShouldPerformWindowDrag != nullptr) shouldPerformWindowDrag = inShouldPerformWindowDrag;
    else shouldPerformWindowDrag = [](int, int ) { return true; };
}

void FramelessWindowConverter::setReleaseMouseGrab(std::function<void(void)> inReleaseMouseGrab)
{
    if(inReleaseMouseGrab != nullptr) releaseMouseGrab = inReleaseMouseGrab;
    else releaseMouseGrab = []() {};
}

void FramelessWindowConverter::minimizeWindow()
{
    d_ptr->minimizeWindow();
}

void FramelessWindowConverter::maximizeWindow()
{
    d_ptr->maximizeWindow();
}

void FramelessWindowConverter::closeWindow()
{
    d_ptr->closeWindow();
}

void FramelessWindowConverter::restoreWindow()
{
    d_ptr->restoreWindow();
}

void FramelessWindowConverter::hideForTranslucency()
{
    d_ptr->hideForTranslucency();
}

void FramelessWindowConverter::showForTranslucency()
{
    d_ptr->showForTranslucency();
}

