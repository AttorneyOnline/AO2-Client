#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T01:14:46
#
#-------------------------------------------------

QT       += core gui multimedia network uitools
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_ICONS = logo.ico

TARGET = Attorney_Online
TEMPLATE = app

VERSION = 2.6.0.0

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
    aolineedit.cpp \
    aotextedit.cpp \
    aoevidencedisplay.cpp \
    discord_rich_presence.cpp \
    aooptionsdialog.cpp \
    chatlogpiece.cpp \
    aocaseannouncerdialog.cpp \
    aouiloader.cpp \
    aoviewport.cpp \
    aoiclog.cpp \
    aoserverchat.cpp \
    aojukebox.cpp

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
    aolineedit.h \
    aotextedit.h \
    aoevidencedisplay.h \
    discord_rich_presence.h \
    discord-rpc.h \
    aooptionsdialog.h \
    text_file_functions.h \
    chatlogpiece.h \
    aocaseannouncerdialog.h \
    aouiloader.h \
    aoviewport.h \
    aoiclog.h \
    aoserverchat.h \
    aojukebox.h

# 1. You need to get BASS and put the x86 bass DLL/headers in the project root folder
#    AND the compilation output folder. If you are compiling statically, you'll probably
#    need the .lib file too. MinGW-GCC is really finicky finding BASS, it seems. However,
#    even with the .lib file, you still need the DLL in the final output.
# 2. You need to compile the Discord Rich Presence SDK separately and add the lib/headers
#    in the same way as BASS. Discord RPC uses CMake, which does not play nicely with
#    QMake, so this step must be manual. If you are compiling dynamically, it's fine to
#    use the prebuilt libraries.
# 3. You also need to build QtApng (https://github.com/Skycoder42/QtApng).
#    Optionally, you may install it in /usr/share/qt5/plugins/imageformats, but if you do
#    so, then you must patch qapng.pri, qapngd.pri, png.pri, pngd.pri, z.pri, and zd.pri
#    such that they no longer point to the builds in the original project directory
#    (by removing those respective entries in QMAKE_PRL_LIBS and replacing them with
#    something like `-L$$[QT_INSTALL_LIBS] -lpng -lz`).
#
# Naturally, the build process becomes significantly less convoluted if you simply
# compile dynamically. If your primary distribution method is via the launcher, then
# a simple dynamic compilation is recommended.
unix:LIBS += -L$$PWD -lbass -ldiscord-rpc
win32:LIBS += -L$$PWD "$$PWD/bass.dll" -ldiscord-rpc
INCLUDEPATH += $$PWD/include

CONFIG += c++11
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os -ffunction-sections -fdata-sections -fvisibility=hidden

RESOURCES += \
    resources.qrc

DISTFILES +=

FORMS += \
    resource/ui/lobby.ui \
    resource/ui/courtroom.ui \
    resource/ui/viewport.ui \
    resource/ui/ic_log.ui \
    resource/ui/ooc.ui \
    resource/ui/jukebox.ui
