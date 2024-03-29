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

rm -rf tmp
mkdir tmp
cd tmp

#get the bass prebuilt
curl http://www.un4seen.com/files/bass24-linux.zip -o bass_linux.zip
curl http://www.un4seen.com/files/bassmidi24-linux.zip -o bassmidi_linux.zip
curl http://www.un4seen.com/files/bassopus24-linux.zip -o bassopus_linux.zip

mkdir -p bass_linux
unzip bass_linux.zip -d ./bass_linux
mkdir -p bassmidi_linux
unzip bassmidi_linux.zip -d ./bassmidi_linux
mkdir -p bassopus_linux
unzip bassopus_linux.zip -d ./bassopus_linux

cp ./bass_linux/libs/x86_64/libbass.so ../../lib
cp ./bassmidi_linux/libs/x86_64/libbassmidi.so ../../lib
cp ./bassopus_linux/libs/x86_64/libbassopus.so ../../lib

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
