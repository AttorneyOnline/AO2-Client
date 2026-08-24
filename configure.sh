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

PLATFORM=$(detect_platform)
ARCH=$(detect_arch)
QT_MIN_VERSION="6.5.0"

# BUILD_CONFIG (Debug|Release, default Debug) and QT_ROOT_DIR (the Qt toolchain
# dir; empty auto-detects under ~/Qt) are read from the environment.
BUILD_CONFIG="${BUILD_CONFIG:-Debug}"
QT_ROOT_DIR="${QT_ROOT_DIR:-}"

print_help() {
    echo "Usage: [BUILD_CONFIG=Debug|Release] [QT_ROOT_DIR=path] $0 [command]"
    echo "Commands:"
    echo "  -h, --help: Print this help message"
    echo "  clean: Remove everything configure.sh writes (deps, build files, build.env, cmake_cmd.txt)"
    echo "Environment variables:"
    echo "  BUILD_CONFIG=Debug|Release: CMake build type (default: Debug)"
    echo "  QT_ROOT_DIR=path: Qt toolchain dir to use directly (eg. ~/Qt/6.8.3/gcc_arm64); empty auto-detects under ~/Qt"
}

check_command() {
    # set +e around the probe so a failure returns non-zero instead of exiting.
    set +e
    if ! "$@" &> /dev/null; then
        set -e
        return 1
    fi
    set -e
    return 0
}

find_qt() {
    # Emit the Qt root if a common install location exists, else empty string.
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
    # Emit the newest Qt >= QT_MIN_VERSION under $QT_ROOT that has a usable
    # desktop toolchain for this platform/arch, else empty string.
    local -a candidates=()
    if [[ "$PLATFORM" == "windows" ]]; then
        candidates=(mingw_64)
    elif [[ "$PLATFORM" == "macos" ]]; then
        candidates=(macos)
    elif [[ "$PLATFORM" == "linux" ]]; then
        if [[ "$ARCH" == "arm64" ]]; then
            candidates=(gcc_arm64 arm64)
        else
            candidates=(gcc_64)
        fi
    fi

    local best_ver=""
    local best_path=""

    shopt -s nullglob
    local dir ver tc sub path
    for dir in "$QT_ROOT"/*/ ; do
        ver=$(basename "$dir")
        [[ "$ver" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || continue

        # Prefer a known toolchain dir name; otherwise fall back to any subdir
        # that provides the Qt6 CMake package (what we pass as CMAKE_PREFIX_PATH)
        # and isn't a cross-compile target (android/wasm/ios).
        path=""
        for tc in "${candidates[@]}"; do
            [[ -d "${dir}${tc}" ]] && { path="${dir}${tc}"; break; }
        done
        if [[ -z "$path" ]]; then
            for sub in "${dir}"*/ ; do
                case "$(basename "$sub")" in android*|wasm*|ios*) continue ;; esac
                if [[ -d "${sub}lib/cmake/Qt6" ]]; then
                    path="${sub%/}"; break
                fi
            done
        fi
        [[ -n "$path" ]] || continue

        if [[ "$(printf '%s\n%s\n' "$QT_MIN_VERSION" "$ver" | sort -V | head -n 1)" != "$QT_MIN_VERSION" ]]; then
            continue
        fi
        if [[ -z "$best_ver" || "$(printf '%s\n%s\n' "$best_ver" "$ver" | sort -V | tail -n 1)" == "$ver" ]]; then
            best_ver="$ver"
            best_path="$path"
        fi
    done
    shopt -u nullglob

    echo "$best_path"
}

find_cmake() {
    # Emit the cmake bundled with Qt, else empty (caller falls back to PATH).
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
    # Emit the newest MinGW bundled under ${QT_ROOT}/Tools/, else empty.
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
    if [ "$#" -lt 2 ]; then
        echo "Usage: get_zip <url> <sourcefile:destination> [<sourcefile:destination> ...]"
        return 1
    fi

    mkdir -p ./tmp

    url="$1"
    shift

    zip_filename=$(basename "$url")
    tmp_zip=./tmp/"$zip_filename"

    curl -L "$url" -o "$tmp_zip"
    if [ $? -ne 0 ]; then
        echo "Failed to download the zip file from $url"
        rm -f "$tmp_zip"
        return 1
    fi

    if [ ! -f "$tmp_zip" ]; then
        echo "Error: The zip file '$tmp_zip' does not exist."
        return 1
    fi

    # Snapshot the listing first; piping unzip to `grep -q` under pipefail can
    # trip SIGPIPE on unzip and spuriously fail.
    zip_listing=$(unzip -l "$tmp_zip")
    for arg in "$@" ; do
        src_file="${arg%%:*}"
        if ! grep -q "$src_file" <<< "$zip_listing"; then
            echo "Error: The file '$src_file' does not exist in the zip archive $tmp_zip."
            return 1
        fi
    done

    while [ "$#" -gt 0 ]; do
        src_dst="$1"
        src_file="${src_dst%%:*}"
        dst_dir="${src_dst##*:}"

        mkdir -p "$dst_dir"

        unzip -o -j "$tmp_zip" "$src_file" -d "$dst_dir"

        shift
    done

    rm -rf "$tmp_zip"
}

