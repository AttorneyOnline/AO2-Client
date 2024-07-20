#!/bin/bash

set -euxo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}" || { echo "Failed to cd to pwd"; exit 1; }

detect_platform() {
    unameOut="$(uname -s)"
    case "${unameOut}" in
        CYGWIN*|MINGW*|MSYS*) platform=windows;;
        Linux*)     platform=linux;;
        Darwin*)    platform=macos;;
        *)          platform=unknown;;
    esac
    echo "${platform}"
}

# Basic data such as platform can be global
PLATFORM=$(detect_platform)
BUILD_CONFIG="Debug"
QT_VERSION="6.5.3"

print_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help: Print this help message"
    echo "  clean: Remove all files from lib, bin and tmp"
    echo "  QT_PATH=path: Specify the path to where Qt is installed (eg. /c/Qt/)"
}

find_cmake() {
    local cmake_path=""

    # Function to check if a file exists
    check_path() {
        if [[ -f "$1" ]]; then
            cmake_path="$1"
            return 0
        else
            return 1
        fi
    }

    # See if we can find the cmake bundled with Qt
    if [[ "$PLATFORM" == "windows" ]]; then
        # Windows paths
        check_path "${QT_PATH}/Tools/CMake_64/bin/cmake.exe"
    elif [[ "$PLATFORM" == "linux" ]]; then
        # Linux paths
        check_path "/usr/bin/cmake" ||
        check_path "/usr/local/bin/cmake"
    elif [[ "$PLATFORM" == "macos" ]]; then
        # macOS paths
        check_path "/usr/local/bin/cmake"
    else
        echo "Unsupported platform: ${PLATFORM}"
        return 1
    fi

    # If cmake is found, print the path
    if [[ -n "$cmake_path" ]]; then
        echo "$cmake_path"
        return 0
    else
        echo ""
        return 1
    fi
}

find_qt() {
    local qt_path=""

    # Function to check if a dir exists
    check_path() {
        if [[ -d "$1" ]]; then
            qt_path="$1"
            return 0
        else
            return 1
        fi
    }

    # Check common Qt installation paths on different OSes
    if [[ "$PLATFORM" == "windows" ]]; then
        # Windows paths, maybe check for more in the future
        check_path "/c/Qt"
    elif [[ "$PLATFORM" == "linux" ]]; then
        # Linux paths
        # TODO: check this on a linux machine
        check_path "/usr/lib/qt5/" ||
        check_path "/usr/local/Qt-*/" ||
        check_path "$HOME/Qt5.*/" ||
        check_path "$HOME/Qt/5.*//"
    elif [[ "$PLATFORM" == "macos" ]]; then
        # macOS paths
        # TODO: check this on a mac machine
        check_path "$HOME/Qt"
    fi

    # If qt-cmake is found, print the path
    if [[ -n "$qt_path" ]]; then
        echo "$qt_path"
    else
        echo ""
        return 1
    fi
}

find_qt_mingw() {
    # Find a mingw compiler bundled with Qt

    QT_TOOLS_PATH=""

    if [[ "$PLATFORM" == "windows" ]]; then
        QT_TOOLS_PATH="/c/Qt/Tools"
        # find "/c/Qt/Tools/" -maxdepth 1 -type d -name "mingw*" -print0 | xargs -0 ls -td | head -n 1
    elif [[ "$PLATFORM" == "linux" ]]; then
        QT_TOOLS_PATH="/usr/lib/qt5"
    elif [[ "$PLATFORM" == "macos" ]]; then
        # macOS paths
        find /usr -name gcc | head -n 1
    fi
}

# Check if a given command returns a non-zero exit code
check_command() {
    if ! "$@" &> /dev/null; then
        return 1
    fi
    return 0
}

