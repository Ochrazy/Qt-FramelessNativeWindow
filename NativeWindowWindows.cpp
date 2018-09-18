#ifdef WIN32

#include "NativeWindowWindows.h"
#include <Windows.h>
#include <windowsx.h>
#include <stdexcept>
#include <Uxtheme.h>

struct FWCRect
{
    int x1;
    int y1;
    int x2;
    int y2;

    FWCRect(int inX1, int inY1, int inX2, int inY2) : x1(inX1), y1(inY1), x2(inX2), y2(inY2) {}
    inline int left()  { return x1; }
    inline int right()  { return x2; }
    inline int top()  { return y1; }
    inline int bottom()  { return y2; }
};

struct FWCPoint
{
    int x;
    int y;

    FWCPoint(int inX, int inY) : x(inX), y(inY) {}
};

//#define _NET_WM_MOVERESIZE_SIZE_TOPLEFT      0
//#define _NET_WM_MOVERESIZE_SIZE_TOP          1
//#define _NET_WM_MOVERESIZE_SIZE_TOPRIGHT     2
//#define _NET_WM_MOVERESIZE_SIZE_RIGHT        3
//#define _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT  4
//#define _NET_WM_MOVERESIZE_SIZE_BOTTOM       5
//#define _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT   6
//#define _NET_WM_MOVERESIZE_SIZE_LEFT         7
//#define _NET_WM_MOVERESIZE_MOVE              8   /* movement only */
enum class FWCBorderHitTestResult
{
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT,
    CLIENT,
    NONE
};

FWCBorderHitTestResult doBorderHitTest(FWCRect inWindowRect, FWCPoint inMousePosition, int borderWidth)
{
    FWCBorderHitTestResult bhtResult = FWCBorderHitTestResult::NONE; // outside window

    // Client (inside window)
    if(inMousePosition.x >= inWindowRect.left() + borderWidth && inMousePosition.x <= inWindowRect.right() - borderWidth &&
            inMousePosition.y <= inWindowRect.bottom() - borderWidth && inMousePosition.y >= inWindowRect.top() + borderWidth)
    {
        return FWCBorderHitTestResult::CLIENT;
    }
    //left border
    if (inMousePosition.x >= inWindowRect.left() && inMousePosition.x <= inWindowRect.left() + borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::LEFT;
    }
    //right border
    else if (inMousePosition.x <= inWindowRect.right() && inMousePosition.x >= inWindowRect.right() - borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::RIGHT;
    }
    //bottom border
    else if (inMousePosition.y <= inWindowRect.bottom() && inMousePosition.y >= inWindowRect.bottom() - borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::BOTTOM;
    }
    //top border
    else if (inMousePosition.y >= inWindowRect.top() && inMousePosition.y <= inWindowRect.top() + borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::TOP;
    }

    //bottom left corner
    if (bhtResult == FWCBorderHitTestResult::LEFT &&
            inMousePosition.y <= inWindowRect.bottom() && inMousePosition.y >= inWindowRect.bottom() - borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::BOTTOM_LEFT;
    }
    //bottom right corner
    else if (bhtResult == FWCBorderHitTestResult::RIGHT &&
             inMousePosition.y <= inWindowRect.bottom() && inMousePosition.y >= inWindowRect.bottom() - borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::BOTTOM_RIGHT;
    }
    //top left corner
    else if (bhtResult == FWCBorderHitTestResult::LEFT &&
             inMousePosition.y >= inWindowRect.top() && inMousePosition.y <= inWindowRect.top() + borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::TOP_LEFT;
    }
    //top right corner
    else if (bhtResult == FWCBorderHitTestResult::RIGHT &&
             inMousePosition.y >= inWindowRect.top() && inMousePosition.y <= inWindowRect.top() + borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::TOP_RIGHT;
    }

    return bhtResult;
}

FWCRect NativeWindowWindows::getCurrentClientRect()
{
    RECT WINRect;
    if(!GetClientRect(hWnd, &WINRect))
    {
        return FWCRect(-1,-1,-1,-1);
    }
    return FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom);
}

FWCRect NativeWindowWindows::getCurrentWindowRect()
{
    RECT WINRect;
    if(!GetWindowRect(hWnd, &WINRect))
    {
        return FWCRect(-1,-1,-1,-1);
    }
    return FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom);
}

FWCPoint NativeWindowWindows::getCurrentMousePos(LPARAM lParam)
{
    return  FWCPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}

