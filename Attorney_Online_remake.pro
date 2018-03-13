#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T01:14:46
#
#-------------------------------------------------

QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = logo.ico

TARGET = Attorney_Online_remake
TEMPLATE = app

VERSION = 2.4.8.0

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
    discord_rich_presence.cpp

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
    bass.h \
    aosfxplayer.h \
    aomusicplayer.h \
    aoblipplayer.h \
    aoevidencebutton.h \
    aotextarea.h \
    aolineedit.h \
    aotextedit.h \
    aoevidencedisplay.h \
    discord_rich_presence.h \
    discord-rpc.h

# 1. You need to get BASS and put the x86 bass DLL/headers in the project root folder
#    AND the compilation output folder. If you want a static link, you'll probably
#    need the .lib file too. MinGW-GCC is really finicky finding BASS, it seems.
# 2. You need to compile the Discord Rich Presence SDK separately and add the lib/headers
#    in the same way as BASS. Discord RPC uses CMake, which does not play nicely with
#    QMake, so this step must be manual.
unix:LIBS += -L$$PWD -lbass -ldiscord-rpc
win32:LIBS += -L$$PWD "$$PWD/bass.lib" -ldiscord-rpc #"$$PWD/discord-rpc.dll"
android:LIBS += -L$$PWD\android\libs\armeabi-v7a\ -lbass

CONFIG += c++11

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
    resources.qrc

DISTFILES +=
