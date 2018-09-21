#ifdef __APPLE__

#include "SystemWideHotkey.h"
#import <AppKit/AppKit.h>
#include <QMap>
#include <QHash>
#include "Carbon/Carbon.h"

using Identifier = QPair<uint, uint>;
static bool bIsHotkeyHandlerInstalled = false;
static QMap<quint32, EventHotKeyRef> keyRefs;
static QHash<Identifier, quint32> keyIDs;
static quint32 hotKeySerial = 0;

OSStatus handleHotkey(EventHandlerCallRef nextHandler, EventRef event, void* data)
{
    Q_UNUSED(nextHandler);
    SystemWideHotkey* thisHotkey = static_cast<SystemWideHotkey*>(data);

    if (GetEventClass(event) == kEventClassKeyboard && GetEventKind(event) == kEventHotKeyPressed)
    {
        emit thisHotkey->hotkeyPressed();
    }

    return noErr;
}

void SystemWideHotkey::initialize()
{
    registerNewHotkey(Qt::Key_Space);

    if(!bIsHotkeyHandlerInstalled)
    {
        EventTypeSpec t;
        t.eventClass = kEventClassKeyboard;
        t.eventKind = kEventHotKeyPressed;
        InstallApplicationEventHandler(&handleHotkey, 1, &t, this, nullptr);
        bIsHotkeyHandlerInstalled = true;
    }
}

void SystemWideHotkey::registerNewHotkey(Qt::Key key)
{
    unregisterHotkey();

    EventHotKeyID keyID;
    keyID.signature = 42;
    keyID.id = ++hotKeySerial;

    quint32 keycode = nativeKeycode(key);

    EventHotKeyRef ref = nullptr;
    bool rv = !RegisterEventHotKey(keycode, 0, keyID, GetApplicationEventTarget(), 0, &ref);

    if (rv)
    {
        keyIDs.insert(Identifier(0, keycode), keyID.id);
        keyRefs.insert(keyID.id, ref);
    }

    currentKeycode = keycode;
}

void SystemWideHotkey::unregisterHotkey()
{
    Identifier id(0, currentKeycode);
    if (!keyIDs.contains(id)) return;

    EventHotKeyRef ref = keyRefs.take(keyIDs[id]);
    keyIDs.remove(id);
    UnregisterEventHotKey(ref);
}

bool SystemWideHotkey::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    Q_UNUSED(eventType)
    Q_UNUSED(message)

    return false;
}

