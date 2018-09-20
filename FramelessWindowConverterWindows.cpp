#ifdef WIN32

#include "FramelessWindowConverterWindows.h"
#include "FramelessWindowConverter.h"
#include <windowsx.h>
#include <Uxtheme.h>
#include "dwmapi.h"

#include <qdebug.h>

using namespace FWC;

FramelessWindowConverterWindows::FramelessWindowConverterWindows(FramelessWindowConverter* q) : FramelessWindowConverterPrivate(q)
{
}

void FramelessWindowConverterWindows::convertToFrameless()
{
    handle =(HWND)q_ptr->getWindowHandle();
    set_borderless(true);
}

void FramelessWindowConverterWindows::minimizeWindow()
{
    SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) | WS_CAPTION);
    ShowWindow(handle, SW_MINIMIZE);
}

void FramelessWindowConverterWindows::maximizeWindow()
{
    ShowWindow(handle, SW_MAXIMIZE);
}

void FramelessWindowConverterWindows::restoreWindow()
{
    ShowWindow(handle, SW_RESTORE);
}

void FramelessWindowConverterWindows::closeWindow()
{
    SendMessage(handle, WM_CLOSE, 0, 0);
}

FWCRect FramelessWindowConverterWindows::getCurrentClientRect()
{
    RECT WINRect;
    if(!GetClientRect(handle, &WINRect))
    {
        return FWCRect(-1,-1,-1,-1);
    }
    return FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom);
}

FWCRect FramelessWindowConverterWindows::getCurrentWindowRect()
{
    RECT WINRect;
    if(!GetWindowRect(handle, &WINRect))
    {
        return FWCRect(-1,-1,-1,-1);
    }
    return FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom);
}

FWCPoint FramelessWindowConverterWindows::getCurrentMousePos(LPARAM lParam)
{
    return  FWCPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}

static QByteArray debugPWP(UINT pwp)
{
    QByteArray rc = "0x";
    rc += QByteArray::number(qulonglong(pwp), 16);
    if (pwp & SWP_DRAWFRAME)
        rc += " SWP_DRAWFRAME";
    if (pwp & SWP_HIDEWINDOW)
        rc += " SWP_HIDEWINDOW";
    if (pwp & SWP_NOACTIVATE)
        rc += " SWP_NOACTIVATE";
    if (pwp & SWP_NOCOPYBITS)
        rc += " SWP_NOCOPYBITS";
    if (pwp & SWP_NOMOVE)
        rc += " SWP_NOMOVE";
    if (pwp & SWP_NOOWNERZORDER)
        rc += " SWP_ NOOWNERZORDER";
    if (pwp & SWP_NOREDRAW)
        rc += " SWP_NOREDRAW";
    if (pwp & SWP_NOREPOSITION)
        rc += " SWP_NOREPOSITION";
    if (pwp & SWP_NOSENDCHANGING)
        rc += " SWP_NOSENDCHANGING";
    if (pwp & SWP_NOSIZE)
        rc += " SWP_NOSIZE";
    if (pwp & SWP_NOZORDER)
        rc += " SWP_NOZORDER";
    if (pwp & SWP_SHOWWINDOW)
        rc += " SWP_SHOWWINDOW";

    return rc;
}

