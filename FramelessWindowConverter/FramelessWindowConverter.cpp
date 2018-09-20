#include "FramelessWindowConverter.h"
#include "FramelessWindowConverter_p.h"

using namespace FWC;

FramelessWindowConverter::FramelessWindowConverter() : windowHandle(-1),
    d_ptr(FramelessWindowConverterPrivate::create(this)),  minimumWindowWidth(-1), minimumWindowHeight(-1) ,maximumWindowWidth(-1), maximumWindowHeight(-1)
{

}

FramelessWindowConverter::~FramelessWindowConverter()
{

}

void FramelessWindowConverter::convertWindowToFrameless(unsigned long long inWindowHandle, std::function<bool(int,int)> inShouldPerformWindowDrag)
{
    windowHandle = inWindowHandle;
    shouldPerformWindowDrag = inShouldPerformWindowDrag;
    d_ptr->convertToFrameless();
}

bool FramelessWindowConverter::filterNativeEvents(void *message, long *result)
{
    return d_ptr->filterNativeEvent(message, result);
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

