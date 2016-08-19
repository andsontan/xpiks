TARGET = z            # The name of the output library - can be whatever you want
TEMPLATE = lib        # Tells Qt that we are compiling a library so the output will be bundled into a .a or .so file
# CONFIG += staticlib   # Tells Qt that we want a static library, so a .a file. Remove this and you will get a .so file

CONFIG += c++11

DEFINES += ZLIB_WINAPI \
           ZLIB_DLL

QMAKE_CFLAGS_WARN_ON -= -Wall   # Optional - disable warnings when compiling this library
QMAKE_CXXFLAGS_WARN_ON -= -Wall # Optional - disable warnings when compiling this library

HEADERS += \
    ../zlib-1.2.8/crc32.h \
    ../zlib-1.2.8/deflate.h \
    ../zlib-1.2.8/gzguts.h \
    ../zlib-1.2.8/inffast.h \
    ../zlib-1.2.8/inffixed.h \
    ../zlib-1.2.8/inflate.h \
    ../zlib-1.2.8/inftrees.h \
    ../zlib-1.2.8/trees.h \
    ../zlib-1.2.8/zconf.h \
    ../zlib-1.2.8/zlib.h \
    ../zlib-1.2.8/zutil.h

SOURCES += \
    ../zlib-1.2.8/adler32.c \
    ../zlib-1.2.8/compress.c \
    ../zlib-1.2.8/crc32.c \
    ../zlib-1.2.8/deflate.c \
    ../zlib-1.2.8/gzclose.c \
    ../zlib-1.2.8/gzlib.c \
    ../zlib-1.2.8/gzread.c \
    ../zlib-1.2.8/gzwrite.c \
    ../zlib-1.2.8/infback.c \
    ../zlib-1.2.8/inffast.c \
    ../zlib-1.2.8/inflate.c \
    ../zlib-1.2.8/inftrees.c \
    ../zlib-1.2.8/trees.c \
    ../zlib-1.2.8/uncompr.c \
    ../zlib-1.2.8/zutil.c
