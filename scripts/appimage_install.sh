#!/usr/bin/env sh

# exit on error
set -e

# Move to script's directory
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
cd "${SCRIPT_DIR}"

#add .desktop file (which should allow most DE's easy access to the program
mkdir -p ~/.local/share/applications

# desktop files don't like spaces in the Exec field, we have to replace them with "\s"
appimage="AttorneyOnline.AppImage"
escaped_exec="$(echo "$(pwd)/$appimage" | sed 's/ /\\s/g')"

desktop_file="\
[Desktop Entry]
Type=Application
Name=Attorney Online
Comment=The courtroom drama simulator
Path=$(pwd)
Exec=\"$escaped_exec\"
Icon=$(pwd)/icon.png
StartupWMClass=AttorneyOnline"

echo "$desktop_file" > ~/.local/share/applications/AttorneyOnline.desktop

#marking the program as executable
chmod +x "$appimage"

echo "Attorney Online has been added to your app menu."
echo "You can also run it directly with: ./$appimage"
