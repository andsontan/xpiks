#-------------------------------------------------
#
# Project created by QtCreator 2015-11-15T16:40:11
#
#-------------------------------------------------

QT       -= core gui

TARGET = hunspell
TEMPLATE = lib
CONFIG += staticlib
DEFINES += HUNSPELL_STATIC

CONFIG += c++11

INCLUDEPATH += "../hunspell-1.6.0/src/hunspell/"
INCLUDEPATH += "../hunspell-1.6.0/src/win_api/"

SOURCES += \
    ../hunspell-1.6.0/src/hunspell/affentry.cxx \
    ../hunspell-1.6.0/src/hunspell/affixmgr.cxx \
    ../hunspell-1.6.0/src/hunspell/csutil.cxx \
    ../hunspell-1.6.0/src/hunspell/filemgr.cxx \
    ../hunspell-1.6.0/src/hunspell/hashmgr.cxx \
    ../hunspell-1.6.0/src/hunspell/hunspell.cxx \
    ../hunspell-1.6.0/src/hunspell/hunzip.cxx \
    ../hunspell-1.6.0/src/hunspell/phonet.cxx \
    ../hunspell-1.6.0/src/hunspell/replist.cxx \
    ../hunspell-1.6.0/src/hunspell/suggestmgr.cxx \
    ../hunspell-1.6.0/src/hunspell/utf_info.cxx

HEADERS += \
    ../hunspell-1.6.0/src/hunspell/affentry.hxx \
    ../hunspell-1.6.0/src/hunspell/affixmgr.hxx \
    ../hunspell-1.6.0/src/hunspell/atypes.hxx \
    ../hunspell-1.6.0/src/hunspell/baseaffix.hxx \
    ../hunspell-1.6.0/src/hunspell/csutil.hxx \
    ../hunspell-1.6.0/src/hunspell/dictmgr.hxx \
    ../hunspell-1.6.0/src/hunspell/filemgr.hxx \
    ../hunspell-1.6.0/src/hunspell/hashmgr.hxx \
    ../hunspell-1.6.0/src/hunspell/htypes.hxx \
    ../hunspell-1.6.0/src/hunspell/hunspell.h \
    ../hunspell-1.6.0/src/hunspell/hunspell.hxx \
    ../hunspell-1.6.0/src/hunspell/hunvisapi.h \
    ../hunspell-1.6.0/src/hunspell/hunzip.hxx \
    ../hunspell-1.6.0/src/hunspell/langnum.hxx \
    ../hunspell-1.6.0/src/hunspell/phonet.hxx \
    ../hunspell-1.6.0/src/hunspell/replist.hxx \
    ../hunspell-1.6.0/src/hunspell/suggestmgr.hxx \
    ../hunspell-1.6.0/src/hunspell/w_char.hxx \
    ../hunspell-1.6.0/src/win_api/config.h \
    ../hunspell-1.6.0/src/hunspell/hunvisapi.h.in
unix {
    target.path = /usr/lib
    INSTALLS += target
}

DISTFILES += \
    ../hunspell-1.6.0/src/hunspell/Makefile.am
