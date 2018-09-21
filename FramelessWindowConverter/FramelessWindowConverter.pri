INCLUDEPATH += $$PWD/..

SOURCES += $$PWD/FramelessWindowConverter.cpp \
    $$PWD/FramelessWindowConverter_p.cpp \
    $$PWD/FramelessWindowConverterLinux.cpp \
    $$PWD/FramelessWindowConverterWindows.cpp

HEADERS += \
    $$PWD/FramelessWindowConverter.h \
    $$PWD/FramelessWindowConverter_p.h \
    $$PWD/FramelessWindowConverterLinux.h \
    $$PWD/FramelessWindowConverterMacos.h \
    $$PWD/FramelessWindowConverterWindows.h

OBJECTIVE_SOURCES +=  $$PWD/FramelessWindowConverterMacos.mm
