QT += testlib core

CONFIG += qt console warn_on depend_includepath testcase no_testcase_installs
CONFIG -= app_bundle

unix: CONFIG += c++1z
win32: CONFIG += c++2a

DESTDIR = $$PWD/../bin_tests

INCLUDEPATH += $$PWD/../Attorney_Online/include
INCLUDEPATH += $$PWD/../Attorney_Online/build
OBJECTS_DIR = $$PWD/../Attorney_Online/build
MOC_DIR = $$PWD/../Attorney_Online/build
