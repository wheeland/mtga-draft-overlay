QT += core gui widgets network qml quick

CONFIG += c++14 console

LIBS += -luser32 -lgdi32

SOURCES += \
    carddata.cpp \
    cardlayout.cpp \
    cardstatistics.cpp \
    draftmodel.cpp \
    filter.cpp \
    inputtracker.cpp \
    logparser.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    carddata.h \
    cardlayout.h \
    cardstatistics.h \
    draftmodel.h \
    filter.h \
    inputtracker.h \
    logparser.h \
    mainwindow.h

RESOURCES += \
    ui.qrc
