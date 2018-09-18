#ifdef WIN32

#ifndef NATIVEWINDOWWINDOWS_H
#define NATIVEWINDOWWINDOWS_H

#include <Windows.h>

class NativeWindowWindows
{
public:
    NativeWindowWindows();

private:
     static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
     HWND hWnd;
     struct FWCRect getCurrentClientRect();
     FWCRect getCurrentWindowRect();
     struct FWCPoint getCurrentMousePos(LPARAM lParam);

};

#endif
#endif // NATIVEWINDOWWINDOWS_H
