# Attorney Online

[Attorney Online](https://aceattorneyonline.com) is an online version of the world-renowned courtroom drama simulator that allows you to create and play out cases in an off-the-cuff format.

## Introduction for beginners

Rerfer to the Wiki for information.
---

## Compiling

The traditional route is by undergoing the [AO2 Rite of Passage](https://gist.github.com/oldmud0/6c645bd1667370c3e92686f7d0642c38). Recently, however, it has become more feasible to get away with a dynamic compilation, which is much easier for beginners and requires less setup.

### Dependencies

- [QtApng](https://github.com/Skycoder42/QtApng)
- [BASS](http://un4seen.com) (proprietary, but will become optional in the future; see #35)
- [Discord Rich Presence](https://github.com/discordapp/discord-rpc)

## Release instructions

Follow these steps to make a new full release:

- Set a new AO version in the `.pro` file and in `aoapplication.h`.
- Compile the project.
- Commit the version bump and and create a tag for the commit.
- Rename the executable to `Attorney_Online`.
- Create a temp directory.
- Copy a fresh `base` folder to the temp dir. Ensure that the timestamps are consistent.
  - Ignore this step if creating a client-only release.
- Copy the repository's `base` folder to the temp dir.
- Append `.sample` to the names of all `.ini` files, including `serverlist.txt`.
- Copy the game executable to the temp dir.
- Copy `bass.dll`, `discord-rpc.dll`, and `qapng.dll` if applicable.
- Copy `README.md` as `README.md.txt` with CRLF line endings.
- Copy `LICENSE` as `LICENSE.txt` with CRLF line endings.
- Compress the contents of the temp dir to an archive with maximum compression, but
  be sure that the contents are placed inside the root directory of the archive and not
  within a subdirectory.
- Compute the SHA-1 hash of the archive.
- Upload the archive to the Wasabi bucket and an additional mirror (e.g. MEGA or OneDrive)
  (if this is a full release).
- Publish a GitHub release and upload the archive there (if this is a client-only release).
- Add the new version to the `program.json` manifest for the respective platform 
  (if this is a client-only release).
- Update the following on the website for the respective platform:
  - Full download links (Wasabi and mirror)
  - Client download link
  - Full download hash
  - Client download hash

Repeat for each platform (currently 32-bit Windows and 64-bit Linux). Once you're done, don't forget to announce your release!

## Credits

This is a open-source remake of Attorney Online written by OmniTroid. The original Attorney Online client was written by FanatSors in Delphi.

The logo (`logo.png` and `logo.ico`) was designed by Lucas Carbi. The characters depicted in the logo are owned by Capcom.

### Project

The project is dual-licensed; you are free to copy, modify and distribute AO2 under the GPLv3 or the MIT license.

Copyright (c) 2016-2018 David "OmniTroid" Skoland

Modifications copyright (c) 2017-2018 oldmud0

Case Café additions copyright (c) 2018 Cerapter

Killing Fever Online additions copyright (c) 2019 Crystalwarrior

### Qt

This project uses Qt 5, which is licensed under the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.txt) with [certain licensing restrictions and exceptions](https://www.qt.io/qt-licensing-terms/). To comply with licensing requirements for static linking, object code is available if you would like to relink with an alternative version of Qt, and the source code for Qt may be found at https://github.com/qt/qtbase, http://code.qt.io/cgit/, or at https://qt.io.

Copyright (c) 2016 The Qt Company Ltd.

### BASS

This project depends on the BASS shared library. Get it here: http://www.un4seen.com/

Copyright (c) 1999-2016 Un4seen Developments Ltd. All rights reserved.
