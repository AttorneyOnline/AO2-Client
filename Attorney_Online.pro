QT += core gui widgets network

TARGET = Attorney_Online
TEMPLATE = app

VERSION = 2.8.3.1

INCLUDEPATH += $$PWD/include
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build
MOC_DIR = $$PWD/build

SOURCES += $$files($$PWD/src/*.cpp)
HEADERS += $$files($$PWD/include/*.h)

LIBS += -L$$PWD/lib

# Uncomment to enable Discord Rich Presence
# DEFINES += DISCORD

contains(DEFINES, DISCORD) {
  LIBS += -ldiscord-rpc
}

# Uncomment to enable the BASS audio engine
# (Recommended for Windows)
# DEFINES += BASSAUDIO

contains(DEFINES, BASSAUDIO) {
  LIBS += -lbass
  LIBS += -lbassopus
}

# Uncomment to enable the Qt audio engine
# (Recommended for non-Windows platforms)
# DEFINES += QTAUDIO

contains(DEFINES, QTAUDIO) {
  QT += multimedia
}

macx:LIBS += -framework CoreFoundation -framework Foundation -framework CoreServices


CONFIG += c++14

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
