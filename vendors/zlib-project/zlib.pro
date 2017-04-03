TARGET = z            # The name of the output library - can be whatever you want
TEMPLATE = lib        # Tells Qt that we are compiling a library so the output will be bundled into a .a or .so file
# CONFIG += staticlib   # Tells Qt that we want a static library, so a .a file. Remove this and you will get a .so file

CONFIG += c++11

DEFINES += ZLIB_WINAPI \
           ZLIB_DLL

QMAKE_CFLAGS_WARN_ON -= -Wall   # Optional - disable warnings when compiling this library
QMAKE_CXXFLAGS_WARN_ON -= -Wall # Optional - disable warnings when compiling this library

HEADERS += \
    ../zlib-1.2.11/crc32.h \
    ../zlib-1.2.11/deflate.h \
    ../zlib-1.2.11/gzguts.h \
    ../zlib-1.2.11/inffast.h \
    ../zlib-1.2.11/inffixed.h \
    ../zlib-1.2.11/inflate.h \
    ../zlib-1.2.11/inftrees.h \
    ../zlib-1.2.11/trees.h \
    ../zlib-1.2.11/zconf.h \
    ../zlib-1.2.11/zlib.h \
    ../zlib-1.2.11/zutil.h

SOURCES += \
    ../zlib-1.2.11/adler32.c \
    ../zlib-1.2.11/compress.c \
    ../zlib-1.2.11/crc32.c \
    ../zlib-1.2.11/deflate.c \
    ../zlib-1.2.11/gzclose.c \
    ../zlib-1.2.11/gzlib.c \
    ../zlib-1.2.11/gzread.c \
    ../zlib-1.2.11/gzwrite.c \
    ../zlib-1.2.11/infback.c \
    ../zlib-1.2.11/inffast.c \
    ../zlib-1.2.11/inflate.c \
    ../zlib-1.2.11/inftrees.c \
    ../zlib-1.2.11/trees.c \
    ../zlib-1.2.11/uncompr.c \
    ../zlib-1.2.11/zutil.c
