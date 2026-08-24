#!/usr/bin/env sh

set -e

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
cd "${SCRIPT_DIR}"

mkdir -p ~/.local/share/applications

# desktop files don't like spaces in the Exec field, replace with "\s"
escaped_exec="$(echo "$(pwd)/AttorneyOnline.bin" | sed 's/ /\\s/g')"

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

#make the program executable
chmod +x AttorneyOnline.bin

echo "Attorney Online has been added to your app menu."
echo "You can also run it directly with: ./AttorneyOnline.bin"
