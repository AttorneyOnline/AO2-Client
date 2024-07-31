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
BUILD_CONFIG="Debug"
QT_VERSION="6.5.3"

print_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help: Print this help message"
    echo "  clean: Remove all files from lib, bin and tmp"
    echo "  QT_ROOT=path: Specify the root path to where Qt is installed (eg. /c/Qt/)"
}

# Check if a given command returns a non-zero exit code
check_command() {
    # Hack to not make the whole script exit..
    set +e
    if ! "$@" &> /dev/null; then
        set -e
        return 1
    fi
    set -e
    return 0
}

find_qt() {
    local qt_root=""

    # Function to check if a dir exists
    check_path() {
        if [[ -d "$1" ]]; then
            qt_root="$1"
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
        check_path "$HOME/Qt"
    elif [[ "$PLATFORM" == "macos" ]]; then
        check_path "$HOME/Qt"
    fi

    # If qt-cmake is found, print the path
    if [[ -n "$qt_root" ]]; then
        echo "$qt_root"
    else
        echo ""
        return 1
    fi
}

find_qtpath() {
    local qt_path=""

    check_path() {
        if [[ -d "$1" ]]; then
            qt_path="$1"
            return 0
        else
            return 1
        fi
    }

    if [[ "$PLATFORM" == "windows" ]]; then
        check_path "${QT_ROOT}/${QT_VERSION}/mingw_64"
    elif [[ "$PLATFORM" == "linux" ]]; then
        check_path "${QT_ROOT}/${QT_VERSION}/gcc_64"
    elif [[ "$PLATFORM" == "macos" ]]; then
        check_path "${QT_ROOT}/${QT_VERSION}/macos"
    fi

    echo "$qt_path"
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
        check_path "${QT_ROOT}/Tools/CMake_64/bin/cmake.exe"
    elif [[ "$PLATFORM" == "linux" ]]; then
        check_path "${QT_ROOT}/Tools/CMake/bin/cmake"
    elif [[ "$PLATFORM" == "macos" ]]; then
        check_path "${QT_ROOT}/Tools/CMake/CMake.app/Contents/bin/cmake"
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

find_mingw() {
    # Find a mingw installation bundled with Qt

    QT_TOOLS_PATH="${QT_ROOT}/Tools"

    mingw_dir=$(find "${QT_TOOLS_PATH}" -maxdepth 1 -type d -name "mingw*" -print0 | xargs -0 ls -td | head -n 1)

    # Find returns . if the directory is not found
    if [[ "$mingw_dir" == "." ]]; then
        mingw_dir=""
    fi

    echo "$mingw_dir"
}

find_ninja() {
    # Find a ninja installation bundled with Qt
    QT_TOOLS_PATH="${QT_ROOT}/Tools"

    local ninja_path=""

    if [[ "$PLATFORM" == "windows" ]]; then
        ninja_path="${QT_TOOLS_PATH}/Ninja/ninja.exe"
    else
        ninja_path="${QT_TOOLS_PATH}/Ninja/ninja"
    fi

    echo "$ninja_path"
}

get_zip() {
    # Check if at least two arguments are provided
    if [ "$#" -lt 2 ]; then
        echo "Usage: get_zip <url> <sourcefile:destination> [<sourcefile:destination> ...]"
        return 1
    fi

    mkdir -p ./tmp

    # URL of the zip file
    url="$1"
    shift

    zip_filename=$(basename "$url")

    # Temporary file to store the downloaded zip
    tmp_zip=./tmp/"$zip_filename"

    # Download the zip file
    curl -L "$url" -o "$tmp_zip"
    if [ $? -ne 0 ]; then
        echo "Failed to download the zip file from $url"
        rm -f "$tmp_zip"
        return 1
    fi

    # Sanity check: zip file is there
    if [ ! -f "$tmp_zip" ]; then
        echo "Error: The zip file '$tmp_zip' does not exist."
        return 1
    fi

    # First, check that all the specified files exist in the zip archive
    for arg in "$@" ; do
        src_file="${arg%%:*}"
        if ! unzip -l "$tmp_zip" | grep -q "$src_file"; then
            echo "Error: The file '$src_file' does not exist in the zip archive $tmp_zip."
            return 1
        fi
    done

    # Extract the specified files to their destinations
    while [ "$#" -gt 0 ]; do
        src_dst="$1"
        src_file="${src_dst%%:*}"
        dst_dir="${src_dst##*:}"

        # Create the destination directory if it doesn't exist
        mkdir -p "$dst_dir"

        unzip -j "$tmp_zip" "$src_file" -d "$dst_dir"

        shift
    done

    # Clean up the temporary zip file
    rm -rf "$tmp_zip"
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
            c/bass.h:./lib \
            c/x64/bass.lib:./lib \
            x64/bass.dll:./bin
    elif [[ "$PLATFORM" == "linux" ]]; then
        get_zip https://www.un4seen.com/files/bass24-linux.zip \
            bass.h:./lib \
            libs/x86_64/libbass.so:./lib \
            libs/x86_64/libbass.so:./bin
    elif [[ "$PLATFORM" == "macos" ]]; then
        get_zip https://www.un4seen.com/files/bass24-osx.zip \
            bass.h:./lib \
            libbass.dylib:./lib
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
            c/bassopus.h:./lib \
            c/x64/bassopus.lib:./lib \
            x64/bassopus.dll:./bin
    elif [[ "$PLATFORM" == "linux" ]]; then
        get_zip https://www.un4seen.com/files/bassopus24-linux.zip \
            bassopus.h:./lib \
            libs/x86_64/libbassopus.so:./lib \
            libs/x86_64/libbassopus.so:./bin
    elif [[ "$PLATFORM" == "macos" ]]; then
        get_zip https://www.un4seen.com/files/bassopus24-osx.zip \
            bassopus.h:./lib \
            libbassopus.dylib:./lib
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
            discord-rpc/win64-dynamic/lib/discord-rpc.lib:./lib \
            discord-rpc/win64-dynamic/bin/discord-rpc.dll:./bin \
            discord-rpc/win64-dynamic/include/discord_rpc.h:./lib \
            discord-rpc/win64-dynamic/include/discord_register.h:./lib
    elif [[ "$PLATFORM" == "linux" ]]; then
        get_zip https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-linux.zip \
            discord-rpc/linux-dynamic/lib/libdiscord-rpc.so:./lib \
            discord-rpc/linux-dynamic/lib/libdiscord-rpc.so:./bin \
            discord-rpc/linux-dynamic/include/discord_rpc.h:./lib \
            discord-rpc/linux-dynamic/include/discord_register.h:./lib
    elif [[ "$PLATFORM" == "macos" ]]; then
        get_zip https://github.com/discord/discord-rpc/releases/download/v3.4.0/discord-rpc-osx.zip \
            discord-rpc/osx-dynamic/lib/libdiscord-rpc.dylib:./lib \
            discord-rpc/osx-dynamic/include/discord_rpc.h:./lib \
            discord-rpc/osx-dynamic/include/discord_rpc.h:./lib
    fi
}

get_qtapng() {
    echo "Checking for Qt apng plugin..."
    apng_build_dir="./qtapng/plugins/imageformats"
    imageformats_dir="./bin/imageformats"
    APNG_LIB=""
    if [[ "$PLATFORM" == "windows" ]]; then
        APNG_LIB="qapng.dll"
    elif [[ "$PLATFORM" == "linux" ]]; then
        APNG_LIB="libqapng.so"
    elif [[ "$PLATFORM" == "macos" ]]; then
        APNG_LIB="libqapng.dylib"
    else
        echo "Unsupported platform: ${PLATFORM}"
        return 1
    fi

    apng_dst_path="${imageformats_dir}/${APNG_LIB}"

    if [ -f "$apng_dst_path" ]; then
        echo "Qt apng plugin is installed."
        return 0
    fi

    if [ ! -d "./qtapng" ]; then
        git clone git@github.com:jurplel/QtApng.git ./qtapng
    fi

    cd ./qtapng

    $CMAKE . \
        -G Ninja \
        -DCMAKE_MAKE_PROGRAM="$NINJA" \
        -DCMAKE_PREFIX_PATH="$QT_PATH" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX"

    $NINJA

    cd "${SCRIPT_DIR}"

    mkdir -p "$imageformats_dir"

    apng_src_path="${apng_build_dir}/${APNG_LIB}"

    cp "$apng_src_path" "$apng_dst_path"
}

get_themes() {
    echo "Checking for themes..."
    if [ -d "./bin/base/themes" ]; then
        echo "Themes are installed."
        return 0
    fi

    echo "Downloading themes..."
    git clone git@github.com:AttorneyOnline/AO2-Themes.git ./bin/base/themes

    rm -rf ./bin/base/themes/.gitignore
    rm -rf ./bin/base/themes/.gitattributes
    rm -rf ./bin/base/themes/.git
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

    echo "Platform: ${PLATFORM}"

    # If platform is unknown, terminate
    if [ "$PLATFORM" == "unknown" ]; then
        echo "Unknown platform. Aborting."
        exit 1
    fi

    # Now we look for qt
    QT_ROOT=""

    # If QT_ROOT=path is passed, use that
    if [ "$#" -gt 0 ] && [ "${1%%=*}" = "QT_ROOT" ]; then
        QT_ROOT="${1#*=}"
        shift
    # Try to find it otherwise
    else
        QT_ROOT=$(find_qt)
        if [ -z "$QT_ROOT" ]; then
            echo "Qt not found. Aborting."; exit 1;
        fi
    fi
    if [ ! -d "$QT_ROOT" ]; then
        echo "$QT_ROOT is not a directory. Aborting."
        exit 1
    fi
    echo "Using Qt root: $QT_ROOT"

    QT_PATH=$(find_qtpath)
    if [ ! -d "$QT_PATH" ]; then
        echo "$QT_PATH is not a directory. Aborting."
        exit 1
    fi
    echo "Using Qt installation: $QT_PATH"

    # Check for cmake, and prefer the one bundled with Qt
    CMAKE=$(find_cmake)
    if [ -z "$CMAKE" ]; then
        echo "No cmake bundled with Qt found. Trying path..."
        if ! check_command cmake ; then
            echo "CMake not found. Aborting."
            exit 1
        fi
        CMAKE="cmake"
    fi

    check_command "$CMAKE" --version || { echo "cmake not working. Aborting."; exit 1; }
    echo "Using cmake: $CMAKE"

    # Find the compiler bundled in Qt
    CC=""
    CXX=""
    # If we're on Windows, find mingw
    if [[ "$PLATFORM" == "windows" ]]; then
        MINGW_PATH=$(find_mingw)
        if [ -z "$MINGW_PATH" ]; then
            echo "MinGW not found. Aborting."
            exit 1
        fi
        CC="${MINGW_PATH}/bin/gcc.exe"
        CXX="${MINGW_PATH}/bin/g++.exe"
    else
        # On non-Windows platforms, use the system compiler, as it's usually safe
        CC="gcc"
        CXX="g++"
    fi
    check_command "$CC" --version || { echo "CC not working. Aborting"; exit 1; }
    echo "Using CC: $CC"

    check_command "$CXX" --version || { echo "CXX not working. Aborting"; exit 1; }
    echo "Using CXX: $CXX"

    NINJA=""
    NINJA=$(find_ninja)
    check_command "$NINJA" --version || { echo "Ninja not working. Aborting"; exit 1; }
    echo "Using Ninja: $NINJA"

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
    get_qtapng
    get_themes

    # Typically, IDEs like running cmake themselves, but we need the binary to fix dependencies correctly
    FULL_CMAKE_CMD="\
$CMAKE . \
-G Ninja \
-DCMAKE_MAKE_PROGRAM=${NINJA} \
-DCMAKE_PREFIX_PATH=${QT_PATH} \
-DCMAKE_BUILD_TYPE=${BUILD_CONFIG} \
-DCMAKE_C_COMPILER=${CC} \
-DCMAKE_CXX_COMPILER=${CXX}"

    $FULL_CMAKE_CMD
    $NINJA

    if [[ "$PLATFORM" == "windows" ]]; then
        echo "Fixing dependencies..."
        windeployqt="${QT_PATH}/bin/windeployqt.exe"
        "$windeployqt" --no-quick-import --no-translations --no-compiler-runtime --no-opengl-sw ./bin/Attorney_Online.exe
    fi

    echo "Configuration and build complete."
    echo "Full cmake cmd: $FULL_CMAKE_CMD"
    echo "$FULL_CMAKE_CMD" > cmake_cmd.txt
}

configure "$@"
