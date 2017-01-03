#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T01:14:46
#
#-------------------------------------------------

QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Attorney_Online_remake
TEMPLATE = app

VERSION = 2.1.0.0

SOURCES += main.cpp\
        lobby.cpp \
    text_file_functions.cpp \
    path_functions.cpp \
    aoimage.cpp \
    file_functions.cpp \
    aobutton.cpp \
    global_variables.cpp \
    debug_functions.cpp \
    networkmanager.cpp \
    aoapplication.cpp

HEADERS  += lobby.h \
    text_file_functions.h \
    path_functions.h \
    aoimage.h \
    file_functions.h \
    aobutton.h \
    global_variables.h \
    debug_functions.h \
    networkmanager.h \
    aoapplication.h \
    datatypes.h
