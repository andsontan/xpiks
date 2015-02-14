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

win32:INCLUDEPATH += ../tiny-aes
win32:INCLUDEPATH += ../xpiks-qt/Encryption


TEMPLATE = app

SOURCES += main.cpp \
    encryption_tests.cpp \
    ../tiny-aes/aes.cpp \
    indicestoranges_tests.cpp \
    ../xpiks-qt/Helpers/indiceshelper.cpp

HEADERS += \
    encryption_tests.h \
    ../tiny-aes/aes.h \
    ../xpiks-qt/Encryption/aes-qt.h \
    indicestoranges_tests.h \
    ../xpiks-qt/Helpers/indiceshelper.h
