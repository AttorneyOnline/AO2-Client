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

DEFINES += DISCORD

contains(DEFINES, DISCORD) {
LIBS += -ldiscord-rpc
}

DEFINES += BASSAUDIO

contains(DEFINES, BASSAUDIO) {
LIBS += -lbass
LIBS += -lbassopus
}

#DEFINES += QTAUDIO

contains(DEFINES, QTAUDIO) {
QT += multimedia
}

macx:LIBS += -framework CoreFoundation -framework Foundation -framework CoreServices


CONFIG += c++14

RESOURCES += resources.qrc

win32:RC_ICONS = resource/logo.ico
macx:ICON = resource/logo.icns

android:DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
