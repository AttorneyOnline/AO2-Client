#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T01:14:46
#
#-------------------------------------------------

QT       += core gui multimedia network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Attorney_Online
TEMPLATE = app

VERSION = 2.6.0.0

INCLUDEPATH += $$PWD/include
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build
MOC_DIR = $$PWD/build

SOURCES += $$files($$PWD/src/*.cpp)
HEADERS += $$files($$PWD/include/*.h)
LIBS += -L$$PWD/lib -lbass -ldiscord-rpc

CONFIG += c++11

RESOURCES += resources.qrc

win32:RC_ICONS = resource/logo.ico
