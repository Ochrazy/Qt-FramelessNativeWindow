QT += core gui widgets

TARGET = ExampleApplication
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

include("../MachineClicker/MachineClicker.pri")
include("../FramelessWindowConverter/FramelessWindowConverter.pri")
include("../TranslucentBlurEffect/TranslucentBlurEffect.pri")

SOURCES += main.cpp \
    ExampleApplication.cpp \
    MinimalScrollBar.cpp \
    ToggleButton.cpp \
    WindowButtons.cpp \
    SettingWidget.cpp \
    ControlHLabel.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ExampleApplication.h \
    MinimalScrollBar.h \
    ToggleButton.h \
    WindowButtons.h \
    SettingWidget.h \
    ControlHLabel.h

RESOURCES += $$PWD/../FramelessWindowImages.qrc
