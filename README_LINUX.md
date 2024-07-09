## Running on Linux

There are two download options for running on Linux: the **dynamically-linked** build and the **AppImage**. The dynamic build is lighter, but might only run on newer systems. The AppImage is a bit bigger, but should run seamlessly on most systems (anything newer than Ubuntu 22.04 LTS).

### AppImage

If you downloaded the **AppImage** version, it should just be plug-and-play. If you run into errors or bugs, contact us in our [Discord server](https://discord.gg/wWvQ3pw) or open an [issue on GitHub](https://github.com/AttorneyOnline/AO2-Client/issues).

### Dynamic

If you downloaded the **dynamically-linked** version, use the `launch.sh` script to run AO.

You may need to install some libraries in your system. These are the commands to run on a terminal for some distributions:

* Arch Linux:
```
$ sudo pacman -S qt6-base qt6-tools qt6-websockets qt6-imageformats
```
* Fedora:
```
$ sudo dnf install qt6-qtbase qt6-qttools qt6-qtwebsockets qt6-qtimageformats
```
* Ubuntu 22.04 LTS:
```
$ sudo apt-get install qt6base-dev libqt6uitools6 libqt6websockets6 qt6-image-formats-plugins
```
