QT += core gui widgets network qml quick

CONFIG += c++14 console

LIBS += -luser32 -lgdi32

SOURCES += \
    cardstatistics.cpp \
    filter.cpp \
    logparser.cpp \
    main.cpp \
    mainwindow.cpp \
    mousetracker.cpp

HEADERS += \
    cardstatistics.h \
    filter.h \
    logparser.h \
    mainwindow.h \
    mousetracker.h

RESOURCES += \
    ui.qrc
