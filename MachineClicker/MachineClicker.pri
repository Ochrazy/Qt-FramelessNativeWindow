QT += core gui widgets
unix:!macx:QT += x11extras

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

INCLUDEPATH += $$PWD/..

SOURCES += \
    $$PWD/InputSimulation/InputSimulation_linux.cpp \
    $$PWD/InputSimulation/InputSimulation_windows.cpp \
    $$PWD/SystemWideHotkey/SystemWideHotkey.cpp \
    $$PWD/SystemWideHotkey/SystemWideHotkey_linux.cpp \
    $$PWD/SystemWideHotkey/SystemWideHotkey_windows.cpp \
    $$PWD/Clicker.cpp \
    $$PWD/MachineClicker.cpp

OBJECTIVE_SOURCES += $$PWD/SystemWideHotkey/SystemWideHotkey_macos.mm \
    $$PWD/InputSimulation/InputSimulation_macos.mm \
    $$PWD/Clicker_macos.mm

HEADERS += \
    $$PWD/InputSimulation/InputSimulation.h \
    $$PWD/SystemWideHotkey/SystemWideHotkey.h \
    $$PWD/Clicker.h \
    $$PWD/MachineClicker.h

RESOURCES += $$PWD/../FramelessWindowImages.qrc