bool FramelessWindowConverterWindows::filterNativeEvent(void *message, long *result)
{
    #if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
    MSG* msg = *reinterpret_cast<MSG**>(message); // Nice Bug Qt...
    #else
    MSG* msg = reinterpret_cast<MSG*>(message);
    #endif    

    switch (msg->message)
    {
    case WM_NCCALCSIZE:
    {
        if (msg->wParam == TRUE && borderless) {
            //  auto& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);

            //adjust_maximized_client_rect(handle, params.rgrc[0]);

            // SetWindowTheme(handle, L"", L"");
            // SetWindowRgn(handle, NULL, true);

            *result = 0;
            return true;
        }
        break;
    }
    case WM_NCACTIVATE:
    {
        // Prevents window frame reappearing on window activation in "basic" theme,
        // where no aero shadow is present.
        *result = 1;
        return false;

    }
        //    case WM_WINDOWPOSCHANGED:
        //    {
        //        WINDOWPOS wp = *reinterpret_cast<WINDOWPOS*>(msg->lParam);
        //        if(wp.flags & SWP_SHOWWINDOW)
        //        {
        //            qDebug("bbbb");
        //            if(doCaption)
        //            {
        //                SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) & ~WS_CAPTION);
        //                doCaption = false;
        //            }
        //        }
        //        break;
        //        qDebug("bbbb");
        //        if(doCaption)
        //        {
        //            SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) & ~WS_CAPTION);
        //            SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        //            ShowWindow(handle, SW_SHOW);

        //            doCaption = false;
        //        }
        //    }
    case WM_ACTIVATEAPP:
    {
        if(msg->wParam == TRUE)
        {
            // Remove WS_CAPTION when switching from another window
            WINDOWPLACEMENT wpm;

            GetWindowPlacement(handle, &wpm);
            if(wpm.showCmd != SW_SHOWMINIMIZED)
            {
                SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) & ~WS_CAPTION);
            }
        }
        break;
    }
    case WM_ACTIVATE :
    {
        // Enable Minimize Animation by adding/removing WS_CAPTION window style
        if(LOWORD(msg->wParam) == WA_ACTIVE && HIWORD(msg->wParam) == 0) // not minimized
        {
            SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) & ~WS_CAPTION);
        }
        else if(LOWORD(msg->wParam) == WA_INACTIVE && HIWORD(msg->wParam) == 0) // not minimized
        {
            // This actually also gets called when switching to another window (see WM_ACTIVATEAPP for the fix)
            SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) | WS_CAPTION);
        }
        //  }
        return false;
        break;
    }
    case WM_NCHITTEST:
    {
        // Not using WM_NCHITTEST.
        // QAbstractNativeEventFilter does not send this message (bug?).
        // Instead WM_LBUTTONDBLCLK and WM_LBUTTONDOWN is used to send WM_NCLBUTTON*** events manually
        // WM_MOUSEMOVE is used to set appropriate cursor.
        // This is also more consistent with the implementation on other platforms (linux, mac).
        break;

        //        switch(doBorderHitTest(getCurrentWindowRect(), getCurrentMousePos(msg->lParam), 8))
        //        {
        //        case FWCBorderHitTestResult::TOP_LEFT:
        //            *result = HTTOPLEFT;
        //            return true;
        //        case FWCBorderHitTestResult::TOP:
        //            *result = HTTOP;
        //            return true;
        //        case FWCBorderHitTestResult::TOP_RIGHT:
        //            *result = HTTOPRIGHT;
        //            return true;
        //        case FWCBorderHitTestResult::RIGHT:
        //            *result = HTRIGHT;
        //            return true;
        //        case FWCBorderHitTestResult::BOTTOM_RIGHT:
        //            *result = HTBOTTOMRIGHT;
        //            return true;
        //        case FWCBorderHitTestResult::BOTTOM:
        //            *result = HTBOTTOM;
        //            return true;
        //        case FWCBorderHitTestResult::BOTTOM_LEFT:
        //            *result = HTBOTTOMLEFT;
        //            return true;
        //        case FWCBorderHitTestResult::LEFT:
        //            *result = HTLEFT;
        //            return true;
        //        case FWCBorderHitTestResult::CLIENT:
        //            *result = HTCAPTION;
        //            return true;
        //        case FWCBorderHitTestResult::NONE:
        //            *result = 0;
        //            return false;
        //        }
    }
    case WM_LBUTTONDBLCLK:
    {
        // Exclude Child Widgets
        FWCPoint mousePos(getCurrentMousePos(msg->lParam));

        // Only this widget is used for dragging.
        if (!q_ptr->getShouldPerformWindowDrag()(mousePos.x, mousePos.y))
        {
            return false;
        }

        if(doBorderHitTest(getCurrentClientRect(), mousePos, 8) == FWCBorderHitTestResult::CLIENT)
        {
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDBLCLK, HTCAPTION, msg->lParam);
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        FWCPoint mousePos(getCurrentMousePos(msg->lParam));

        // Only this widget is used for dragging.
        if (!q_ptr->getShouldPerformWindowDrag()(mousePos.x, mousePos.y))
        {
            return false;
        }

        switch (doBorderHitTest(getCurrentClientRect(), mousePos, 8))
        {
        case FWCBorderHitTestResult::LEFT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTLEFT, msg->lParam);
            break;
        case FWCBorderHitTestResult::RIGHT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTRIGHT, msg->lParam);
            break;
        case FWCBorderHitTestResult::TOP:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTTOP, msg->lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTBOTTOM, msg->lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM_LEFT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTBOTTOMLEFT, msg->lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM_RIGHT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, msg->lParam);
            break;
        case FWCBorderHitTestResult::TOP_LEFT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTTOPLEFT, msg->lParam);
            break;
        case FWCBorderHitTestResult::TOP_RIGHT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTTOPRIGHT, msg->lParam);
            break;
        default:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTCAPTION, msg->lParam);
            break;
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        switch (doBorderHitTest(getCurrentClientRect(), getCurrentMousePos(msg->lParam), 8))
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
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* minMaxInfo = ( MINMAXINFO* )msg->lParam;

        bool bMinMaxInfo = false;
        if(q_ptr->getMinimumWindowWidth() >= 0)
        {
            minMaxInfo->ptMinTrackSize.x = q_ptr->getMinimumWindowWidth();
            bMinMaxInfo = true;
        }

        if(q_ptr->getMinimumWindowHeight() >= 0)
        {
            minMaxInfo->ptMinTrackSize.y = q_ptr->getMinimumWindowHeight();
            bMinMaxInfo = true;
        }

        if(q_ptr->getMaximumWindowWidth() >= 0)
        {
            minMaxInfo->ptMaxTrackSize.x = q_ptr->getMaximumWindowWidth();
            bMinMaxInfo = true;
        }

        if(q_ptr->getMaximumWindowHeight() >= 0)
        {
            minMaxInfo->ptMaxTrackSize.y =  q_ptr->getMaximumWindowHeight();
            bMinMaxInfo = true;
        }

        if(bMinMaxInfo)
        {
            *result = 0;
            return true;
        }
        else return false;
    }
    }
    return false;
}

