#-------------------------------------------------
#
# Project created by QtCreator 2019-09-05T02:07:01
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = IInterval
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ClassicInterval.cpp

HEADERS += \
    IInterval.h \
    error.h \
    SHARED_EXPORT.h \
    ILog.h

LIBS += "..\IInterval-build-desktop-Qt_4_7_4__mingw_________\debug\ILog.dll"
