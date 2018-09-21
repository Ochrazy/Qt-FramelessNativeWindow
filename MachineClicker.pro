#-------------------------------------------------
#
# Project created by QtCreator 2018-07-29T15:29:55
#
#-------------------------------------------------

QT       += core gui widgets

unix:!macx:QT += x11extras

TARGET = MachineClicker
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

win32:LIBS += -luser32 "dwmapi.lib" "uxtheme.lib" gdi32.lib

macx:LIBS += -framework ApplicationServices -framework Carbon -framework AppKit

unix:!macx:LIBS += -lX11 -lxcb

SOURCES += \
        main.cpp \
    FramelessWindowConverter/FramelessWindowConverter.cpp \
    FramelessWindowConverter/FramelessWindowConverter_p.cpp \
    FramelessWindowConverter/FramelessWindowConverterLinux.cpp \
    FramelessWindowConverter/FramelessWindowConverterWindows.cpp \
    InputSimulation/InputSimulation_linux.cpp \
    InputSimulation/InputSimulation_windows.cpp \
    SystemWideHotkey/SystemWideHotkey.cpp \
    SystemWideHotkey/SystemWideHotkey_linux.cpp \
    SystemWideHotkey/SystemWideHotkey_windows.cpp \
    MachineClicker/Clicker.cpp \
    MachineClicker/MachineClicker.cpp \

OBJECTIVE_SOURCES += SystemWideHotkey/SystemWideHotkey_macos.mm \
    InputSimulation/InputSimulation_macos.mm \
    MachineClicker/Clicker_macos.mm
    FramelessWindowConverter/FramelessWindowConverterMacos.mm

HEADERS += \
    FramelessWindowConverter/FramelessWindowConverter.h \
    FramelessWindowConverter/FramelessWindowConverter_p.h \
    FramelessWindowConverter/FramelessWindowConverterLinux.h \
    FramelessWindowConverter/FramelessWindowConverterMacos.h \
    FramelessWindowConverter/FramelessWindowConverterWindows.h \
    InputSimulation/InputSimulation.h \
    SystemWideHotkey/SystemWideHotkey.h \
    MachineClicker/Clicker.h \
    MachineClicker/MachineClicker.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    FramelessWindowImages.qrc