//auto maximized(HWND hwnd) -> bool {
//     WINDOWPLACEMENT placement;
//     if (!::GetWindowPlacement(hwnd, &placement)) {
//           return false;
//     }

//     return placement.showCmd == SW_MAXIMIZE;
// }

///* Adjust client rect to not spill over monitor edges when maximized.
//    * rect(in/out): in: proposed window rect, out: calculated client rect
//    * Does nothing if the window is not maximized.
//    */
//   auto adjust_maximized_client_rect(HWND window, RECT& rect) -> void {
//       if (!maximized(window)) {
//           return;
//       }

//       auto monitor = ::MonitorFromWindow(window, MONITOR_DEFAULTTONULL);
//       if (!monitor) {
//           return;
//       }

//       MONITORINFO monitor_info{};
//       monitor_info.cbSize = sizeof(monitor_info);
//       if (!::GetMonitorInfoW(monitor, &monitor_info)) {
//           return;
//       }

//       // when maximized, make the client area fill just the monitor (without task bar) rect,
//       // not the whole window rect which extends beyond the monitor.
//       rect = monitor_info.rcWork;
//   }

enum class Style : DWORD {
    windowed         = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WM_CLOSE,
    aero_borderless  = WS_POPUP | WS_THICKFRAME /*| WS_CAPTION*/ | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WM_CLOSE
};

void FramelessWindowConverterWindows::set_borderless(bool enabled)
{
    Style new_style = (enabled) ? Style::aero_borderless : Style::windowed;
    Style old_style = static_cast<Style>(::GetWindowLongPtrW(handle, GWL_STYLE));

    if (new_style != old_style) {
        borderless = enabled;

        SetWindowLongPtrW(handle, GWL_STYLE, static_cast<LONG>(new_style));

        // when switching between borderless and windowed, restore appropriate shadow state
        // set_shadow(handle.get(), borderless_shadow && (new_style != Style::windowed));

        // redraw frame
        SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(handle, SW_SHOW);

    }
}

#endif
