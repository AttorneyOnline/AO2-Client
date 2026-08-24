#!/bin/bash

# Packages the Linux build into two distributables at the repo root:
#   - Attorney_Online-Dynamic.tar : dynamic build (expects system Qt at runtime)
#   - Attorney_Online-AppImage.tar : self-contained AppImage
#
# Prerequisites: run ./configure.sh (fetches deps, generates build files, writes
# build.env) and then build (run the command configure.sh prints).

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
ROOT_DIR=$(cd "${SCRIPT_DIR}/.." && pwd)
cd "${ROOT_DIR}"

if [ -f ./build.env ]; then
    # shellcheck disable=SC1091
    source ./build.env
fi
: "${QT_ROOT_DIR:?QT_ROOT_DIR not set, run ./configure.sh first (it writes build.env)}"

if [ ! -f ./bin/Attorney_Online ]; then
    echo "Error: ./bin/Attorney_Online not found. Build first with the command configure.sh printed."
    exit 1
fi

# Stage the APNG plugin into the Qt install so appimagetool bundles it.
mkdir -p "${QT_ROOT_DIR}/plugins/imageformats"
cp ./qtapng/plugins/imageformats/libqapng.so "${QT_ROOT_DIR}/plugins/imageformats/"

# --- Dynamic tarball (expects system Qt at runtime) ---
(
    cd ./bin
    cp ../data/logo-client.png ./icon.png
    cp ../README_LINUX.md .
    cp ../scripts/DYNAMIC_INSTALL.sh ./INSTALL.sh
    chmod +x INSTALL.sh Attorney_Online
    patchelf --add-rpath . Attorney_Online
)
tar --transform='flags=r;s|bin|Attorney Online|' -cvf Attorney_Online-Dynamic.tar bin

# --- AppImage (self-contained) ---
# from https://github.com/probonopd/go-appimage/blob/master/src/appimagetool/README.md
wget -c "https://github.com/$(wget -q https://github.com/probonopd/go-appimage/releases/expanded_assets/continuous -O - | grep "appimagetool-.*-x86_64.AppImage" | head -n 1 | cut -d '"' -f 2)"
mv appimagetool-*-x86_64.AppImage appimagetool
chmod +x appimagetool

mkdir -p AppDir/usr/bin AppDir/usr/lib/plugins/imageformats AppDir/usr/share/applications
cp bin/Attorney_Online AppDir/usr/bin
cp bin/lib*.so AppDir/usr/lib 2>/dev/null || true
cp scripts/Attorney_Online.desktop AppDir/usr/share/applications
cp data/logo-client.png AppDir/Attorney_Online.png

# Prefer the CI-provided commit SHA; fall back to git (or "dev") for local runs.
GIT_SHORT_SHA="${GITHUB_SHA:-}"
GIT_SHORT_SHA="${GIT_SHORT_SHA:0:8}"
if [ -z "$GIT_SHORT_SHA" ]; then
    GIT_SHORT_SHA="$(git rev-parse --short=8 HEAD 2>/dev/null || echo dev)"
fi
QTDIR="$QT_ROOT_DIR" ./appimagetool deploy AppDir/usr/share/applications/Attorney_Online.desktop
ARCH=x86_64 VERSION="${GIT_SHORT_SHA}" ./appimagetool AppDir

# --- AppImage tarball ---
rm -rf bin-appimage
mkdir bin-appimage
cp -r bin/base bin-appimage
cp data/logo-client.png bin-appimage/icon.png
cp README_LINUX.md bin-appimage
cp scripts/APPIMAGE_INSTALL.sh bin-appimage/INSTALL.sh
cp Attorney_Online-*-x86_64.AppImage bin-appimage
chmod +x bin-appimage/INSTALL.sh bin-appimage/Attorney_Online-*-x86_64.AppImage
tar --transform='flags=r;s|bin-appimage|Attorney Online|' -cvf Attorney_Online-AppImage.tar bin-appimage

echo "Wrote ${ROOT_DIR}/Attorney_Online-Dynamic.tar and ${ROOT_DIR}/Attorney_Online-AppImage.tar"