quint32 SystemWideHotkey::nativeKeycode(Qt::Key key)
{
    UTF16Char ch;
    // Constants found in NSEvent.h from AppKit.framework
    switch (key)
    {
    case Qt::Key_Return:
        return kVK_Return;
    case Qt::Key_Enter:
        return kVK_ANSI_KeypadEnter;
    case Qt::Key_Tab:
        return kVK_Tab;
    case Qt::Key_Space:
        return kVK_Space;
    case Qt::Key_Backspace:
        return kVK_Delete;
    case Qt::Key_Control:
        return kVK_Command;
    case Qt::Key_Shift:
        return kVK_Shift;
    case Qt::Key_CapsLock:
        return kVK_CapsLock;
    case Qt::Key_Option:
        return kVK_Option;
    case Qt::Key_Meta:
        return kVK_Control;
    case Qt::Key_F17:
        return kVK_F17;
    case Qt::Key_VolumeUp:
        return kVK_VolumeUp;
    case Qt::Key_VolumeDown:
        return kVK_VolumeDown;
    case Qt::Key_F18:
        return kVK_F18;
    case Qt::Key_F19:
        return kVK_F19;
    case Qt::Key_F20:
        return kVK_F20;
    case Qt::Key_F5:
        return kVK_F5;
    case Qt::Key_F6:
        return kVK_F6;
    case Qt::Key_F7:
        return kVK_F7;
    case Qt::Key_F3:
        return kVK_F3;
    case Qt::Key_F8:
        return kVK_F8;
    case Qt::Key_F9:
        return kVK_F9;
    case Qt::Key_F11:
        return kVK_F11;
    case Qt::Key_F13:
        return kVK_F13;
    case Qt::Key_F16:
        return kVK_F16;
    case Qt::Key_F14:
        return kVK_F14;
    case Qt::Key_F10:
        return kVK_F10;
    case Qt::Key_F12:
        return kVK_F12;
    case Qt::Key_F15:
        return kVK_F15;
    case Qt::Key_Help:
        return kVK_Help;
    case Qt::Key_Home:
        return kVK_Home;
    case Qt::Key_PageUp:
        return kVK_PageUp;
    case Qt::Key_Delete:
        return kVK_ForwardDelete;
    case Qt::Key_F4:
        return kVK_F4;
    case Qt::Key_End:
        return kVK_End;
    case Qt::Key_F2:
        return kVK_F2;
    case Qt::Key_PageDown:
        return kVK_PageDown;
    case Qt::Key_F1:
        return kVK_F1;
    case Qt::Key_Left:
        return kVK_LeftArrow;
    case Qt::Key_Right:
        return kVK_RightArrow;
    case Qt::Key_Down:
        return kVK_DownArrow;
    case Qt::Key_Up:
        return kVK_UpArrow;
    default:
        ;
    }

    if (key == Qt::Key_Escape)	ch = 27;
    else if (key == Qt::Key_Return) ch = 13;
    else if (key == Qt::Key_Enter) ch = 3;
    else if (key == Qt::Key_Tab) ch = 9;
    else ch = key;

    CFDataRef currentLayoutData;
    TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();

    if (currentKeyboard == nullptr)
        return 0;

    currentLayoutData = (CFDataRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyUnicodeKeyLayoutData);
    CFRelease(currentKeyboard);
    if (currentLayoutData == nullptr){//Japanese or Chinese always return null
        currentKeyboard = TISCopyCurrentKeyboardLayoutInputSource();
        currentLayoutData = (CFDataRef)TISGetInputSourceProperty(currentKeyboard, kTISPropertyUnicodeKeyLayoutData);
        if(!currentLayoutData)
            return 0;
    }

    UCKeyboardLayout* header = (UCKeyboardLayout*)CFDataGetBytePtr(currentLayoutData);
    UCKeyboardTypeHeader* table = header->keyboardTypeList;

    uint8_t *data = (uint8_t*)header;
    for (quint32 i=0; i < header->keyboardTypeCount; i++)
    {
        UCKeyStateRecordsIndex* stateRec = 0;
        if (table[i].keyStateRecordsIndexOffset != 0)
        {
            stateRec = reinterpret_cast<UCKeyStateRecordsIndex*>(data + table[i].keyStateRecordsIndexOffset);
            if (stateRec->keyStateRecordsIndexFormat != kUCKeyStateRecordsIndexFormat) stateRec = 0;
        }

        UCKeyToCharTableIndex* charTable = reinterpret_cast<UCKeyToCharTableIndex*>(data + table[i].keyToCharTableIndexOffset);
        if (charTable->keyToCharTableIndexFormat != kUCKeyToCharTableIndexFormat) continue;

        for (quint32 j=0; j < charTable->keyToCharTableCount; j++)
        {
            UCKeyOutput* keyToChar = reinterpret_cast<UCKeyOutput*>(data + charTable->keyToCharTableOffsets[j]);
            for (quint32 k=0; k < charTable->keyToCharTableSize; k++)
            {
                if (keyToChar[k] & kUCKeyOutputTestForIndexMask)
                {
                    long idx = keyToChar[k] & kUCKeyOutputGetIndexMask;
                    if (stateRec && idx < stateRec->keyStateRecordCount)
                    {
                        UCKeyStateRecord* rec = reinterpret_cast<UCKeyStateRecord*>(data + stateRec->keyStateRecordOffsets[idx]);
                        if (rec->stateZeroCharData == ch) return k;
                    }
                }
                else if (!(keyToChar[k] & kUCKeyOutputSequenceIndexMask) && keyToChar[k] < 0xFFFE)
                {
                    if (keyToChar[k] == ch) return k;
                }
            } // for k
        } // for j
    } // for i

    // Last resort is to try mapping the rest of the keys directly.
    switch (key)
    {
    case Qt::Key_A:
        return kVK_ANSI_A;
    case Qt::Key_S:
        return kVK_ANSI_S;
    case Qt::Key_D:
        return kVK_ANSI_D;
    case Qt::Key_F:
        return kVK_ANSI_F;
    case Qt::Key_H:
        return kVK_ANSI_H;
    case Qt::Key_G:
        return kVK_ANSI_G;
    case Qt::Key_Z:
        return kVK_ANSI_Z;
    case Qt::Key_X:
        return kVK_ANSI_X;
    case Qt::Key_C:
        return kVK_ANSI_C;
    case Qt::Key_V:
        return kVK_ANSI_V;
    case Qt::Key_B:
        return kVK_ANSI_B;
    case Qt::Key_Q:
        return kVK_ANSI_Q;
    case Qt::Key_W:
        return kVK_ANSI_W;
    case Qt::Key_E:
        return kVK_ANSI_E;
    case Qt::Key_R:
        return kVK_ANSI_R;
    case Qt::Key_Y:
        return kVK_ANSI_Y;
    case Qt::Key_T:
        return kVK_ANSI_T;
    case Qt::Key_1:
        return kVK_ANSI_1;
    case Qt::Key_2:
        return kVK_ANSI_2;
    case Qt::Key_3:
        return kVK_ANSI_3;
    case Qt::Key_4:
        return kVK_ANSI_4;
    case Qt::Key_6:
        return kVK_ANSI_6;
    case Qt::Key_5:
        return kVK_ANSI_5;
    case Qt::Key_Equal:
        return kVK_ANSI_Equal;
    case Qt::Key_9:
        return kVK_ANSI_9;
    case Qt::Key_7:
        return kVK_ANSI_7;
    case Qt::Key_Minus:
        return kVK_ANSI_Minus;
    case Qt::Key_8:
        return kVK_ANSI_8;
    case Qt::Key_0:
        return kVK_ANSI_0;
    case Qt::Key_BracketRight:
        return kVK_ANSI_RightBracket;
    case Qt::Key_O:
        return kVK_ANSI_O;
    case Qt::Key_U:
        return kVK_ANSI_U;
    case Qt::Key_BracketLeft:
        return kVK_ANSI_LeftBracket;
    case Qt::Key_I:
        return kVK_ANSI_I;
    case Qt::Key_P:
        return kVK_ANSI_P;
    case Qt::Key_L:
        return kVK_ANSI_L;
    case Qt::Key_J:
        return kVK_ANSI_J;
    case Qt::Key_QuoteDbl:
        return kVK_ANSI_Quote;
    case Qt::Key_K:
        return kVK_ANSI_K;
    case Qt::Key_Semicolon:
        return kVK_ANSI_Semicolon;
    case Qt::Key_Backslash:
        return kVK_ANSI_Backslash;
    case Qt::Key_Comma:
        return kVK_ANSI_Comma;
    case Qt::Key_Slash:
        return kVK_ANSI_Slash;
    case Qt::Key_N:
        return kVK_ANSI_N;
    case Qt::Key_M:
        return kVK_ANSI_M;
    case Qt::Key_Period:
        return kVK_ANSI_Period;
    case Qt::Key_Dead_Grave:
        return kVK_ANSI_Grave;
    case Qt::Key_Asterisk:
        return kVK_ANSI_KeypadMultiply;
    case Qt::Key_Plus:
        return kVK_ANSI_KeypadPlus;
    case Qt::Key_Clear:
        return kVK_ANSI_KeypadClear;
    case Qt::Key_Escape:
        return kVK_Escape;
    default:
        ;
    }

    return 0;
}

#endif