get_zip() {
    # Check if at least two arguments are provided
    if [ "$#" -lt 2 ]; then
        echo "Usage: get_zip <url> <source:destination> [<source:destination> ...]"
        return 1
    fi

    # URL of the zip file
    url="$1"
    shift

    zip_filename=$(basename "$url")

    # Temporary file to store the downloaded zip
    tmp_zip=./tmp/"$zip_filename"
    tmp_zip_dir=./tmp/"${zip_filename%.*}"

    # Download the zip file
    curl -L "$url" -o "$tmp_zip"
    if [ $? -ne 0 ]; then
        echo "Failed to download the zip file from $url"
        rm -f "$tmp_zip"
        return 1
    fi

    # unzip tmp zip file to tmp_zip_dir
    unzip "$tmp_zip" -d "$tmp_zip_dir"

    # Extract the specified files to their destinations
    while [ "$#" -gt 0 ]; do
        src_dst="$1"
        src="${src_dst%%:*}"
        dst="${src_dst##*:}"

        # Create the destination directory if it doesn't exist
        mkdir -p "$(dirname "$dst")"

        src_path="${tmp_zip_dir}/${src}"

        # Rename the extracted file to the desired destination name
        # cp "${tmp_zip_dir}$(dirname "$dst")/$(basename "$src")" "$dst"
        cp "$src_path" "$dst"

        shift
    done

    # Clean up the temporary zip file and directory
    rm -rf "$tmp_zip"
    rm -rf "$tmp_zip_dir"
}

get_bass() {
    echo "Checking for BASS..."
    # If lib/bass.h exists, assume that BASS is already present
    if [ -f "./lib/bass.h" ]; then
        echo "BASS is installed."
        return 0
    fi

    echo "Downloading BASS..."
    if [[ "$PLATFORM" == "windows" ]]; then
        get_zip https://www.un4seen.com/files/bass24.zip \
            c/bass.h:./lib/bass.h \
            c/x64/bass.lib:./lib/bass.lib \
            x64/bass.dll:./bin/bass.dll
    elif [[ "$PLATFORM" == "linux" ]]; then
        get_zip https://www.un4seen.com/files/bass24-linux.zip \
            bass.h:./lib/bass.h \
            libs/x86_64/libbass.so:./lib/libbass.so \
            libs/x86_64/libbass.so:./bin/libbass.so
    elif [[ "$PLATFORM" == "macos" ]]; then
        get_zip https://www.un4seen.com/files/bass24-osx.zip \
            bass.h:./lib/bass.h \
            libbass.dylib:./lib/libbass.dylib
    fi
}

get_bassopus() {
    echo "Checking for BASSOPUS..."
    # If lib/bassopus.h exists, assume that BASSOPUS is already present
    if [ -f "./lib/bassopus.h" ]; then
        echo "BASSOPUS is installed."
        return 0
    fi

    echo "Downloading BASSOPUS..."
    if [[ "$PLATFORM" == "windows" ]]; then
        get_zip https://www.un4seen.com/files/bassopus24.zip \
            c/bassopus.h:./lib/bassopus.h \
            c/x64/bassopus.lib:./lib/bassopus.lib \
            x64/bassopus.dll:./bin/bassopus.dll
    elif [[ "$PLATFORM" == "linux" ]]; then
        get_zip https://www.un4seen.com/files/bass24-linux.zip \
            bass/bassopus.h:./lib/bassopus.h \
            bass/libs/x86_64/libbassopus.so:./lib/libbassopus.so \
            bass/libs/x86_64/libbassopus.so:./bin/libbassopus.so
    elif [[ "$PLATFORM" == "macos" ]]; then
        get_zip https://www.un4seen.com/files/bassopus24-osx.zip \
            bassopus.h:./lib/bassopus.h \
            libbassopus.dylib:./lib/libbassopus.dylib
    fi
}

get_discordrpc() {
    echo "Checking for Discord RPC..."
    # If lib/discord_rpc.h exists, assume that Discord RPC is already present
    if [ -f "./lib/discord_rpc.h" ]; then
        echo "Discord RPC is installed."
        return 0
    fi

    echo "Downloading Discord RPC..."
    if [[ "$PLATFORM" == "windows" ]]; then
        get_zip https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-win.zip \
            discord-rpc/win64-dynamic/lib/discord-rpc.lib:./lib/discord-rpc.lib \
            discord-rpc/win64-dynamic/bin/discord-rpc.dll:./bin/discord-rpc.dll \
            discord-rpc/win64-dynamic/include/discord_rpc.h:./lib/discord_rpc.h \
            discord-rpc/win64-dynamic/include/discord_register.h:./lib/discord_register.h
    elif [[ "$PLATFORM" == "linux" ]]; then
        get_zip https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-linux.zip \
            discord-rpc/linux-dynamic/lib/libdiscord-rpc.so:./lib/libdiscord-rpc.so \
            discord-rpc/linux-dynamic/lib/libdiscord-rpc.so:./bin/libdiscord-rpc.so \
            discord-rpc/linux-dynamic/include/discord_rpc.h ./src/discord_rpc.h \
            discord-rpc/linux-dynamic/include/discord_register.h ./src/discord_register.h
    elif [[ "$PLATFORM" == "macos" ]]; then
        get_zip https://github.com/discord/discord-rpc/releases/download/v3.4.0/discord-rpc-osx.zip \
            osx-dynamic/lib/libdiscord-rpc.dylib:./lib/libdiscord-rpc.dylib \
            osx-dynamic/include/discord_rpc.h:./lib/discord_rpc.h \
            osx-dynamic/include/discord_rpc.h:./lib/discord_register.h
    fi
}

