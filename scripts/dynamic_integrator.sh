#!/bin/bash
# Required to launch correctly

# Move to AO's directory
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}"

#put into user's local library
mkdir ~/.local/Attorney_Online
mv -fu {.*,*} ~/.local/Attorney_Online

#add .desktop file (which should allow most DE's easy access to the program
mkdir ~/.local/share/applications
rm ~/.local/share/applications/'Attorney Online'.desktop
touch ~/.local/share/applications/'Attorney Online'.desktop
#please note that the best way forward here is to have the real logo in path, but this is a temporary logo, easily replaceable.
printf "[Desktop Entry]\nType=Application\nName=Attorney Online\nComment=The courtroom drama simulator\nExec=$(realpath ~/.local/Attorney_Online)/launch.sh\nIcon=$(realpath ~/.local/Attorney_Online)/base/themes/AOHD/placeholder.webp\nCategories=Game;" >> ~/.local/share/applications/'Attorney Online'.desktop

#making the launch.sh (probably not necessary, but if we removed launch.sh and replaced it with this, it makes the path clear without any mud from installing)
rm ~/.local/Attorney_Online/launch.sh
touch ~/.local/Attorney_Online/launch.sh
printf "#!/bin/bash\ncd $(realpath ~/.local/Attorney_Online)\nchmod +x $(realpath ~/.local/Attorney_Online/Attorney_Online)\nLD_LIBRARY_PATH=. $(realpath ~/.local/Attorney_Online/Attorney_Online)" >> ~/.local/Attorney_Online/launch.sh

#running the program
chmod +x ~/.local/Attorney_Online/launch.sh
~/.local/Attorney_Online/launch.sh
