#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T01:14:46
#
#-------------------------------------------------

QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = logo.ico

TARGET = Attorney_Online_CC
TEMPLATE = app

VERSION = 2.4.10.0

SOURCES += main.cpp\
        lobby.cpp \
    text_file_functions.cpp \
    path_functions.cpp \
    aoimage.cpp \
    file_functions.cpp \
    aobutton.cpp \
    debug_functions.cpp \
    networkmanager.cpp \
    aoapplication.cpp \
    aopacket.cpp \
    packet_distribution.cpp \
    hex_functions.cpp \
    encryption_functions.cpp \
    courtroom.cpp \
    aocharbutton.cpp \
    hardware_functions.cpp \
    aoscene.cpp \
    aomovie.cpp \
    misc_functions.cpp \
    aocharmovie.cpp \
    aoemotebutton.cpp \
    emotes.cpp \
    aosfxplayer.cpp \
    aomusicplayer.cpp \
    aoblipplayer.cpp \
    evidence.cpp \
    aoevidencebutton.cpp \
    charselect.cpp \
    aotextarea.cpp \
    aolineedit.cpp \
    aotextedit.cpp \
    aoevidencedisplay.cpp \
    discord_rich_presence.cpp \
    aooptionsdialog.cpp

HEADERS  += lobby.h \
    aoimage.h \
    file_functions.h \
    aobutton.h \
    debug_functions.h \
    networkmanager.h \
    aoapplication.h \
    datatypes.h \
    aopacket.h \
    hex_functions.h \
    encryption_functions.h \
    courtroom.h \
    aocharbutton.h \
    hardware_functions.h \
    aoscene.h \
    aomovie.h \
    misc_functions.h \
    aocharmovie.h \
    aoemotebutton.h \
    aosfxplayer.h \
    aomusicplayer.h \
    aoblipplayer.h \
    aoevidencebutton.h \
    aotextarea.h \
    aolineedit.h \
    aotextedit.h \
    aoevidencedisplay.h \
    discord_rich_presence.h \
    discord-rpc.h \
    aooptionsdialog.h

#    You need to compile the Discord Rich Presence SDK separately and add the lib/headers.
#    Discord RPC uses CMake, which does not play nicely with QMake, so this step must be manual.
unix:LIBS += -L$$PWD -ldiscord-rpc
win32:LIBS += -L$$PWD -ldiscord-rpc #"$$PWD/discord-rpc.dll"

CONFIG += c++11

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
    resources.qrc

DISTFILES +=
