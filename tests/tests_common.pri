QT += testlib core

CONFIG += qt console warn_on depend_includepath testcase no_testcase_installs
CONFIG -= app_bundle

DESTDIR = $$PWD/../bin_tests

INCLUDEPATH += $$PWD/../Attorney_Online/include
