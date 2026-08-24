## Attorney Online for Linux (dynamic build)

This is the **dynamically-linked** build. It's lighter than the AppImage but uses
your system's Qt, so it may only run on newer distributions.

### Running

Run the binary directly:

```
./AttorneyOnline.bin
```

Or run `./install.sh` to add Attorney Online to your app launcher — it creates a
desktop entry pointing to this folder. Note that moving this folder requires
running `install.sh` again.

### Dependencies

You may need to install Qt libraries first. Commands for some distributions:

#### Arch Linux
```
sudo pacman -S qt6-base qt6-tools qt6-websockets qt6-imageformats
```
#### Fedora
```
sudo dnf install qt6-qtbase qt6-qttools qt6-qtwebsockets qt6-qtimageformats
```
#### Ubuntu 22.04 LTS
```
sudo apt-get install qt6base-dev libqt6uitools6 libqt6websockets6 qt6-image-formats-plugins
```

If you run into errors or bugs, reach us in our
[Discord server](https://discord.gg/wWvQ3pw) or open an
[issue on GitHub](https://github.com/AttorneyOnline/AO2-Client/issues).
