#!/bin/bash

# Packages the macOS build into a self-contained, distributable .app + zip.
#
# Prerequisites: run ./configure.sh (fetches deps, generates build files, writes
# build.env) and then build (run the command configure.sh prints). This script
# assembles bin/Attorney_Online.app and zips it alongside the base/ assets.

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
ROOT_DIR=$(cd "${SCRIPT_DIR}/.." && pwd)
cd "${ROOT_DIR}"

if [ -f ./build.env ]; then
    # shellcheck disable=SC1091
    source ./build.env
fi
: "${QT_ROOT_DIR:?QT_ROOT_DIR not set, run ./configure.sh first (it writes build.env)}"

APP="./bin/Attorney_Online.app"
if [ ! -d "$APP" ]; then
    echo "Error: ${APP} not found. Build first with the command configure.sh printed."
    exit 1
fi

echo "Bundling dependencies into ${APP}..."

# BASS (and BASSOPUS/Discord if present) are loaded via @loader_path, i.e.
# relative to the executable, so they go beside it in Contents/MacOS.
cp ./lib/libbass.dylib ./lib/libbassopus.dylib "${APP}/Contents/MacOS/"
if [ -f ./lib/libdiscord-rpc.dylib ]; then
    cp ./lib/libdiscord-rpc.dylib "${APP}/Contents/MacOS/"
fi

# The APNG image plugin must live under the bundle's PlugIns dir before
# macdeployqt runs, so its Qt references get rewritten along with the rest.
mkdir -p "${APP}/Contents/PlugIns/imageformats"
cp ./bin/imageformats/libqapng.dylib "${APP}/Contents/PlugIns/imageformats/"

"${QT_ROOT_DIR}/bin/macdeployqt" "$APP" -verbose=1

# Ship the self-contained app plus the base/ assets, matching the layout of the
# Windows/Linux artifacts. -y preserves the framework symlinks macdeployqt makes.
cp ./data/logo-client.png ./bin/icon.png
( cd ./bin && zip -r -y ../Attorney_Online-macOS.zip Attorney_Online.app base icon.png )

echo "Wrote ${ROOT_DIR}/Attorney_Online-macOS.zip"
