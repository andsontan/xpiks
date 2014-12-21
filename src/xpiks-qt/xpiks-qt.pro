TEMPLATE = app

QT += qml quick widgets concurrent

SOURCES += main.cpp \
    Models/artitemsmodel.cpp \
    Models/artworkmetadata.cpp \
    Helpers/globalimageprovider.cpp \
    Models/artworksrepository.cpp \
    Models/combinedartworksmodel.cpp \
    Models/artiteminfo.cpp \
    Helpers/indiceshelper.cpp \
    Models/iptcprovider.cpp \
    Helpers/externaltoolsprovider.cpp \
    Models/artworkuploader.cpp \
    Models/artworksprocessor.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    Models/artitemsmodel.h \
    Models/artworkmetadata.h \
    Helpers/globalimageprovider.h \
    Models/artworksrepository.h \
    Helpers/indiceshelper.h \
    Helpers/clipboardhelper.h \
    Models/combinedartworksmodel.h \
    Models/artiteminfo.h \
    Models/abstractlistmodel.h \
    Helpers/constants.h \
    Helpers/appsettings.h \
    Models/iptcprovider.h \
    Helpers/externaltoolsprovider.h \
    Helpers/exiftoolwrapper.h \
    Models/artworkuploader.h \
    Helpers/curlwrapper.h \
    Models/artworksprocessor.h \
    Models/uploadinfo.h
