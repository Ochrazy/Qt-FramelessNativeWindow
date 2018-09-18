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
        MachineClicker.cpp \
    InputSimulation_linux.cpp \
    InputSimulation_windows.cpp \
    SystemWideHotkey_linux.cpp \
    SystemWideHotkey.cpp \
    SystemWideHotkey_windows.cpp \
    Clicker.cpp \
    FramelessWindowConverter.cpp \
    FramelessWindowConverterWindows.cpp \
    FramelessWindowConverterLinux.cpp \
    FramelessWindowConverter_p.cpp \
    NativeWindowWindows.cpp \
    Clicker.cpp \
    FramelessWindowConverter.cpp \
    FramelessWindowConverter_p.cpp \
    FramelessWindowConverterLinux.cpp \
    FramelessWindowConverterWindows.cpp \
    InputSimulation_linux.cpp \
    InputSimulation_macos.cpp \
    InputSimulation_windows.cpp \
    MachineClicker.cpp \
    main.cpp \
    NativeWindowWindows.cpp \
    SystemWideHotkey.cpp \
    SystemWideHotkey_linux.cpp \
    SystemWideHotkey_windows.cpp

OBJECTIVE_SOURCES += SystemWideHotkey_macos.mm \
    InputSimulation_macos.mm \
    Clicker_macos.mm \
    FramelessWindowConverterMacos.mm

HEADERS += \
    InputSimulation.h \
    SystemWideHotkey.h \
    Clicker.h \
    FramelessWindowConverter.h \
    FramelessWindowConverter_p.h \
    FramelessWindowConverterWindows.h \
    FramelessWindowConverterLinux.h \
    MachineClicker.h \
    FramelessWindowConverterLinux.h \
    NativeWindowWindows.h \
    FramelessWindowConverterMacos.h \
    Clicker.h \
    FramelessWindowConverter.h \
    FramelessWindowConverter_p.h \
    FramelessWindowConverterLinux.h \
    FramelessWindowConverterMacos.h \
    FramelessWindowConverterWindows.h \
    InputSimulation.h \
    MachineClicker.h \
    NativeWindowWindows.h \
    SystemWideHotkey.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    FramelessWindowImages.qrc
