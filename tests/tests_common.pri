QT += testlib core

CONFIG += qt console warn_on depend_includepath testcase no_testcase_installs
CONFIG -= app_bundle

CONFIG += c++17

DESTDIR = $$PWD/../bin_tests

INCLUDEPATH += $$PWD/../attorney_online/include
INCLUDEPATH += $$PWD/../attorney_online/build
OBJECTS_DIR = $$PWD/../attorney_online/build
MOC_DIR = $$PWD/../attorney_online/build
