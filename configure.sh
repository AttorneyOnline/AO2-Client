#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}" || { echo "Failed to cd to pwd"; exit 1; }

# Detect platform
detect_platform() {
    unameOut="$(uname -s)"
    case "${unameOut}" in
        Linux*)     platform=Linux;;
        Darwin*)    platform=MacOS;;
        CYGWIN*|MINGW*|MSYS*) platform=Windows;;
        *)          platform="UNKNOWN:${unameOut}"
    esac
    echo "${platform}"
}

check_command() {
    COMMAND=$1
    if ! command -v "${COMMAND}" &> /dev/null; then
        echo "${COMMAND} not found. Aborting."
        exit 1
    fi
}

print_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help: Print this help message"
}

install_discordrcp_windows() {
    curl -L https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-win.zip -o discord_rpc.zip
    unzip discord_rpc.zip
    cp ./discord-rpc/win64-dynamic/lib/discord-rpc.lib ./lib/
    cp ./discord-rpc/win64-dynamic/bin/discord-rpc.dll ./bin/
    cp ./discord-rpc/win64-dynamic/include/discord*.h ./lib/
}

check_command cmake
check_command curl

platform=$(detect_platform)
echo "Detected platform: ${platform}"