get_qtapng() {
    QT_PATH="$1"
    echo "Checking for Qt apng plugin..."
    if [ -f "./bin/imageformats/changeme" ]; then
        echo "Qt apng plugin is installed."
        return 0
    fi

    git clone git@github.com:jurplel/QtApng.git ./qtapng
    cd ./qtapng

    cmake . \
        -D CMAKE_PREFIX_PATH="$QT_PATH" \
        -D CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE="${SCRIPT_DIR}/bin/imageformats" \
        -D CMAKE_C_COMPILER=C:\Qt\Tools\mingw810_64\bin\gcc.exe \
        -D CMAKE_CXX_COMPILER=C:\Qt\Tools\mingw810_64\bin\g++.exe

    cmake --build . --config Release
    cd "${SCRIPT_DIR}"
}

configure() {
  # If -h is passed, print help
    if [ "$#" -gt 0 ] && { [ "$1" = "-h" ] || [ "$1" = "--help" ]; }; then
        print_help
        exit 0
    fi

    # If clean is passed, remove all files from lib, bin and tmp
    if [ "$#" -gt 0 ] && { [ "$1" = "clean" ]; }; then
        echo "Cleaning up... removing lib, bin and tmp"
        rm -rf ./lib/*
        rm -rf ./bin/*
        rm -rf ./tmp/*
        rm -rf ./qtapng/
        exit 0
    fi

    echo "Detected platform: ${PLATFORM}"

    # If platform is unknown, terminate
    if [ "$PLATFORM" = "unknown" ]; then
        echo "Unknown platform. Aborting."
        exit 1
    fi

    # Now we look for qt
    QT_PATH=""

    # If QT_PATH=path is passed, use that
    if [ "$#" -gt 0 ] && [ "${1%%=*}" = "QT_PATH" ]; then
        QT_PATH="${1#*=}"
        echo "Using argument QT_PATH: ${QT_PATH}"
        shift
    # Try to find it otherwise
    else
        echo "Trying to find Qt..."
        QT_PATH=$(find_qt)
        if [ -n "$QT_PATH" ]; then
            echo "Using found Qt: $QT_PATH"
        else
            echo "Qt not found. Aborting."; exit 1;
        fi
    fi

    # Make it globally available
    export QT_PATH

    # The first dependency is cmake, so check for that first
    echo "Checking if cmake is present..."
    if ! check_command cmake ; then
        # If cmake is not in path, we can see if Qt's bundled cmake is available
        CMAKE_PATH=$(find_cmake)
        if [ -n "$CMAKE_PATH" ]; then
            echo "Using found cmake: $CMAKE_PATH"
            export PATH="$(dirname "$CMAKE_PATH"):$PATH"
        else
            echo "CMake not found. Aborting."
            exit 1
        fi
    fi

    # Ensure the cmake command works as expected
    check_command cmake --version || { echo "cmake not working. Aborting."; exit 1; }

    # Check basic dependencies
    check_command curl --help || { echo "Command curl not found. Aborting"; exit 1; }
    check_command unzip --help || { echo "Command unzip not found. Aborting"; exit 1; }

    # Make sure key folders exist
    mkdir -p ./tmp/
    mkdir -p ./lib/
    mkdir -p ./bin/

    # Get the dependencies
    get_bass
    get_bassopus
    get_discordrpc
    # Qt Apng plugin needs Qt to compile
    get_qtapng "$QT_PATH"
}

configure "$@"
