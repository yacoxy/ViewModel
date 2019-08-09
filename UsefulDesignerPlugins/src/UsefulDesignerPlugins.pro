CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(UsefulDesignerPlugins)
TEMPLATE    = lib

DESTDIR = ../bin

RESOURCES   = icons.qrc
LIBS        += -L. 

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

include(ShapeSelGroupBox.pri)

HEADERS += \
    ShapeSelGroupBoxPlugin.h

SOURCES += \
    ShapeSelGroupBoxPlugin.cpp

