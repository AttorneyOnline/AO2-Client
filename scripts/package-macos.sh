#!/bin/bash

# Packages the macOS build into a self-contained, distributable .app + zip.
#
# Prerequisites: run ./configure.sh (fetches deps, generates build files, writes
# build.env) and then build (run the command configure.sh prints). This script
# assembles bin/"Attorney Online.app" and zips it into dist/.

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
ROOT_DIR=$(cd "${SCRIPT_DIR}/.." && pwd)
cd "${ROOT_DIR}"

if [ -f ./build.env ]; then
    # shellcheck disable=SC1091
    source ./build.env
fi
: "${QT_ROOT_DIR:?QT_ROOT_DIR not set, run ./configure.sh first (it writes build.env)}"

APP="./bin/Attorney Online.app"
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

# macdeployqt and the copies above rewrite Mach-O files, invalidating their
# signatures; without a valid one Gatekeeper calls the app "damaged", so re-sign
# ad-hoc inside-out. Not notarized, so a downloaded copy is still quarantined:
#   xattr -dr com.apple.quarantine "Attorney Online.app"
codesign --force --sign - "${APP}/Contents/MacOS/"*.dylib 2>/dev/null || true
codesign --force --sign - "${APP}/Contents/PlugIns/imageformats/"*.dylib 2>/dev/null || true
codesign --force --deep --sign - "$APP"

# Package the self-contained app into a single, checksummable zip named for
# platform/arch/commit (base is bundled inside the app, icon in Contents/Resources,
# so nothing ships alongside). -y preserves macdeployqt's framework symlinks.
sha="${GITHUB_SHA:-}"; sha="${sha:0:8}"
[ -z "$sha" ] && sha="$(git rev-parse --short=8 HEAD 2>/dev/null || echo dev)"
ZIP="AttorneyOnline-${PLATFORM}-${ARCH}-${sha}.zip"
mkdir -p dist
( cd ./bin && zip -r -y "../dist/${ZIP}" "Attorney Online.app" )

echo "Wrote ${ROOT_DIR}/dist/${ZIP}"
