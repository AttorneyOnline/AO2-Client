QT += core gui widgets network

TARGET = Attorney_Online
TEMPLATE = app

VERSION = 2.9.1.0

INCLUDEPATH += $$PWD/include
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build
MOC_DIR = $$PWD/build

SOURCES += $$files($$PWD/src/*.cpp)
HEADERS += $$files($$PWD/include/*.h)

LIBS += -L$$PWD/lib
QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN/lib'"

# Uncomment for verbose network logging
# DEFINES += DEBUG_NETWORK

# Uncomment for verbose animation logging
# DEFINES += DEBUG_MOVIE

# Uncomment for building with debug symbols
# CONFIG += debug

# Uncomment to enable Discord Rich Presence
# DEFINES += DISCORD

contains(DEFINES, DISCORD) {
  LIBS += -ldiscord-rpc
}

# As of 2.8.5, BASS and BASSOPUS are required for all platforms. Qt Multimedia
# is no longer an option due to outdated code and lack of versatility.
# Download at un4seen.com and place the DLLs in the "lib" and "bin" folders.
DEFINES += BASSAUDIO
LIBS += -lbass
LIBS += -lbassopus

macx:LIBS += -framework CoreFoundation -framework Foundation -framework CoreServices

CONFIG += c++17

RESOURCES += resources.qrc

TRANSLATIONS = resource/translations/ao_en.ts \
               resource/translations/ao_jp.ts \
               resource/translations/ao_de.ts \
               resource/translations/ao_ru.ts \
               resource/translations/ao_es.ts \
               resource/translations/ao_pt.ts \
               resource/translations/ao_pl.ts

win32:RC_ICONS = resource/logo_ao2.ico
macx:ICON = resource/logo_ao2.icns

android:DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
