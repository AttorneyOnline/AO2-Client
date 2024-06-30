# Attorney Online

![CI build](https://github.com/AttorneyOnline/AO2-Client/actions/workflows/build.yml/badge.svg?event=push) ![license](https://img.shields.io/github/license/AttorneyOnline/AO2-Client?color=blue) ![contributors](https://img.shields.io/github/contributors/AttorneyOnline/AO2-Client)<br>

[Attorney Online](https://aceattorneyonline.com) is a world-renowned courtroom drama simulator that allows you to create and play out cases in an off-the-cuff format.

**[Refer to the docs](https://github.com/AttorneyOnline/docs/blob/master/docs/index.md) for more information.**

## Setting up for development

This program's main dependency is Qt and the currently recommended version for development is **6.5.3**. See [this link](https://doc.qt.io/qt-6/qt-online-installation.html)
on how to install Qt.

### Other dependencies

* BASS (http://www.un4seen.com/bass.html)
* BASS Opus Plugin (http://www.un4seen.com/bass.html#addons)
* BASS Midi Plugin (http://www.un4seen.com/bass.html#addons)
* Discord Rich Presence (https://github.com/discordapp/discord-rpc/releases)
* Qt Apng Plugin (https://github.com/jurplel/QtApng/releases)

(see .github/workflows/build.yml for exact installation commands)

## Running Tests
Running tests requires Catch2 and cmake

```sh
mkdir cbuild && cd cbuild
cmake ..
make test

# usage: run all tests
./test/test

# usage: Optionally specify tests and success verbosity
./test/test [bass] --success
```

`[noci]` tag is used to disable a test on GitHub actions


## Credits

The original Attorney Online client was created by FanatSors.

This is an open-source remake of that client created by OmniTroid.

The logo (`logo.png` and `logo.ico`) was designed by Lucas Carbí. The characters depicted in the logo are owned by Capcom.

## Copyright

The project is dual-licensed; you are free to copy, modify and distribute AO2 under the GPLv3 or the MIT license.

Copyright (c) 2016-2018 OmniTroid

Modifications copyright (c) 2017-2018 oldmud0

Case Café additions copyright (c) 2018 Cerapter

Killing Fever Online additions copyright (c) 2019 Crystalwarrior

## Contact

You can find us in the official Attorney Online Discord server: https://discord.gg/wWvQ3pw