LRESULT CALLBACK NativeWindowWindows::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    switch (message)
    {
    case WM_NCACTIVATE:
        //return 0;
        break;
    case WM_NCCALCSIZE:
    {
        if (wParam == TRUE)
        {
            RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME |
                         RDW_ALLCHILDREN);
            BLENDFUNCTION blend;
            blend.BlendOp = AC_SRC_OVER;
            blend.BlendFlags = 0;
            blend.AlphaFormat = 0;
            blend.SourceConstantAlpha = 255;

            UpdateLayeredWindow(hWnd, NULL, NULL, NULL, NULL, NULL,
                                NULL, &blend, ULW_OPAQUE);
            return 0;
        }
        break;
    }
    case WM_LBUTTONDBLCLK:
    {

        // Exclude Child Widgets
        FWCPoint mousePos(FWCPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));

        RECT WINRect;
        GetClientRect(hWnd, &WINRect);

        if(doBorderHitTest(FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom), mousePos, 8) == FWCBorderHitTestResult::CLIENT)
        {
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDBLCLK, HTCAPTION, lParam);
        }

        break;
    }
    case WM_LBUTTONDOWN:
    {
         SetWindowLongPtr(hWnd, GWL_STYLE, GetWindowLongPtrW(hWnd, GWL_STYLE) | WS_CAPTION);
        //        AnimateWindow(hWnd, 500, AW_HIDE | AW_BLEND);
        //       AnimateWindow(hWnd, 500,  AW_BLEND);
        //        ShowWindow(hWnd, SW_HIDE);
        //        ShowWindow(hWnd, SW_SHOW);

        //        BLENDFUNCTION blend;
        //                   blend.BlendOp = AC_SRC_OVER;
        //                   blend.BlendFlags = 0;
        //                   blend.AlphaFormat = 0;
        //                   blend.SourceConstantAlpha = 255;

        //                   UpdateLayeredWindow(hWnd, NULL, NULL, NULL, NULL, NULL,
        //                           NULL, &blend, ULW_OPAQUE);
        //                return true;
        FWCPoint mousePos(FWCPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));

        RECT WINRect;
        GetClientRect(hWnd, &WINRect);

        switch (doBorderHitTest(FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom), mousePos, 8))
        {
        case FWCBorderHitTestResult::LEFT:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTLEFT, lParam);
            break;
        case FWCBorderHitTestResult::RIGHT:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTRIGHT, lParam);
            break;
        case FWCBorderHitTestResult::TOP:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTTOP,lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTBOTTOM,lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM_LEFT:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTBOTTOMLEFT, lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM_RIGHT:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, lParam);
            break;
        case FWCBorderHitTestResult::TOP_LEFT:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTTOPLEFT, lParam);
            break;
        case FWCBorderHitTestResult::TOP_RIGHT:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTTOPRIGHT, lParam);
            break;
        default:
            ReleaseCapture();
            SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
            break;
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        RECT WINRect;
        GetClientRect(hWnd, &WINRect);
        switch (doBorderHitTest(FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom), FWCPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)), 8))
        {
        case FWCBorderHitTestResult::LEFT:
            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
            break;
        case FWCBorderHitTestResult::RIGHT:
            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
            break;
        case FWCBorderHitTestResult::TOP:
            SetCursor(LoadCursor(nullptr, IDC_SIZENS));
            break;
        case FWCBorderHitTestResult::BOTTOM:
            SetCursor(LoadCursor(nullptr, IDC_SIZENS));
            break;
        case FWCBorderHitTestResult::BOTTOM_LEFT:
            SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
            break;
        case FWCBorderHitTestResult::BOTTOM_RIGHT:
            SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
            break;
        case FWCBorderHitTestResult::TOP_LEFT:
            SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
            break;
        case FWCBorderHitTestResult::TOP_RIGHT:
            SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
            break;
        default:
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            break;
        }
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

NativeWindowWindows::NativeWindowWindows()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASSEX wcx = { 0 };
    wcx.cbSize = sizeof( WNDCLASSEX );
    wcx.style = CS_HREDRAW | CS_VREDRAW;
    wcx.hInstance = hInstance;
    wcx.lpfnWndProc = &NativeWindowWindows::WndProc;
    wcx.cbClsExtra	= 0;
    wcx.cbWndExtra	= 0;
    wcx.lpszClassName = L"WindowClass";
    wcx.hbrBackground = CreateSolidBrush( RGB( 255, 255, 255 ));
    wcx.hCursor = LoadCursor(hInstance, IDC_ARROW );
    RegisterClassEx( &wcx );
    if ( FAILED( RegisterClassEx( &wcx ) ) ) throw std::runtime_error( "Couldn't register window class" );

    DWORD style =  WS_POPUP  | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
    hWnd = CreateWindowEx(WS_EX_LAYERED,  L"WindowClass", L"Frameless Window", style, 200, 200, 400, 300, 0, 0, hInstance, nullptr);

    if ( !hWnd ) throw std::runtime_error( "Couldn't create window because of reasons" );

    //    SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( this ) );
    //    // Transparent windows with click through
    //  SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED);
    //  SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);

    // Create memory DC
    HDC hdcScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hdcScreen);
    // Create memory bitmap
    HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, 400, 300);
    HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, hBmp);
    // Draw image to memory bitmap (currently selected in memory DC)
    RECT rect;
    rect.left = 0;
    rect.right = 400;
    rect.bottom = 300;
    rect.top = -50;
    SetBkMode(hDC, OPAQUE);
    FillRect(hDC, &rect, CreateSolidBrush( RGB( 128, 128, 128 )));

    // Call UpdateLayeredWindow
    BLENDFUNCTION blend = {0};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;// half transparent
    blend.AlphaFormat = AC_SRC_ALPHA;
    POINT ptLocation = {200, 200};
    SIZE szWnd = {400, 300};
    POINT ptSrc = {0, 0};
    UpdateLayeredWindow(hWnd, hdcScreen, &ptLocation, &szWnd, hDC, &ptSrc, 0, &blend, ULW_OPAQUE);



    //     COLORREF RRR = RGB(255, 0, 255);
    //     SetLayeredWindowAttributes(hWnd, RRR, (BYTE)51, LWA_ALPHA);

    ShowWindow( hWnd, SW_SHOW );
}

#endif
