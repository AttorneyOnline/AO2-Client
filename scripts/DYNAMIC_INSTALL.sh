#!/bin/bash

# exit on error
set -e

# Move to script's directory
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}"

#add .desktop file (which should allow most DE's easy access to the program
mkdir -p ~/.local/share/applications

# desktop files don't like spaces in the Exec field, we have to replace them with "\s"
escaped_exec="$(echo "$(pwd)" | sed 's/ /\\s/g')"/Attorney_Online

desktop_file="\
[Desktop Entry]
Type=Application
Name=Attorney Online
Comment=The courtroom drama simulator
Path=$(pwd)
Exec=\"$escaped_exec\"
Icon=$(pwd)/icon.png"

echo "$desktop_file" > ~/.local/share/applications/'Attorney Online'.desktop

#running the program
chmod +x Attorney_Online
./Attorney_Online
