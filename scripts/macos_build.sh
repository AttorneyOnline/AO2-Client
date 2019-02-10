#!/bin/sh

set -Eexo pipefail

export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig:/usr/local/opt/openssl/lib/pkgconfig
export PATH=$PATH:/usr/local/opt/qt5/bin:/usr/local/bin

mkdir bass
cd bass
curl http://www.un4seen.com/files/bass24-osx.zip -o bass.zip
unzip bass.zip
cp libbass.dylib ../lib
cd ..

mkdir discord-rpc
cd discord-rpc
curl -L https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-osx.zip -o discord_rpc_osx.zip
unzip discord_rpc_osx.zip
cp discord-rpc/osx-static/lib/libdiscord-rpc.a ../lib
cd ..

mkdir qtapng
cd qtapng
curl -L https://github.com/Skycoder42/QtApng/releases/download/1.1.0-5/build_clang_64_5.12.0.tar.xz -o apng.tar.xz
tar -xvf apng.tar.xz
cp clang_64/plugins/imageformats/libqapng.dylib ../lib
cd ..

qmake && make -j2
