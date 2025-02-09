#!/bin/bash
# Required to launch correctly
#please note that this is the APPIMAGE version of this bash script, they are not cross compatible, and one will not work for the other

# Move to AO's directory
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}"

#put into user's local library
mkdir ~/.local/Attorney_Online
mv -f {.*,*} ~/.local/Attorney_Online

#add .desktop file (which should allow most DE's easy access to the program
mkdir ~/.local/share/applications
rm ~/.local/share/applications/'Attorney Online'.desktop
touch ~/.local/share/applications/'Attorney Online'.desktop
#please note that the best way forward here is to have the real logo in path, but this is a temporary logo, easily replaceable.
printf "[Desktop Entry]\nType=Application\nName=Attorney Online\nComment=The courtroom drama simulator\nExec=$(realpath ~/.local/Attorney_Online/Attorney_Online-*-x86_64.AppImage)\nIcon=$(realpath ~/.local/Attorney_Online)/base/themes/AOHD/placeholder.webp\nCategories=Game;" >> ~/.local/share/applications/'Attorney Online'.desktop

#marking the program as executable
chmod +x ~/.local/Attorney_Online/Attorney_Online-*-x86_64.AppImage
#running the executable
~/.local/Attorney_Online/Attorney_Online-*-x86_64.AppImage
