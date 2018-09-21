QT += core gui widgets
unix:!macx:QT += x11extras

TARGET = MachineClicker
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

include("../FramelessWindowConverter/FramelessWindowConverter.pri")

SOURCES += \
        main.cpp \
    InputSimulation/InputSimulation_linux.cpp \
    InputSimulation/InputSimulation_windows.cpp \
    SystemWideHotkey/SystemWideHotkey.cpp \
    SystemWideHotkey/SystemWideHotkey_linux.cpp \
    SystemWideHotkey/SystemWideHotkey_windows.cpp \
    Clicker.cpp \
    MachineClicker.cpp \

OBJECTIVE_SOURCES += SystemWideHotkey/SystemWideHotkey_macos.mm \
    InputSimulation/InputSimulation_macos.mm \
    Clicker_macos.mm

HEADERS += \
    InputSimulation/InputSimulation.h \
    SystemWideHotkey/SystemWideHotkey.h \
    Clicker.h \
    MachineClicker.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += ../FramelessWindowImages.qrc
