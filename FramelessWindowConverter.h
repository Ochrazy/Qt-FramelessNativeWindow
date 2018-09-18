#ifndef WINDOWFRAME_H
#define WINDOWFRAME_H

#include <functional>

namespace FWC
{

class FramelessWindowConverter
{
public:
    FramelessWindowConverter();
    virtual ~FramelessWindowConverter();

    virtual bool filterNativeEvents(void* message, long *result) final;

    // Decide if the window should be dragged when a mouse click was detected at current mouse Position
    // Override this function to customize window dragging
    //virtual bool shouldPerformWindowDrag(int mousePosXInWindow, int mousePosYInWindow);
    // default inShouldPerformWindowDrag means you can click anywhere in the window to move it
    void convertWindowToFrameless(unsigned long long inWindowHandle, std::function<bool(int,int)> inShouldPerformWindowDrag =
            [](int mousePosXInWindow, int mousePosYInWindow) { return true; } );

    inline unsigned long long getWindowHandle() { return windowHandle; }
    inline std::function<bool(int,int)> getShouldPerformWindowDrag() { return shouldPerformWindowDrag; }
    inline std::function<bool(int,int)> setShouldPerformWindowDrag(std::function<bool(int,int)> inShouldPerformWindowDrag) { shouldPerformWindowDrag = inShouldPerformWindowDrag; }

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
    std::function<void(void)> repaint;

private:
    unsigned long long windowHandle;
    std::function<bool(int,int)> shouldPerformWindowDrag;
    int minimumWindowWidth;
    int minimumWindowHeight;
    int maximumWindowWidth;
    int maximumWindowHeight;
    class FramelessWindowConverterPrivate* d_ptr;
};

}

#endif // WINDOWFRAME_H
