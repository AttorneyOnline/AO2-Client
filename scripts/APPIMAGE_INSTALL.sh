#!/bin/bash

# Move to script's directory
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}"

#add .desktop file (which should allow most DE's easy access to the program
mkdir -p ~/.local/share/applications
#please note that the best way forward here is to have the real logo in path, but this is a temporary logo, easily replaceable.
desktop_file="
[Desktop Entry]
Type=Application
Name=Attorney Online
Comment=The courtroom drama simulator
Exec=$(realpath $(pwd)/Attorney_Online-*-x86_64.AppImage)
Icon=$(pwd)/base/themes/AOHD/placeholder.webp"

echo "$desktop_file" > ~/.local/share/applications/'Attorney Online'.desktop

#marking the program as executable
chmod +x Attorney_Online-*-x86_64.AppImage

#running the executable
./Attorney_Online-*-x86_64.AppImage
