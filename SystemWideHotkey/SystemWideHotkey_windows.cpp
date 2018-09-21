#ifdef WIN32

#include "SystemWideHotkey.h"
#include <qt_windows.h>


void SystemWideHotkey::initialize()
{
    registerNewHotkey(Qt::Key_Space);
}

void SystemWideHotkey::registerNewHotkey(Qt::Key key)
{
    unregisterHotkey();

    unsigned int keycode = nativeKeycode(key);
    RegisterHotKey(nullptr, static_cast<int>(keycode), 0, keycode);
    currentKeycode = keycode;
}

void SystemWideHotkey::unregisterHotkey()
{
    UnregisterHotKey(nullptr, static_cast<int>(currentKeycode));
}

quint32 SystemWideHotkey::nativeKeycode(Qt::Key key)
{
    switch (key)
    {
    case Qt::Key_Escape:
        return VK_ESCAPE;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        return VK_TAB;
    case Qt::Key_Backspace:
        return VK_BACK;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return VK_RETURN;
    case Qt::Key_Insert:
        return VK_INSERT;
    case Qt::Key_Delete:
        return VK_DELETE;
    case Qt::Key_Pause:
        return VK_PAUSE;
    case Qt::Key_Print:
        return VK_PRINT;
    case Qt::Key_Clear:
        return VK_CLEAR;
    case Qt::Key_Home:
        return VK_HOME;
    case Qt::Key_End:
        return VK_END;
    case Qt::Key_Left:
        return VK_LEFT;
    case Qt::Key_Up:
        return VK_UP;
    case Qt::Key_Right:
        return VK_RIGHT;
    case Qt::Key_Down:
        return VK_DOWN;
    case Qt::Key_PageUp:
        return VK_PRIOR;
    case Qt::Key_PageDown:
        return VK_NEXT;
    case Qt::Key_F1:
        return VK_F1;
    case Qt::Key_F2:
        return VK_F2;
    case Qt::Key_F3:
        return VK_F3;
    case Qt::Key_F4:
        return VK_F4;
    case Qt::Key_F5:
        return VK_F5;
    case Qt::Key_F6:
        return VK_F6;
    case Qt::Key_F7:
        return VK_F7;
    case Qt::Key_F8:
        return VK_F8;
    case Qt::Key_F9:
        return VK_F9;
    case Qt::Key_F10:
        return VK_F10;
    case Qt::Key_F11:
        return VK_F11;
    case Qt::Key_F12:
        return VK_F12;
    case Qt::Key_F13:
        return VK_F13;
    case Qt::Key_F14:
        return VK_F14;
    case Qt::Key_F15:
        return VK_F15;
    case Qt::Key_F16:
        return VK_F16;
    case Qt::Key_F17:
        return VK_F17;
    case Qt::Key_F18:
        return VK_F18;
    case Qt::Key_F19:
        return VK_F19;
    case Qt::Key_F20:
        return VK_F20;
    case Qt::Key_F21:
        return VK_F21;
    case Qt::Key_F22:
        return VK_F22;
    case Qt::Key_F23:
        return VK_F23;
    case Qt::Key_F24:
        return VK_F24;
    case Qt::Key_Space:
        return VK_SPACE;
    case Qt::Key_Asterisk:
        return VK_MULTIPLY;
    case Qt::Key_Plus:
        return VK_ADD;
    case Qt::Key_Minus:
        return VK_SUBTRACT;
    case Qt::Key_Slash:
        return VK_DIVIDE;
    case Qt::Key_MediaNext:
        return VK_MEDIA_NEXT_TRACK;
    case Qt::Key_MediaPrevious:
        return VK_MEDIA_PREV_TRACK;
    case Qt::Key_MediaPlay:
        return VK_MEDIA_PLAY_PAUSE;
    case Qt::Key_MediaStop:
        return VK_MEDIA_STOP;
    case Qt::Key_VolumeDown:
        return VK_VOLUME_DOWN;
    case Qt::Key_VolumeUp:
        return VK_VOLUME_UP;
    case Qt::Key_VolumeMute:
        return VK_VOLUME_MUTE;

    default:
        // Try to get virtual key from current keyboard layout or US.
        const HKL layout = GetKeyboardLayout(0);
        SHORT vk = VkKeyScanExW(static_cast<WCHAR>(key), layout);
        if (vk == -1) {
            const HKL layoutUs = GetKeyboardLayout(0x409);
            vk = VkKeyScanExW(static_cast<WCHAR>(key), layoutUs);
        }
        return vk == -1 ? 0 : static_cast<quint32>(vk);
    }
}

bool SystemWideHotkey::nativeEventFilter(const QByteArray & eventType,
                                         void * message, long * result)
{
    Q_UNUSED(result);
    Q_UNUSED(eventType);

    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        const unsigned int keycode = static_cast<unsigned int>(msg->wParam);

        if(keycode == currentKeycode)
        {
            emit hotkeyPressed();
        }
    }
    return false;
}

#endif
