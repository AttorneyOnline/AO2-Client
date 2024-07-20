#!/bin/bash

set -euo pipefail

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
QT_VERSION="6.5.3"

print_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help: Print this help message"
}

find_qt_cmake() {
    local qt_cmake_path=""

    # Function to check if a file exists
    check_path() {
        if [[ -f "$1" ]]; then
            qt_cmake_path="$1"
            return 0
        else
            return 1
        fi
    }

    # Check common Qt installation paths on different OSes
    if [[ "$PLATFORM" == "linux" ]]; then
        # Linux paths
        check_path "/usr/lib/qt5/bin/qt-cmake" ||
        check_path "/usr/local/Qt-*/bin/qt-cmake" ||
        check_path "$HOME/Qt5.*/bin/qt-cmake" ||
        check_path "$HOME/Qt/5.*/gcc_64/bin/qt-cmake"
    elif [[ "$PLATFORM" == "macos" ]]; then
        # macOS paths
        check_path "/usr/local/opt/qt5/bin/qt-cmake" ||
        check_path "/usr/local/Qt-*/bin/qt-cmake" ||
        check_path "$HOME/Qt5.*/bin/qt-cmake" ||
        check_path "$HOME/Qt/5.*/clang_64/bin/qt-cmake"
    elif [[ "$PLATFORM" == "windows" ]]; then
        # Windows paths
        check_path "/c/Qt/${QT_VERSION}/mingw_64/bin/qt-cmake.exe"
    else
        echo "Unsupported platform: ${PLATFORM}"
        return 1
    fi

    # If qt-cmake is found, print the path
    if [[ -n "$qt_cmake_path" ]]; then
        echo "$qt_cmake_path"
    else
        echo "qt-cmake not found"
        return 1
    fi
}

check_command() {
    COMMAND=$1
    if ! command -v "${COMMAND}" &> /dev/null; then
        echo "Command ${COMMAND} not found."
        return 1
    fi
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

    # Download the zip file
    curl -o "$tmp_zip" "$url"
    if [ $? -ne 0 ]; then
        echo "Failed to download the zip file from $url"
        rm -f "$tmp_zip"
        return 1
    fi

    # Extract the specified files to their destinations
    while [ "$#" -gt 0 ]; do
        src_dst="$1"
        src="${src_dst%%:*}"
        dst="${src_dst##*:}"

        # Create the destination directory if it doesn't exist
        mkdir -p "$(dirname "$dst")"

        # Extract the file
        unzip -j "$tmp_zip" "$src" -d "$(dirname "$dst")"
        if [ $? -ne 0 ]; then
            echo "Failed to extract $src to $dst"
            rm -f "$tmp_zip"
            return 1
        fi

        # Rename the extracted file to the desired destination name
        mv "$(dirname "$dst")/$(basename "$src")" "$dst"

        shift
    done

    # Clean up the temporary zip file
    rm -f "$tmp_zip"
}

configure_windows() {
    # Bass
    get_zip https://www.un4seen.com/files/bass24.zip \
        c/bass.h:./lib/bass.h \
        c/x64/bass.lib:./lib/bass.lib \
        x64/bass.dll:./bin/bass.dll

    # Bass Opus
    get_zip https://www.un4seen.com/files/bassopus24.zip \
        bass/c/bassopus.h:./lib/bassopus.h \
        bass/c/x64/bassopus.lib:./lib/bassopus.lib \
        bass/x64/bassopus.dll:./bin/bassopus.dll

  # Discord RPC
    get_zip https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-win.zip \
        discord-rpc/win64-dynamic/lib/discord-rpc.lib:./lib/discord-rpc.lib \
        discord-rpc/win64-dynamic/bin/discord-rpc.dll:./bin/discord-rpc.dll \
        discord-rpc/win64-dynamic/include/discord_rpc.h:./lib/discord_rpc.h \
        discord-rpc/win64-dynamic/include/discord_register.h:./lib/discord_register.h

  # APng
  # TODO
}

configure_linux() {
  # Bass
    get_zip https://www.un4seen.com/files/bass24-linux.zip \
        bass.h:./lib/bass.h \
        libs/x86_64/libbass.so:./lib/libbass.so \
        libs/x86_64/libbass.so:./bin/libbass.so

  # Bass Opus
    get_zip https://www.un4seen.com/files/bass24-linux.zip \
        bass/bassopus.h:./lib/bassopus.h \
        bass/libs/x86_64/libbassopus.so:./lib/libbassopus.so \
        bass/libs/x86_64/libbassopus.so:./bin/libbassopus.so

  # Discord RPC
    get_zip https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-linux.zip \
        discord-rpc/linux-dynamic/lib/libdiscord-rpc.so:./lib/libdiscord-rpc.so \
        discord-rpc/linux-dynamic/lib/libdiscord-rpc.so:./bin/libdiscord-rpc.so \
        discord-rpc/linux-dynamic/include/discord_rpc.h ./src/discord_rpc.h \
        discord-rpc/linux-dynamic/include/discord_register.h ./src/discord_register.h

  # APng
  # TODO
}

configure_macos() {
  # Bass
    get_zip https://www.un4seen.com/files/bass24-osx.zip \
        bass.h:./lib/bass.h \
        libbass.dylib:./lib/libbass.dylib

  # Bass Opus
  # TODO

  # Discord RPC
    get_zip https://github.com/discord/discord-rpc/releases/download/v3.4.0/discord-rpc-osx.zip \
        osx-dynamic/lib/libdiscord-rpc.dylib:./lib/libdiscord-rpc.dylib \
        osx-dynamic/include/discord_rpc.h:./lib/discord_rpc.h \
        osx-dynamic/include/discord_rpc.h:./lib/discord_register.h

  # APng
  # TODO
}

configure() {
  # If -h is passed, print help
    if [ "$#" -gt 0 ] && { [ "$1" = "-h" ] || [ "$1" = "--help" ]; }; then
        print_help
        exit 0
    fi

    echo "Detected platform: ${PLATFORM}"

    # If platform is unknown, terminate
    if [ "$PLATFORM" = "unknown" ]; then
        echo "Unknown platform. Aborting."
        exit 1
    fi

    QT_CMAKE=""

    # If QT_CMAKE=path is passed, assign variable
    if [ "$#" -gt 0 ] && [ "${1%%=*}" = "QT_CMAKE" ]; then
        QT_CMAKE="${1#*=}"
        echo "Using argument QT_CMAKE: ${QT_CMAKE}"
        shift
    # If not, check for environment variable
    elif [ -n "${QT_CMAKE}" ]; then
        echo "Using QT_CMAKE environment variable: ${QT_CMAKE}"
    # Try to find it otherwise
    else
        echo "Qt cmake not found. Attempting to find it..."
        QT_CMAKE=$(find_qt_cmake)
    fi

    echo "$QT_CMAKE"

    # Check required commands
    check_command cmake
    check_command curl || echo "Aborting"; exit 1;
    check_command unzip

    # Make sure key folders exist
    mkdir -p ./tmp/
    mkdir -p ./lib/
    mkdir -p ./bin/

    # Do platform-specific conf
    case "${PLATFORM}" in
        windows*)   configure_windows;;
        linux*)     configure_linux;;
        macos*)     configure_macos;;
        *)          echo "Unsupported platform: ${unameOut}"; exit 1;
    esac
}

configure "$@"
