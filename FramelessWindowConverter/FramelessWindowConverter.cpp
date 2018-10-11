#include "FramelessWindowConverter.h"
#include "FramelessWindowConverter_p.h"

using namespace FWC;

FramelessWindowConverter::FramelessWindowConverter() : d_ptr(FramelessWindowConverterPrivate::create(this)),
    windowHandle(0), minimumWindowWidth(-1), minimumWindowHeight(-1) ,maximumWindowWidth(-1), maximumWindowHeight(-1)
{

}

FramelessWindowConverter::~FramelessWindowConverter()
{

}

void FramelessWindowConverter::convertWindowToFrameless(unsigned long long inWindowHandle, std::function<void(void)> inReleaseMouseGrab, std::function<bool(int,int)> inShouldPerformWindowDrag)
{
    bIsFramless = true;
    windowHandle = inWindowHandle;
    setShouldPerformWindowDrag(inShouldPerformWindowDrag);
    setReleaseMouseGrab(inReleaseMouseGrab);
    d_ptr->convertToFrameless();
}

void FramelessWindowConverter::convertToWindowWithFrame()
{
    bIsFramless = false;
    d_ptr->convertToWindowWithFrame();
}

bool FramelessWindowConverter::getIsFrameless()
{
    return bIsFramless;
}

void FramelessWindowConverter::convertWindowToFrameless(const FWCPARAMS& fwcParams)
{
    convertWindowToFrameless(fwcParams.windowHandle, fwcParams.releaseMouseGrab, fwcParams.shouldPerformWindowDrag);
}


bool FramelessWindowConverter::filterNativeEvents(void *message, long *result)
{
    if(bIsFramless)
        return d_ptr->filterNativeEvent(message, result);
    else return false;
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

void FramelessWindowConverter::toggleFullscreen()
{
    d_ptr->toggleFullscreen();
}

void FramelessWindowConverter::hideForTranslucency()
{
    d_ptr->hideForTranslucency();
}

void FramelessWindowConverter::showForTranslucency()
{
    d_ptr->showForTranslucency();
}

void FramelessWindowConverter::setEnableShadow(bool inEnableShadow)
{
    bHasShadow = inEnableShadow;
}

bool FramelessWindowConverter::getHasShadow()
{
    return bHasShadow;
}

void FramelessWindowConverter::setBorderWidth(int inBorderWidth) { borderWidth = inBorderWidth; }
int FramelessWindowConverter::getBorderWidth() { return borderWidth; }

int FramelessWindowConverter::getMinimumWindowWidth() { return minimumWindowWidth; }
int FramelessWindowConverter::getMinimumWindowHeight() { return minimumWindowHeight; }
int FramelessWindowConverter::getMaximumWindowWidth() { return maximumWindowWidth; }
int FramelessWindowConverter::getMaximumWindowHeight() { return maximumWindowHeight; }

void FramelessWindowConverter::setMinimumWindowWidth(int inWidth) { minimumWindowWidth = inWidth; }
void FramelessWindowConverter::setMinimumWindowHeight(int inHeight) { minimumWindowHeight = inHeight; }
void FramelessWindowConverter::setMaximumWindowWidth(int inWidth) { maximumWindowWidth = inWidth; }
void FramelessWindowConverter::setMaximumWindowHeight(int inHeight) { maximumWindowHeight = inHeight; }

void FramelessWindowConverter::setMinMaxWindowSizes(int inMinWidth, int inMinHeight, int inMaxWidth, int inMaxHeight)
{
    minimumWindowWidth = inMinWidth;
    minimumWindowHeight = inMinHeight;
    maximumWindowWidth = inMaxWidth;
    maximumWindowHeight = inMaxHeight;
}

void FramelessWindowConverter::useTrafficLightsOnMacOS(bool inUseTrafficLights)
{
    bUseTrafficLights = inUseTrafficLights;
}

bool FramelessWindowConverter::isUsingTrafficLightsOnMacOS()
{
    return bUseTrafficLights;
}

void FramelessWindowConverter::setHiddenGreenTrafficLightOnMacOS(bool inHidden) { bHiddenGreen = inHidden; d_ptr->setHiddenGreenTrafficLightOnMacOS(inHidden); }
void FramelessWindowConverter::setHiddenRedTrafficLightOnMacOS(bool inHidden) { bHiddenRed = inHidden; d_ptr->setHiddenRedTrafficLightOnMacOS(inHidden); }
void FramelessWindowConverter::setHiddenYellowTrafficLightOnMacOS(bool inHidden) { bHiddenYellow = inHidden; d_ptr->setHiddenYellowTrafficLightOnMacOS(inHidden); }

bool FramelessWindowConverter::getHiddenGreenTrafficLightOnMacOS() { return bHiddenGreen; }
bool FramelessWindowConverter::getHiddenRedTrafficLightOnMacOS() { return bHiddenRed; }
bool FramelessWindowConverter::getHiddenYellowTrafficLightOnMacOS() { return bHiddenYellow; }

void FramelessWindowConverter::setEnabledGreenTrafficLightOnMacOS(bool inEnabled) { bEnabledGreen = inEnabled; d_ptr->setEnabledGreenTrafficLightOnMacOS(inEnabled); }
void FramelessWindowConverter::setEnabledRedTrafficLightOnMacOS(bool inEnabled) { bEnabledRed = inEnabled; d_ptr->setEnabledRedTrafficLightOnMacOS(inEnabled);}
void FramelessWindowConverter::setEnabledYellowTrafficLightOnMacOS(bool inEnabled) { bEnabledYellow = inEnabled; d_ptr->setEnabledYellowTrafficLightOnMacOS(inEnabled);}

bool FramelessWindowConverter::getEnabledGreenTrafficLightOnMacOS() { return bEnabledGreen; }
bool FramelessWindowConverter::getEnabledRedTrafficLightOnMacOS() { return bEnabledRed; }
bool FramelessWindowConverter::getEnabledYellowTrafficLightOnMacOS() { return bEnabledYellow; }

void FramelessWindowConverter::setPosOfGreenTrafficLightOnMacOS(int inXPos, int inYPos)
{
    xPosOfGreen = inXPos;
    yPosOfGreen = inYPos;
}

void FramelessWindowConverter::setPosOfRedTrafficLightOnMacOS(int inXPos, int inYPos)
{
    xPosOfRed = inXPos;
    yPosOfRed = inYPos;
}

void FramelessWindowConverter::setPosOfYellowTrafficLightOnMacOS(int inXPos, int inYPos)
{
    xPosOfYellow = inXPos;
    yPosOfYellow = inYPos;
}

int FramelessWindowConverter::getXPosOfGreenTrafficLightOnMacOS() { return xPosOfGreen; }
int FramelessWindowConverter::getYPosOfGreenTrafficLightOnMacOS() { return yPosOfGreen; }

int FramelessWindowConverter::getXPosOfRedTrafficLightOnMacOS() { return xPosOfRed; }
int FramelessWindowConverter::getYPosOfRedTrafficLightOnMacOS() { return yPosOfRed; }

int FramelessWindowConverter::getXPosOfYellowTrafficLightOnMacOS() { return xPosOfYellow; }
int FramelessWindowConverter::getYPosOfYellowTrafficLightOnMacOS() { return yPosOfYellow; }
