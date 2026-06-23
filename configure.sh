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

detect_arch() {
    case "$(uname -m)" in
        x86_64|amd64) echo "x86_64";;
        arm64|aarch64) echo "arm64";;
        *) echo "unknown";;
    esac
}

# Basic data such as platform can be global
PLATFORM=$(detect_platform)
ARCH=$(detect_arch)
BUILD_CONFIG="Debug"
QT_MIN_VERSION="6.5.0"

print_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help: Print this help message"
    echo "  clean: Remove all files from lib, bin and tmp"
    echo "  QT_PATH=path: Use this Qt toolchain directly, skipping auto-detection (eg. /c/Qt/6.5.3/mingw_64)"
    echo "  BUILD_TYPE=Debug|Release: CMake build type (default: Debug)"
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
    # Auto-detect the Qt root by checking common install locations.
    # Emits the path on stdout, or empty string if nothing was found.
    local qt_root=""
    if [[ "$PLATFORM" == "windows" ]]; then
        qt_root="/c/Qt"
    else
        qt_root="$HOME/Qt"
    fi

    if [[ -d "$qt_root" ]]; then
        echo "$qt_root"
    else
        echo ""
    fi
}

find_qtpath() {
    # Pick the newest installed Qt under $QT_ROOT whose version is at least
    # QT_MIN_VERSION and which has the toolchain subdir for this platform.
    local toolchain=""
    if [[ "$PLATFORM" == "windows" ]]; then
        toolchain="mingw_64"
    elif [[ "$PLATFORM" == "linux" ]]; then
        toolchain="gcc_64"
    elif [[ "$PLATFORM" == "macos" ]]; then
        toolchain="macos"
    fi

    local best_ver=""
    local best_path=""

    shopt -s nullglob
    local dir ver
    for dir in "$QT_ROOT"/*/ ; do
        ver=$(basename "$dir")
        [[ "$ver" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || continue
        [[ -d "${dir}${toolchain}" ]] || continue
        # Skip versions below the floor
        if [[ "$(printf '%s\n%s\n' "$QT_MIN_VERSION" "$ver" | sort -V | head -n 1)" != "$QT_MIN_VERSION" ]]; then
            continue
        fi
        if [[ -z "$best_ver" || "$(printf '%s\n%s\n' "$best_ver" "$ver" | sort -V | tail -n 1)" == "$ver" ]]; then
            best_ver="$ver"
            best_path="${dir}${toolchain}"
        fi
    done
    shopt -u nullglob

    echo "$best_path"
}

find_cmake() {
    # Prefer the cmake bundled with Qt; emits the path on stdout, or empty
    # string if none is bundled (the caller falls back to cmake on PATH).
    local cmake_path=""
    if [[ "$PLATFORM" == "windows" ]]; then
        cmake_path="${QT_ROOT}/Tools/CMake_64/bin/cmake.exe"
    elif [[ "$PLATFORM" == "linux" ]]; then
        cmake_path="${QT_ROOT}/Tools/CMake/bin/cmake"
    elif [[ "$PLATFORM" == "macos" ]]; then
        cmake_path="${QT_ROOT}/Tools/CMake/CMake.app/Contents/bin/cmake"
    fi

    if [[ -f "$cmake_path" ]]; then
        echo "$cmake_path"
    else
        echo ""
    fi
}

find_mingw() {
    # Find the newest MinGW installation bundled under ${QT_ROOT}/Tools/.
    # Emits the path on stdout, or empty if the Tools dir or mingw dir is absent.
    local tools_path="${QT_ROOT}/Tools"
    if [[ ! -d "$tools_path" ]]; then
        echo ""
        return 0
    fi

    local mingw_dir=""
    mingw_dir=$(find "$tools_path" -maxdepth 1 -type d -name "mingw*" -print0 \
        | xargs -0 -r ls -td 2>/dev/null \
        | head -n 1)

    echo "$mingw_dir"
}

find_ninja() {
    # Prefer the ninja bundled with Qt, fall back to ninja on PATH.
    local bundled=""
    if [[ "$PLATFORM" == "windows" ]]; then
        bundled="${QT_ROOT}/Tools/Ninja/ninja.exe"
    else
        bundled="${QT_ROOT}/Tools/Ninja/ninja"
    fi

    if [[ -f "$bundled" ]]; then
        echo "$bundled"
        return 0
    fi

    if command -v ninja >/dev/null 2>&1; then
        echo "ninja"
        return 0
    fi

    echo ""
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

    # First, check that all the specified files exist in the zip archive.
    # Snapshot the listing into a variable — piping to `grep -q` under
    # `set -o pipefail` can trip SIGPIPE on `unzip` and spuriously fail.
    zip_listing=$(unzip -l "$tmp_zip")
    for arg in "$@" ; do
        src_file="${arg%%:*}"
        if ! grep -q "$src_file" <<< "$zip_listing"; then
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

        unzip -o -j "$tmp_zip" "$src_file" -d "$dst_dir"

        shift
    done

    # Clean up the temporary zip file
    rm -rf "$tmp_zip"
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
        if [[ "$ARCH" == "x86_64" ]]; then
            get_zip https://github.com/discord/discord-rpc/releases/download/v3.4.0/discord-rpc-osx.zip \
                discord-rpc/osx-dynamic/lib/libdiscord-rpc.dylib:./lib \
                discord-rpc/osx-dynamic/include/discord_rpc.h:./lib \
                discord-rpc/osx-dynamic/include/discord_register.h:./lib
        else
            # The official discord-rpc v3.4.0 release only ships an x86_64
            # dylib and the repo was archived in 2018, so there is no native
            # arm64 build. Skip the download — Discord RPC is disabled at
            # build time on arm64 macOS via -DAO_ENABLE_DISCORD_RPC=OFF below.
            echo "Skipping Discord RPC on macOS ${ARCH} (no native binary available)."
        fi
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
        git clone https://github.com/jurplel/QtApng.git ./qtapng
    fi

    cd ./qtapng

    $CMAKE . \
        -G Ninja \
        -DCMAKE_MAKE_PROGRAM="$NINJA" \
        -DCMAKE_PREFIX_PATH="$QT_PATH" \
        -DCMAKE_MODULE_PATH="${SCRIPT_DIR}/cmake" \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX"

    "$CMAKE" --build . --parallel

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
    git clone https://github.com/AttorneyOnline/AO2-Themes.git ./bin/base/themes

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

    # Parse KEY=VALUE overrides
    QT_PATH=""
    while [ "$#" -gt 0 ]; do
        case "$1" in
            QT_PATH=*) QT_PATH="${1#*=}" ;;
            BUILD_TYPE=*) BUILD_CONFIG="${1#*=}" ;;
            *) echo "Unknown argument: $1"; print_help; exit 1 ;;
        esac
        shift
    done

    # Resolve QT_PATH: explicit override wins, otherwise auto-detect under $HOME/Qt.
    # QT_ROOT is the parent of the version dir, derived from QT_PATH. Tools/ lives
    # under it (find_cmake / find_mingw / find_ninja look there).
    if [ -n "$QT_PATH" ]; then
        if [ ! -d "$QT_PATH" ]; then
            echo "$QT_PATH is not a directory. Aborting."
            exit 1
        fi
        QT_ROOT="$(cd "$QT_PATH/../.." && pwd)"
    else
        QT_ROOT=$(find_qt)
        if [ -z "$QT_ROOT" ]; then
            echo "Qt not found. Aborting."; exit 1;
        fi
        QT_PATH=$(find_qtpath)
        if [ -z "$QT_PATH" ] || [ ! -d "$QT_PATH" ]; then
            echo "No Qt >= ${QT_MIN_VERSION} found under ${QT_ROOT}. Aborting."
            exit 1
        fi
    fi
    echo "Using Qt root: $QT_ROOT"
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

    # Prefer the MinGW bundled with Qt on Windows; fall back to gcc/g++ on PATH.
    # On non-Windows platforms the system compiler is usually safe.
    CC=""
    CXX=""
    if [[ "$PLATFORM" == "windows" ]]; then
        MINGW_PATH=$(find_mingw)
        if [ -n "$MINGW_PATH" ]; then
            CC="${MINGW_PATH}/bin/gcc.exe"
            CXX="${MINGW_PATH}/bin/g++.exe"
        else
            echo "No MinGW bundled with Qt found. Trying PATH..."
            CC="gcc"
            CXX="g++"
        fi
    else
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
    get_discordrpc
    get_qtapng
    get_themes

    # Discord RPC has no native arm64 macOS binary, so turn it off there.
    EXTRA_CMAKE_FLAGS=""
    if [[ "$PLATFORM" == "macos" && "$ARCH" != "x86_64" ]]; then
        EXTRA_CMAKE_FLAGS="-DAO_ENABLE_DISCORD_RPC=OFF"
    fi

    # Typically, IDEs like running cmake themselves, but we need the binary to fix dependencies correctly
    FULL_CMAKE_CMD="\
$CMAKE . \
-G Ninja \
-DCMAKE_MAKE_PROGRAM=${NINJA} \
-DCMAKE_PREFIX_PATH=${QT_PATH} \
-DCMAKE_MODULE_PATH=${SCRIPT_DIR}/cmake \
-DCMAKE_BUILD_TYPE=${BUILD_CONFIG} \
-DCMAKE_C_COMPILER=${CC} \
-DCMAKE_CXX_COMPILER=${CXX} \
${EXTRA_CMAKE_FLAGS}"

    $FULL_CMAKE_CMD
    "$CMAKE" --build . --parallel

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