get_bass() {
    echo "Checking for BASS..."
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
        local arch_dir="x86_64"
        [[ "$ARCH" == "arm64" ]] && arch_dir="aarch64"
        get_zip https://www.un4seen.com/files/bass24-linux.zip \
            c/bass.h:./lib \
            "libs/${arch_dir}/libbass.so:./lib" \
            "libs/${arch_dir}/libbass.so:./bin"
    elif [[ "$PLATFORM" == "macos" ]]; then
        get_zip https://www.un4seen.com/files/bass24-osx.zip \
            c/bass.h:./lib \
            libbass.dylib:./lib
    fi
}

get_bassopus() {
    echo "Checking for BASSOPUS..."
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
        local arch_dir="x86_64"
        [[ "$ARCH" == "arm64" ]] && arch_dir="aarch64"
        get_zip https://www.un4seen.com/files/bassopus24-linux.zip \
            c/bassopus.h:./lib \
            "libs/${arch_dir}/libbassopus.so:./lib" \
            "libs/${arch_dir}/libbassopus.so:./bin"
    elif [[ "$PLATFORM" == "macos" ]]; then
        get_zip https://www.un4seen.com/files/bassopus24-osx.zip \
            c/bassopus.h:./lib \
            libbassopus.dylib:./lib
    fi
}

get_discordrpc() {
    echo "Checking for Discord RPC..."
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
        if [[ "$ARCH" == "x86_64" ]]; then
            get_zip https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-linux.zip \
                discord-rpc/linux-dynamic/lib/libdiscord-rpc.so:./lib \
                discord-rpc/linux-dynamic/lib/libdiscord-rpc.so:./bin \
                discord-rpc/linux-dynamic/include/discord_rpc.h:./lib \
                discord-rpc/linux-dynamic/include/discord_register.h:./lib
        else
            # discord-rpc v3.4.0 only ships x86_64 binaries, so there is no arm64
            # build. Disabled at build time via -DAO_ENABLE_DISCORD_RPC=OFF below.
            echo "Skipping Discord RPC on Linux ${ARCH} (no native binary available)."
        fi
    elif [[ "$PLATFORM" == "macos" ]]; then
        if [[ "$ARCH" == "x86_64" ]]; then
            get_zip https://github.com/discord/discord-rpc/releases/download/v3.4.0/discord-rpc-osx.zip \
                discord-rpc/osx-dynamic/lib/libdiscord-rpc.dylib:./lib \
                discord-rpc/osx-dynamic/include/discord_rpc.h:./lib \
                discord-rpc/osx-dynamic/include/discord_register.h:./lib
        else
            # discord-rpc v3.4.0 only ships an x86_64 dylib and the repo was
            # archived in 2018, so there is no arm64 build, so Discord RPC is
            # disabled at build time on arm64 macOS (-DAO_ENABLE_DISCORD_RPC=OFF).
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
        -DCMAKE_PREFIX_PATH="$QT_ROOT_DIR" \
        -DCMAKE_MODULE_PATH="${SCRIPT_DIR}/cmake" \
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
    git clone https://github.com/AttorneyOnline/AO2-Themes.git ./bin/base/themes

    rm -rf ./bin/base/themes/.gitignore
    rm -rf ./bin/base/themes/.gitattributes
    rm -rf ./bin/base/themes/.git
}

