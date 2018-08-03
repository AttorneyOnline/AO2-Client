# Attorney Online 2: Case Café Custom Client (AO2:CCCC)

This project is a custom client made specifically for the Case Café server of Attorney Online 2. Nevertheless, the client itself has a bunch of features that are server independent, and if you so wish to run a server with the additional features, get yourself a copy of `tsuserver3`, and replace its `server/` folder with the one supplied here.

Building the project is... complicated. I'm not even sure what I'm doing myself, most of the time. Still, get yourself Qt Creator, and compile the project using that, that's the easiest method of doing things.

Alternatively, you may wait till I make some stuff, and release a compiled executable. You may find said executables in the 'Tags' submenu to the left.

## Features

- **Inline colouring:** allows you to change the text's colour midway through the text.
  - `()` (parentheses) will make the text inbetween them blue.
  - \` (backwards apostrophes) will make the text green.
  - `|` (straight lines) will make the text orange.
  - `[]` (square brackets) will make the text grey.
  - No need for server support: the clients themselves will interpret these.
- **Additional text features:**
  - Type `{` to slow down the text a bit. This takes effect after the character has been typed, so the text may take up different speeds at different points.
  - Type `}` to do the opposite! Similar rules apply.
  - Both of these can be stacked up to three times, and even against eachother.
  - As an example, here is a text:
  ```
  Hello there! This text goes at normal speed.} Now, it's a bit faster!{ Now, it's back to normal.}}} Now it goes at maximum speed! {{Now it's only a little bit faster than normal.
  ```
  - If you begin a message with `~~` (two tildes), those two tildes will be removed, and your message will be centered.
- **Server-supported features:** These will require the modifications in the `server/` folder applied to the server.
  - Call mod reason: allows you to input a reason for your modcall.
  - Modcalls can be cancelled, if needed.
  - Shouts can be disabled serverside (in the sense that they can still interrupt text, but will not make a sound or make the bubble appear).
  - The characters' shownames can be changed.
    - This needs the server to specifically approve it in areas.
    - The client can also turn off the showing of changed shownames if someone is maliciously impersonating someone.

---

# Attorney-Online-Client-Remake
This is a open-source remake of Attorney Online written by OmniTroid. The original Attorney Online client was written by FanatSors in Delphi.

The logo (`logo.png` and `logo.ico`) was designed by Lucas Carbi. The characters depicted in the logo are owned by Capcom.

## License

The project is dual-licensed; you are free to copy, modify and distribute AO2 under the GPLv3 or the MIT license.

Copyright (c) 2016-2018 David "OmniTroid" Skoland

Modifications copyright (c) 2017-2018 oldmud0

## Qt
This project uses Qt 5, which is licensed under the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.txt) with [certain licensing restrictions and exceptions](https://www.qt.io/qt-licensing-terms/). To comply with licensing requirements for static linking, object code is available if you would like to relink with an alternative version of Qt, and the source code for Qt may be found at https://github.com/qt/qtbase, http://code.qt.io/cgit/, or at https://qt.io.

Copyright (c) 2016 The Qt Company Ltd.

## BASS

This project depends on the BASS shared library. Get it here: http://www.un4seen.com/

Copyright (c) 1999-2016 Un4seen Developments Ltd. All rights reserved.
