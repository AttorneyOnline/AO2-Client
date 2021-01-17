#!/bin/bash

# Assumes a somewhat recent 64-bit ubuntu

# Exit on errors and unset variables
set -eu

ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/"

cd "${ROOT_DIR}"


#need some openGL stuff
sudo apt install libgl1-mesa-dev
#install curl incase of fresh vm
sudo apt install curl
mkdir tmp

cd tmp

#get the bass prebuilt
curl http://www.un4seen.com/files/bass24-linux.zip -o bass_linux.zip
curl http://www.un4seen.com/files/bassopus24-linux.zip -o bassopus_linux.zip

unzip bass_linux.zip
unzip bassopus_linux.zip

cp x64/libbass.so ../../lib
cp x64/libbassopus.so ../../lib

#get the discord-rpc prebuilt
curl -L https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-linux.zip -o discord_rpc_linux.zip

unzip discord_rpc_linux.zip

cp discord-rpc/linux-dynamic/lib/libdiscord-rpc.so ../../lib/

#get libqtapng plugin prebuilt
curl -L https://github.com/Skycoder42/QtApng/releases/download/1.1.0-5/build_gcc_64_5.12.0.tar.xz -o apng.tar.xz

tar -xvf apng.tar.xz

cp gcc_64/plugins/imageformats/libqapng.so ../../lib

cd ..

rm -rf tmp

#during runtime, the program expects the libraries to exist in the root folder
cp ../lib/* ..
