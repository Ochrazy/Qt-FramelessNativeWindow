#include "SystemWideHotkey.h"

SystemWideHotkey::SystemWideHotkey()
{
    QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
    initialize();
}

SystemWideHotkey::~SystemWideHotkey()
{
    unregisterHotkey();
}

