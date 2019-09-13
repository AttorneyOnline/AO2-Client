#!/bin/sh

# This script fetches all build dependencies for MacOS
# Tested on MacOS 10.14 (Mojave), Qt 5.13 and XCode 10.2

# Exit on errors and unset variables
set -eu

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/"

cd ${ROOT_DIR}

LIB_TARGET="../../lib"
BASS_LINK="http://uk.un4seen.com/files/bass24-osx.zip"
BASSOPUS_LINK="http://www.un4seen.com/files/bassopus24-osx.zip"
DISCORD_RPC_LINK="https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-osx.zip"
APNG_LINK="https://github.com/Skycoder42/QtApng/releases/download/1.1.2-2/qtapng_clang_64_5.13.0.tar.xz"

# Easier if we don't need to worry about an existing tmp folder tbh smh
#      v Add a slash here for free tmp folder cleanup in true javascript community style
rm -rf tmp
mkdir tmp
cd tmp

curl -Ls ${BASS_LINK} -o bass.zip
unzip -qq bass.zip
cp libbass.dylib ${LIB_TARGET}

curl -Ls ${BASSOPUS_LINK} -o bassopus.zip
unzip -qq bassopus.zip
cp libbassopus.dylib ${LIB_TARGET}

curl -Ls ${DISCORD_RPC_LINK} -o discord_rpc.zip
unzip -qq discord_rpc.zip
cp discord-rpc/osx-dynamic/lib/libdiscord-rpc.dylib ${LIB_TARGET}

curl -Ls ${APNG_LINK} -o apng.tar.xz
tar -xf apng.tar.xz
cp clang_64/plugins/imageformats/libqapng.dylib ../../lib

cd ..
rm -rf tmp