install_build_tools() {
    # Install the non-Qt build tools if missing. The presence check keeps local
    # re-runs from invoking sudo/brew; fresh CI runners install here.
    if [[ "$PLATFORM" == "linux" ]]; then
        local -a pkgs=()
        command -v ninja    >/dev/null 2>&1 || pkgs+=(ninja-build)
        command -v patchelf >/dev/null 2>&1 || pkgs+=(patchelf)
        command -v cmake    >/dev/null 2>&1 || pkgs+=(cmake)
        command -v curl     >/dev/null 2>&1 || pkgs+=(curl)
        # Library packages have no command to probe, so check dpkg. The GL dev
        # libs satisfy Qt6Gui's WrapOpenGL; libxcb-cursor0 lets Qt apps launch.
        local libs
        for libs in libxcb-cursor0 libgl1-mesa-dev libglvnd-dev mesa-common-dev; do
            dpkg -s "$libs" >/dev/null 2>&1 || pkgs+=("$libs")
        done
        if [ ${#pkgs[@]} -gt 0 ]; then
            echo "Installing system packages: ${pkgs[*]}"
            sudo apt-get update
            sudo apt-get install -y "${pkgs[@]}"
        fi
    elif [[ "$PLATFORM" == "macos" ]]; then
        if ! command -v ninja >/dev/null 2>&1; then
            echo "Installing system build tools (ninja)..."
            brew install ninja
        fi
    fi
}

clean() {
    echo "Cleaning up all files written by configure.sh..."

    # Downloaded dependencies, cloned qtapng, and build/runtime output.
    rm -rf ./lib/* ./bin/* ./tmp/* ./qtapng/

    # Files configure.sh writes directly.
    rm -f ./build.env ./cmake_cmd.txt

    # In-source CMake/Ninja build artifacts.
    rm -rf ./.cmake/ ./.qt/ ./CMakeFiles/ ./Attorney_Online_autogen/ ./Testing/
    rm -f ./CMakeCache.txt ./cmake_install.cmake ./CTestTestfile.cmake ./build.ninja ./.ninja_deps ./.ninja_log
    rm -rf ./test/CMakeFiles/ ./test/test_aopacket_autogen/
    rm -f ./test/CTestTestfile.cmake ./test/cmake_install.cmake ./test/test_aopacket
}

configure() {
    if [ "$#" -gt 0 ] && { [ "$1" = "-h" ] || [ "$1" = "--help" ]; }; then
        print_help
        exit 0
    fi

    if [ "$#" -gt 0 ] && [ "$1" = "clean" ]; then
        clean
        exit 0
    fi

    echo "Platform: ${PLATFORM}"

    if [ "$PLATFORM" == "unknown" ]; then
        echo "Unknown platform. Aborting."
        exit 1
    fi

    # Only 'clean' and '-h' are arguments (handled above); configuration comes
    # from the environment.
    if [ "$#" -gt 0 ]; then
        echo "Unknown argument: $1"
        echo "(Configuration such as BUILD_CONFIG is read from the environment, not arguments.)"
        print_help
        exit 1
    fi

    install_build_tools

    # Resolve the Qt toolchain dir (env value wins, else auto-detect under ~/Qt).
    # QT_ROOT is its grandparent, where Tools/ lives, so find_cmake / find_mingw
    # / find_ninja look there.
    if [ -n "$QT_ROOT_DIR" ]; then
        if [ ! -d "$QT_ROOT_DIR" ]; then
            echo "$QT_ROOT_DIR is not a directory. Aborting."
            exit 1
        fi
        QT_ROOT="$(cd "$QT_ROOT_DIR/../.." && pwd)"
    else
        QT_ROOT=$(find_qt)
        if [ -z "$QT_ROOT" ]; then
            echo "Qt not found. Aborting."; exit 1;
        fi
        QT_ROOT_DIR=$(find_qtpath)
        if [ -z "$QT_ROOT_DIR" ] || [ ! -d "$QT_ROOT_DIR" ]; then
            echo "No Qt >= ${QT_MIN_VERSION} found under ${QT_ROOT}. Aborting."
            exit 1
        fi
    fi
    echo "Using Qt root: $QT_ROOT"
    echo "Using Qt installation: $QT_ROOT_DIR"

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

    check_command curl --help || { echo "Command curl not found. Aborting"; exit 1; }
    check_command unzip --help || { echo "Command unzip not found. Aborting"; exit 1; }

    mkdir -p ./tmp/
    mkdir -p ./lib/
    mkdir -p ./bin/

    get_bass
    get_bassopus
    get_discordrpc
    get_qtapng
    get_themes

    # Discord RPC only ships x86_64 binaries, so turn it off on arm64.
    EXTRA_CMAKE_FLAGS=""
    if [[ "$ARCH" != "x86_64" ]]; then
        EXTRA_CMAKE_FLAGS="-DAO_ENABLE_DISCORD_RPC=OFF"
    fi

    FULL_CMAKE_CMD="\
$CMAKE . \
-G Ninja \
-DCMAKE_MAKE_PROGRAM=${NINJA} \
-DCMAKE_PREFIX_PATH=${QT_ROOT_DIR} \
-DCMAKE_MODULE_PATH=${SCRIPT_DIR}/cmake \
-DCMAKE_BUILD_TYPE=${BUILD_CONFIG} \
-DCMAKE_C_COMPILER=${CC} \
-DCMAKE_CXX_COMPILER=${CXX} \
${EXTRA_CMAKE_FLAGS}"

    $FULL_CMAKE_CMD

    # Record the resolved environment so the build step and packaging scripts
    # can reuse it without re-running detection.
    {
        echo "PLATFORM=\"${PLATFORM}\""
        echo "ARCH=\"${ARCH}\""
        echo "QT_ROOT_DIR=\"${QT_ROOT_DIR}\""
        echo "NINJA=\"${NINJA}\""
        echo "CMAKE=\"${CMAKE}\""
        echo "BUILD_CONFIG=\"${BUILD_CONFIG}\""
        echo "MINGW_PATH=\"${MINGW_PATH:-}\""
    } > build.env

    echo "$FULL_CMAKE_CMD" > cmake_cmd.txt

    # configure.sh stops at a ready-to-build tree; building and packaging are
    # separate steps.
    echo
    echo "Dependencies fetched and build files generated (build type: ${BUILD_CONFIG})."
    echo "To compile, run:"
    echo
    echo "${NINJA}"
}

configure "$@"
