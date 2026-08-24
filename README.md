# Attorney Online

![CI build](https://github.com/AttorneyOnline/AO2-Client/actions/workflows/build.yml/badge.svg?event=push) ![license](https://img.shields.io/github/license/AttorneyOnline/AO2-Client?color=blue) ![contributors](https://img.shields.io/github/contributors/AttorneyOnline/AO2-Client)<br>

[Attorney Online](https://aceattorneyonline.com) is a world-renowned courtroom drama simulator that allows you to create and play out cases in an off-the-cuff format.

**[Refer to the docs](https://github.com/AttorneyOnline/docs/blob/master/docs/index.md) for more information.**

## Setting up for development

### Prerequisites

A C/C++ toolchain:

Ubuntu: `sudo apt install build-essential`  
macOS: `xcode-select --install`

Qt **6.5 or newer**, installed via the [Qt online installer](https://doc.qt.io/qt-6/qt-online-installation.html).
Check off the following under "Additional Libraries":
- Qt Image Formats
- Qt WebSockets

On Windows, also check off a toolchain (MinGW), CMake, and Ninja under
"Developer and Designer tools".

### Setup

Run `./configure.sh`. It detects Qt, installs any other build tools it needs,
fetches the remaining dependencies, and generates the build files, then prints
the command to compile the program.

### Content

To test key features, you may need the vanilla base content, which is not included in this repository. 
You can get it from https://ao-dl.b-cdn.net/vanilla_full_2024_8_2.zip

This should be put in `./bin/base`

## Formatting

All code must be formatted according to the `.clang-format` file. CI runs a
`clang-format` check (version 17) and fails the build if any file under `src/`
is not formatted correctly. Format your changes before pushing:

```sh
clang-format -i src/<file>   # format a specific file
git clang-format             # format only the lines you changed
```

## Running Tests

Tests are written with [Qt Test](https://doc.qt.io/qt-6/qtest-overview.html) and
registered with CTest. After running `./configure.sh` and building, run them from
the repo root:

```sh
ctest --output-on-failure

# or run a single test binary directly, e.g.
./test/test_aopacket
```

## Credits

The original Attorney Online client was created by FanatSors.

This is an open-source remake of that client created by OmniTroid.

The logo (`logo.png` and `logo.ico`) was designed by Lucas Carbí. The characters depicted in the logo are owned by Capcom.

## Copyright

The project is licensed under the GPLv3 license.
Code that was released under the MIT/GPLv3 dual-license can be found [here](https://github.com/AttorneyOnline/AO2-Client/tree/last-dual-licensed).

Copyright (c) 2016-2018 OmniTroid

Modifications copyright (c) 2017-2018 oldmud0

Case Café additions copyright (c) 2018 Cerapter

Killing Fever Online additions copyright (c) 2019 Crystalwarrior

## Contact

You can find us in the official Attorney Online Discord server: https://discord.gg/wWvQ3pw
