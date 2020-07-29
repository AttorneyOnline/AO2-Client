#!/bin/sh

# This script prepares the compiled bundle for shipping as a standalone release
# Assumes the Qt bin folder is in PATH
# Should be used on a "Release" build from QT creator
# Note that this DOES NOT add the base/ folder

# Exit on errors and unset variables
set -eu

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/"

cd ${ROOT_DIR}

# This thing basically does all the work
/usr/local/opt/qt/bin/macdeployqt ../bin/Attorney_Online.app

# Need to add the dependencies
cp ../lib/* ../bin/Attorney_Online.app/Contents/Frameworks

# libbass has a funny path for some reason, just use rpath
install_name_tool -change @loader_path/libbass.dylib @rpath/libbass.dylib ../bin/Attorney_Online.app/Contents/MacOS/Attorney_Online
install_name_tool -change @loader_path/libbassopus.dylib @rpath/libbassopus.dylib ../bin/Attorney_Online.app/Contents/MacOS/Attorney_Online

zip -r -9 ../bin/Attorney_Online_macOS.zip ../bin/