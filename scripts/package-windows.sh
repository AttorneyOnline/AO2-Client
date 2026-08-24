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

EXE="./bin/Attorney Online.exe"
if [ ! -f "$EXE" ]; then
    echo "Error: ${EXE} not found. Build first with the command configure.sh printed."
    exit 1
fi

echo "Running windeployqt on ${EXE}..."
"${QT_ROOT_DIR}/bin/windeployqt.exe" \
    --no-quick-import --no-translations --no-compiler-runtime --no-opengl-sw "$EXE"

# needed becauce windeployqt is a bit unreliable with MINGW
if [ -n "${MINGW_PATH:-}" ]; then
    for dll in libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll; do
        cp "${MINGW_PATH}/bin/${dll}" ./bin/
    done
fi

# Package bin/ into one zip
sha="${GITHUB_SHA:-}"; sha="${sha:0:8}"
[ -z "$sha" ] && sha="$(git rev-parse --short=8 HEAD 2>/dev/null || echo dev)"
ZIP="AttorneyOnline-${PLATFORM}-${ARCH}-${sha}.zip"
mkdir -p dist
pwsh -NoProfile -Command "Compress-Archive -Path (Get-ChildItem -Path bin -Exclude '.gitignore').FullName -DestinationPath 'dist/${ZIP}' -Force"

echo "Wrote ${ROOT_DIR}/dist/${ZIP}"
