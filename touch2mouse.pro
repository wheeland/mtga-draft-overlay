QT += core gui widgets network qml quick

CONFIG += c++14 console

LIBS += -luser32 -lgdi32

SOURCES += \
    cardlayout.cpp \
    cardstatistics.cpp \
    draftmodel.cpp \
    filter.cpp \
    logparser.cpp \
    main.cpp \
    mainwindow.cpp \
    mousetracker.cpp

HEADERS += \
    cardlayout.h \
    cardstatistics.h \
    draftmodel.h \
    filter.h \
    logparser.h \
    mainwindow.h \
    mousetracker.h

RESOURCES += \
    ui.qrc
