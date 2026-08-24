#!/bin/bash

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

cp ./lib/libbass.dylib ./lib/libbassopus.dylib "${APP}/Contents/MacOS/"
if [ -f ./lib/libdiscord-rpc.dylib ]; then
    cp ./lib/libdiscord-rpc.dylib "${APP}/Contents/MacOS/"
fi

mkdir -p "${APP}/Contents/PlugIns/imageformats"
cp ./bin/imageformats/libqapng.dylib "${APP}/Contents/PlugIns/imageformats/"

"${QT_ROOT_DIR}/bin/macdeployqt" "$APP" -verbose=1

# macdeployqt invalidates signatures, we need to fix that here or else macOS will call the app damaged and refuse to run
# note that it still needs to be manually approved in Privacy & Security but oh well
codesign --force --sign - "${APP}/Contents/MacOS/"*.dylib 2>/dev/null || true
codesign --force --sign - "${APP}/Contents/PlugIns/imageformats/"*.dylib 2>/dev/null || true
codesign --force --deep --sign - "$APP"

sha="${GITHUB_SHA:-}"; sha="${sha:0:8}"
[ -z "$sha" ] && sha="$(git rev-parse --short=8 HEAD 2>/dev/null || echo dev)"
ZIP="AttorneyOnline-${PLATFORM}-${ARCH}-${sha}.zip"
mkdir -p dist
( cd ./bin && zip -r -y "../dist/${ZIP}" "Attorney Online.app" )

echo "Wrote ${ROOT_DIR}/dist/${ZIP}"
