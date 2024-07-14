#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}" || { echo "Failed to cd to pwd"; exit 1; }

print_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help: Print this help message"
}

check_command() {
    COMMAND=$1
    if ! command -v "${COMMAND}" &> /dev/null; then
        echo "${COMMAND} not found. Aborting."
        exit 1
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
    wget -O "$tmp_zip" "$url"
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
  get_zip http://www.un4seen.com/files/bass24.zip \
    c/bass.h:./lib/bass.h \
    c/x64/bass.lib:./lib/bass.lib \
    x64/bass.dll:./bin/bass.dll

  # Bass Opus
  get_zip http://www.un4seen.com/files/bassopus24.zip \
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
  get_zip http://localhost:8000/bass24-linux.zip \
    bass.h:./lib/bass.h \
    libs/x86_64/libbass.so:./lib/libbass.so \
    libs/x86_64/libbass.so:./bin/libbass.so

  # Bass Opus
  # TODO

  # Discord RPC
  # TODO

  # APng
  # TODO
}

configure_macos() {
  # Bass
  get_zip http://localhost:8000/bass24-osx.zip \
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
  if [ "$#" -eq 1 ] && { [ "$1" = "-h" ] || [ "$1" = "--help" ]; }; then
    print_help
    exit 0
  fi

  # Check required commands
  check_command cmake
  check_command wget
  check_command unzip

  # Make sure key folders exist
  mkdir -p ./tmp/
  mkdir -p ./lib/
  mkdir -p ./bin/

  # Detect platform and do platform-specific conf
  unameOut="$(uname -s)"
  case "${unameOut}" in
    CYGWIN*|MINGW*|MSYS*) configure_windows;;
    Linux*)     configure_linux;;
    Darwin*)    configure_macos;;
    *)          echo "Unsupported platform: ${unameOut}"; exit 1;
  esac
}

configure "$@"
