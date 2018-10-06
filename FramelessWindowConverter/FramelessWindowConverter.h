#ifndef WINDOWFRAME_H
#define WINDOWFRAME_H

#include <functional>

namespace FWC
{

struct FWCPARAMS
{
    unsigned long long windowHandle;
    std::function<void(void)> releaseMouseGrab;
    std::function<bool(int,int)> shouldPerformWindowDrag;
};

class FramelessWindowConverter
{
public:
    FramelessWindowConverter();
    virtual ~FramelessWindowConverter();

    virtual bool filterNativeEvents(void* message, long *result) final;

    void convertWindowToFrameless(unsigned long long inWindowHandle, std::function<void(void)> releaseMouseGrab = [](){}, std::function<bool(int,int)> inShouldPerformWindowDrag =
            [](int, int) { return true; } );

    void convertWindowToFrameless(const FWCPARAMS& fwcParams);
    void convertToWindowWithFrame();
    bool isFrameless();

    inline unsigned long long getWindowHandle() { return windowHandle; }

    inline std::function<bool(int,int)> getShouldPerformWindowDrag() { return shouldPerformWindowDrag; }

    // Decide if the window should be dragged when a mouse click was detected at current mouse Position
    // default inShouldPerformWindowDrag means you can click anywhere in the window to move it
    void setShouldPerformWindowDrag(std::function<bool(int,int)> inShouldPerformWindowDrag);

    // Release Mouse Grab to give the window manager control over it
    inline std::function<void(void)> getReleaseMouseGrab() { return releaseMouseGrab; }
    void setReleaseMouseGrab(std::function<void(void)> inReleaseMouseGrab);

    // Border Width for resizing (default is 8 pixels)
    void setBorderWidth(int inBorderWidth);
    int getBorderWidth();

    // Minimum and maximum window sizes
    int getMinimumWindowWidth();
    int getMinimumWindowHeight();
    int getMaximumWindowWidth();
    int getMaximumWindowHeight();

    void setMinimumWindowWidth(int inWidth);
    void setMinimumWindowHeight(int inHeight);
    void setMaximumWindowWidth(int inWidth);
    void setMaximumWindowHeight(int inHeight);

    void setMinMaxWindowSizes(int inMinWidth, int inMinHeight, int inMaxWidth, int inMaxHeight);

    // Window control
    void hideForTranslucency();
    void showForTranslucency();
    void minimizeWindow();
    void maximizeWindow();
    void restoreWindow();
    void closeWindow();
    void toggleFullscreen();

    // macOS settings
    void useTrafficLightsOnMacOS(bool inUseTrafficLights);
    bool isUsingTrafficLightsOnMacOS();

    // Positions are relative to upper left window corner
    void setPosOfGreenTrafficLightOnMacOS(int inXPos, int inYPos);
    void setPosOfRedTrafficLightOnMacOS(int inXPos, int inYPos);
    void setPosOfYellowTrafficLightOnMacOS(int inXPos, int inYPos);

    int getXPosOfGreenTrafficLightOnMacOS();
    int getYPosOfGreenTrafficLightOnMacOS();

    int getXPosOfRedTrafficLightOnMacOS();
    int getYPosOfRedTrafficLightOnMacOS();

    int getXPosOfYellowTrafficLightOnMacOS();
    int getYPosOfYellowTrafficLightOnMacOS();

private:
    class FramelessWindowConverterPrivate* d_ptr;
    unsigned long long windowHandle;
    std::function<bool(int,int)> shouldPerformWindowDrag;
    std::function<void(void)> releaseMouseGrab;
    int borderWidth = 8;
    int minimumWindowWidth;
    int minimumWindowHeight;
    int maximumWindowWidth;
    int maximumWindowHeight; 
    bool bUseTrafficLights = false;
    bool bIsFramless = false;

    // Default is horizontal
    int xPosOfGreen = 48, yPosOfGreen = 3;
    int xPosOfRed = 7, yPosOfRed = 3;
    int xPosOfYellow = 28, yPosOfYellow = 3;
};

}

#endif // WINDOWFRAME_H
