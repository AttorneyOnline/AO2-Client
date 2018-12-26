#!/bin/bash

#this script relinks dynamic libraries so the .app file can be distributed as standalone

install_name_tool -id @executable_path/../Frameworks/libbass.dylib ../bin/Attorney_Online.app/Contents/Frameworks/libbass.dylib

install_name_tool -id @executable_path/../Frameworks/libbassopus.dylib ../bin/Attorney_Online.app/Contents/Frameworks/libbassopus.dylib

install_name_tool -id @executable_path/../Frameworks/libdiscord-rpc.dylib ../bin/Attorney_Online.app/Contents/Frameworks/libdiscord-rpc.dylib

install_name_tool -id @executable_path/../Frameworks/libqapng.dylib ../bin/Attorney_Online.app/Contents/Frameworks/libqapng.dylib

install_name_tool -change @loader_path/libbass.dylib @executable_path/../Frameworks/libbass.dylib ../bin/Attorney_Online.app/Contents/MacOS/Attorney_Online

install_name_tool -change @rpath/libdiscord-rpc.dylib @executable_path/../Frameworks/libdiscord-rpc.dylib ../bin/Attorney_Online.app/Contents/MacOS/Attorney_Online

