QT += core gui widgets network websockets uitools

TARGET = kenji
TEMPLATE = app

VERSION = 0.1.0.0

INCLUDEPATH += $$PWD/include
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build
MOC_DIR = $$PWD/build

SOURCES += $$files($$PWD/src/*.cpp, true)
HEADERS += $$files($$PWD/include/*.h, true)

FORMS += $$files($$PWD/resource/ui/*.ui)

LIBS += -L$$PWD/lib
QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN/lib'"

# Uncomment for verbose network logging
# DEFINES += DEBUG_NETWORK

# Uncomment for verbose animation logging
# DEFINES += DEBUG_MOVIE

# Uncomment for transition debug features
# DEFINES += DEBUG_TRANSITIONS

# Uncomment for building with debug symbols
# CONFIG += debug

# Uncomment to enable Discord Rich Presence
# DEFINES += DISCORD

contains(DEFINES, DISCORD) {
  win32:LIBS            += -ldiscord-rpc
  linux:!android:LIBS   += -ldiscord-rpc
  mac:LIBS              += -ldiscord-rpc
}

# As of Attorney Online 2.8.5, BASS and BASSOPUS are required for all platforms. Qt Multimedia
# is no longer an option due to outdated code and lack of versatility.
# Download at un4seen.com and place the DLLs in the "lib" and "bin" folders.
DEFINES += BASSAUDIO
LIBS += -lbass
LIBS += -lbassopus
LIBS += -lbassmidi

macx:LIBS += -framework CoreFoundation -framework Foundation -framework CoreServices

win32:LIBS += -ladvapi32

CONFIG += c++17

RESOURCES += resources.qrc

TRANSLATIONS = resource/translations/ao_en.ts \
               resource/translations/ao_jp.ts \
               resource/translations/ao_de.ts \
               resource/translations/ao_ru.ts \
               resource/translations/ao_es.ts \
               resource/translations/ao_pt.ts \
               resource/translations/ao_pl.ts \
               resource/translations/ao_it.ts \
               ressource/ui/

win32:RC_ICONS = resource/logo_kenji.ico
macx:ICON =

android:QT += androidextras

android:DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle.properties \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
