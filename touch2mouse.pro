QT += core gui widgets network qml quick

CONFIG += c++14 console

LIBS += -luser32 -lgdi32

SOURCES += \
    carddata.cpp \
    carddb.cpp \
    cardlayout.cpp \
    draftmodel.cpp \
    filter.cpp \
    inputtracker.cpp \
    landsdb.cpp \
    logparser.cpp \
    main.cpp \
    mainwindow.cpp \
    scryfalldb.cpp

HEADERS += \
    carddata.h \
    carddb.h \
    cardlayout.h \
    draftmodel.h \
    filter.h \
    inputtracker.h \
    landsdb.h \
    logparser.h \
    mainwindow.h \
    scryfalldb.h

RESOURCES += \
    ui.qrc
