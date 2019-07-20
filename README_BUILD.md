### Dependencies

This program has five main dependencies

* Qt 5.6+ (https://www.qt.io/download)
* BASS (http://www.un4seen.com/bass.html)
* BASS Opus Plugin (http://www.un4seen.com/bass.html#addons)
* Discord Rich Presence (https://github.com/discordapp/discord-rpc/releases)
* Qt Apng Plugin (https://github.com/Skycoder42/QtApng/releases)

### Help

If you're having issues with any of this, ask in the offical Discord: https://discord.gg/wWvQ3pw
Alternatively, you can ask OmniTroid#4004 on Discord.

### How to build dynamically (the easy way)

#### General preparation

What you want to do is first download the latest version of Qt from the first link. (get the prebuilt dynamic version)
After going through the OS-specific steps below, compiling in Qt creator should work.

#### Windows

If you're on Windows, you need to go find all the dependencies (see above) and put them in the lib/ folder.

#### MacOS

If you're on MacOS, you can simply go to terminal and run ./scripts/configure_macos.sh
This will automatically fetch all the required dependencies. Additionally, if you need to create a standalone release, just run ./scripts/release_macos.sh
This will make the .app bundle in bin/ able to execute as a standalone.

#### Ubuntu

If you're on Ubuntu, just go to terminal and run ./scripts/configure_ubuntu.sh
This should fetch all the required dependencies automatically.  

#### Other Linux

With some tweaks to the ubuntu script, it shouldn't be a big hassle to compile it on a modern linux. Look in the script and see what you may have to modify.

### How to build statically (the hard way)

You're gonna have a bad time.
  
Building statically means you can distribute the final program without needing to pack alongside a lot of dynamic libraries.  
This is a tricky process and is not recommended unless you know what you're doing.  

First, you need to build the entirety of Qt statically. To do this, check the desired version under "Sources" in the Qt maintenance tool.  
After this is done, follow these instructions: https://dimitris.apeiro.gr/2015/06/24/build-a-static-qt5-for-windows-by-compiling/  
This guide is specifically for Windows, but Qt is cross-platform and you should be good with or without some minor adjustments to the guide.  
Note that this **is** a computationally heavy process and will take a lot of time on a slow computer.  
After this, you need to configure the project to use the static version of qmake (see http://doc.qt.io/qtcreator/creator-build-settings.html)

BASS and BASS Opus only comes as dynamic libraries and is not open-source. That means you can't link it statically in the program.  

For Discord RPC, you can download prebuilt static libraries for your platform. Just put them in lib/.

For Qt Apng Plugin, you need to compile it statically. This guide does not go into detail how to configure this to build a static library. You may have to add Q_IMPORT_PLUGIN(ApngImagePlugin); in main.cpp. See more about importing static plugins here: http://doc.qt.io/qt-5/plugins-howto.html#static-plugins
It's also possible to just link this dynamically.

