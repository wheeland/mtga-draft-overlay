QT += core gui widgets

CONFIG += c++14 console

LIBS += -luser32 -lgdi32

SOURCES += \
    filter.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    filter.h \
    mainwindow.h
