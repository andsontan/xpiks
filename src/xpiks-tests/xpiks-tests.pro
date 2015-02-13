#-------------------------------------------------
#
# Project created by QtCreator 2015-02-12T22:20:15
#
#-------------------------------------------------

QT       += core testlib

QT       -= gui

TARGET = xpiks-tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#INCLUDEPATH += ../tiny-aes
#INCLUDEPATH += ../xpiks-qt/Encoding

SOURCES += main.cpp \
    encryption_tests.cpp \
    ../tiny-aes/aes.cpp

HEADERS += \
    encryption_tests.h \
    ../tiny-aes/aes.h \
    ../xpiks-qt/Encoding/aes-qt.h

QMAKE_CXXFLAGS += -stdlib=libc++
