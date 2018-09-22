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

    inline unsigned long long getWindowHandle() { return windowHandle; }

    inline std::function<bool(int,int)> getShouldPerformWindowDrag() { return shouldPerformWindowDrag; }

    // Decide if the window should be dragged when a mouse click was detected at current mouse Position
    // default inShouldPerformWindowDrag means you can click anywhere in the window to move it
    void setShouldPerformWindowDrag(std::function<bool(int,int)> inShouldPerformWindowDrag);

    // Release Mouse Grab to give the window manager control over it
    inline std::function<void(void)> getReleaseMouseGrab() { return releaseMouseGrab; }
    void setReleaseMouseGrab(std::function<void(void)> inReleaseMouseGrab);

    int getMinimumWindowWidth() { return minimumWindowWidth; }
    int getMinimumWindowHeight() { return minimumWindowHeight; }
    int getMaximumWindowWidth() { return maximumWindowWidth; }
    int getMaximumWindowHeight() { return maximumWindowHeight; }

    void setMinimumWindowWidth(int inWidth) { minimumWindowWidth = inWidth; }
    void setMinimumWindowHeight(int inHeight) { minimumWindowHeight = inHeight; }
    void setMaximumWindowWidth(int inWidth) { maximumWindowWidth = inWidth; }
    void setMaximumWindowHeight(int inHeight) { maximumWindowHeight = inHeight; }

    void setMinMaxWindowSizes(int inMinWidth, int inMinHeight, int inMaxWidth, int inMaxHeight)
    {
        minimumWindowWidth = inMinWidth;
        minimumWindowHeight = inMinHeight;
        maximumWindowWidth = inMaxWidth;
        maximumWindowHeight = inMaxHeight;
    }

    void hideForTranslucency();
    void showForTranslucency();
    void minimizeWindow();
    void maximizeWindow();
    void restoreWindow();
    void closeWindow();

private:
    class FramelessWindowConverterPrivate* d_ptr;
    unsigned long long windowHandle;
    std::function<bool(int,int)> shouldPerformWindowDrag;
    std::function<void(void)> releaseMouseGrab;
    int minimumWindowWidth;
    int minimumWindowHeight;
    int maximumWindowWidth;
    int maximumWindowHeight; 
};

}

#endif // WINDOWFRAME_H
