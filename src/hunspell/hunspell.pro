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

INCLUDEPATH += "../hunspell-1.3.3/src/hunspell/"

SOURCES += \
    ../hunspell-1.3.3/src/hunspell/affentry.cxx \
    ../hunspell-1.3.3/src/hunspell/affixmgr.cxx \
    ../hunspell-1.3.3/src/hunspell/csutil.cxx \
    ../hunspell-1.3.3/src/hunspell/dictmgr.cxx \
    ../hunspell-1.3.3/src/hunspell/filemgr.cxx \
    ../hunspell-1.3.3/src/hunspell/hashmgr.cxx \
    ../hunspell-1.3.3/src/hunspell/hunspell.cxx \
    ../hunspell-1.3.3/src/hunspell/hunzip.cxx \
    ../hunspell-1.3.3/src/hunspell/phonet.cxx \
    ../hunspell-1.3.3/src/hunspell/replist.cxx \
    ../hunspell-1.3.3/src/hunspell/suggestmgr.cxx \
    ../hunspell-1.3.3/src/hunspell/utf_info.cxx

HEADERS += \
    ../hunspell-1.3.3/src/hunspell/affentry.hxx \
    ../hunspell-1.3.3/src/hunspell/affixmgr.hxx \
    ../hunspell-1.3.3/src/hunspell/atypes.hxx \
    ../hunspell-1.3.3/src/hunspell/baseaffix.hxx \
    ../hunspell-1.3.3/src/hunspell/csutil.hxx \
    ../hunspell-1.3.3/src/hunspell/dictmgr.hxx \
    ../hunspell-1.3.3/src/hunspell/filemgr.hxx \
    ../hunspell-1.3.3/src/hunspell/hashmgr.hxx \
    ../hunspell-1.3.3/src/hunspell/htypes.hxx \
    ../hunspell-1.3.3/src/hunspell/hunspell.h \
    ../hunspell-1.3.3/src/hunspell/hunspell.hxx \
    ../hunspell-1.3.3/src/hunspell/hunvisapi.h \
    ../hunspell-1.3.3/src/hunspell/hunzip.hxx \
    ../hunspell-1.3.3/src/hunspell/langnum.hxx \
    ../hunspell-1.3.3/src/hunspell/phonet.hxx \
    ../hunspell-1.3.3/src/hunspell/replist.hxx \
    ../hunspell-1.3.3/src/hunspell/suggestmgr.hxx \
    ../hunspell-1.3.3/src/hunspell/w_char.hxx \
    ../hunspell-1.3.3/src/win_api/config.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
