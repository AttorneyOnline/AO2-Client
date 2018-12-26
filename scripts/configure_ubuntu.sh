#!/bin/bash

#assumes a somewhat recent 64-bit ubuntu

#need some openGL stuff
sudo apt install libgl1-mesa-dev

mkdir tmp
cd tmp

#get the bass prebuilt
curl http://www.un4seen.com/files/bass24-linux.zip -o bass_linux.zip

unzip bass_linux.zip

cp x64/libbass.so ../../lib

#get the discord-rpc prebuilt
curl -L https://github.com/discordapp/discord-rpc/releases/download/v3.4.0/discord-rpc-linux.zip -o discord_rpc_linux.zip

unzip discord_rpc_linux.zip

cp discord-rpc/linux-dynamic/lib/libdiscord-rpc.so ../../lib/

cd ..

rm -rf tmp/
