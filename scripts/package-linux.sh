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

if [ ! -f "./bin/AttorneyOnline.bin" ]; then
    echo "Error: ./bin/AttorneyOnline.bin not found. Build first with the command configure.sh printed."
    exit 1
fi

mkdir -p "${QT_ROOT_DIR}/plugins/imageformats"
cp ./qtapng/plugins/imageformats/libqapng.so "${QT_ROOT_DIR}/plugins/imageformats/"

### Dynamic build (expects system Qt at runtime)
(
    cd ./bin
    cp ../data/logo-client.png ./icon.png
    cp ../scripts/DYNAMIC_README.md ./README.md
    cp ../scripts/dynamic_install.sh ./install.sh
    chmod +x install.sh AttorneyOnline.bin
    patchelf --add-rpath . AttorneyOnline.bin
)

### AppImage (self-contained)
case "$(uname -m)" in
    aarch64|arm64) APPIMAGE_ARCH=aarch64 ;;
    *)             APPIMAGE_ARCH=x86_64 ;;
esac

wget -c "https://github.com/$(wget -q https://github.com/probonopd/go-appimage/releases/expanded_assets/continuous -O - | grep "appimagetool-.*-${APPIMAGE_ARCH}.AppImage" | head -n 1 | cut -d '"' -f 2)"
mv appimagetool-*-"${APPIMAGE_ARCH}".AppImage appimagetool
chmod +x appimagetool

mkdir -p AppDir/usr/bin AppDir/usr/lib/plugins/imageformats AppDir/usr/share/applications
cp bin/AttorneyOnline.bin AppDir/usr/bin
cp bin/lib*.so AppDir/usr/lib 2>/dev/null || true
cp scripts/AttorneyOnline.desktop AppDir/usr/share/applications
cp data/logo-client.png AppDir/AttorneyOnline.png

GIT_SHORT_SHA="${GITHUB_SHA:-}"
GIT_SHORT_SHA="${GIT_SHORT_SHA:0:8}"
if [ -z "$GIT_SHORT_SHA" ]; then
    GIT_SHORT_SHA="$(git rev-parse --short=8 HEAD 2>/dev/null || echo dev)"
fi
QTDIR="$QT_ROOT_DIR" ./appimagetool deploy AppDir/usr/share/applications/AttorneyOnline.desktop

# go-appimage clobbers permissions, needs fixing
find AppDir -type f -name 'ld-linux-*' -exec chmod +x {} +

ARCH="${APPIMAGE_ARCH}" VERSION="${GIT_SHORT_SHA}" ./appimagetool AppDir

rm -rf bin-appimage
mkdir bin-appimage
cp -r bin/base bin-appimage
cp data/logo-client.png bin-appimage/icon.png
cp scripts/APPIMAGE_README.md bin-appimage/README.md
cp scripts/appimage_install.sh bin-appimage/install.sh
cp ./*-"${APPIMAGE_ARCH}".AppImage "bin-appimage/AttorneyOnline.AppImage"
chmod +x "bin-appimage/install.sh" "bin-appimage/AttorneyOnline.AppImage"

DYN_ZIP="AttorneyOnline-${PLATFORM}dynamic-${ARCH}-${GIT_SHORT_SHA}.zip"
APP_ZIP="AttorneyOnline-${PLATFORM}appimage-${ARCH}-${GIT_SHORT_SHA}.zip"
mkdir -p dist
( cd bin && zip -r -y "../dist/${DYN_ZIP}" . -x '.gitignore' )
( cd bin-appimage && zip -r -y "../dist/${APP_ZIP}" . )

echo "Wrote ${ROOT_DIR}/dist/${DYN_ZIP} and ${ROOT_DIR}/dist/${APP_ZIP}"
