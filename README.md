# Attorney Online 2

[Attorney Online](https://aceattorneyonline.com) is an online version of the world-renowned courtroom drama simulator that allows you to create and play out cases in an off-the-cuff format.

## Introduction for beginners

You may already be familiar with roleplaying in forums, Roll20, and/or [AAO](http://aaonline.fr/) (the online casemaker). In this sense, Attorney Online is nothing more than a medium - an animated chatroom client - that allows cases to be played out as if it were an Ace Attorney game.

Not unlike other roleplaying games, cases can last an absurd amount of time (between 4 to 6 hours) and generally follow a roleplaying format directed by a case sheet.

An implied expectation for fast typing and real-time communication may seem daunting at first, but due to the number of people in the courtroom, things get hectic very quickly even with only a few people talking. Therefore, you should not feel pressured to talk constantly: only when you have the attention of the court (or when you have an objection to make) should you feel the need to speak.

It is recommended, but not strictly necessary, to have played an Ace Attorney game before creating your own case. You should also try to spectate or take part in a community case in order to get a grasp of how cases are done in practice.

---

## Basic features

### In-character chat

Type in a message in the gray box under the viewport, select an emote, and press Enter.

### Emotes

An emote represents a set of animations played while the character is speaking and idle. Some emotes also contain a preanimation, which is played before the text is said by the character.

### Interjections (shouts)

Select an interjection to toggle it. When you send a message, it will interrupt all other dialogue and interject with your message.

### Out-of-character chat

This is a general-purpose chat isolated within areas to discuss matters without interrupting cases. You must enter a name before chatting.

### Music list

Double-click a track to play it. Some servers automatically loop the track. Green tracks are available locally; red tracks are not.

### Areas

Servers have multiple areas to hold multiple cases simultaneously. Double-click an area in the music list to switch to it. (The reason that
areas are in the music list is a historical one.)

### Judge controls

The judge can set health bars and play the Witness Testimony, Cross Examination, Guilty, and Not Guilty animations.

### Mod calls

Calling a mod notifies moderators currently in the server of an incident. (Mod call reasons require 2.6+ server-side support.) Logged-in moderators can toggle the Guard option to be notified of mod calls.

### Muting

Click on a character in the mute list to ignore any in-character chat from the specified character.

### Positions

All characters have a default position within the courtroom, but they can nonetheless be changed within the interface.

Available positions:

- `def` - Defense
- `pro` - Prosecution
- `hld` - Helper defense
- `hlp` - Helper prosecution
- `jud` - Judge
- `wit` - Witness
- `jur` - Juror (2.6+)
- `sea` - Seance (2.6+)

## Advanced features

### Markup language

2.6.0 introduces a markup language for in-character chat. It does not require server-side support.

#### Color

Wrapping text with these characters will set the text inside of them to the associated color.

- `(` and `)` (parentheses) - blue
- \` (backtick) - green
- `|` (vertical bar) - orange
- `[` and `]` (square brackets) - grey

#### Speed

Type `{` to slow down the text a bit, and `}` to speed it up. This takes effect after the character has been typed, so the text may take up different speeds at different points. Both of these can be stacked up to three times, and even against each other.

Example:
```
Hello there! This text goes at normal speed.} Now, it's a bit faster!{ Now, it's back to normal.}}} Now it goes at maximum speed! {{Now it's only a little bit faster than normal.
```

#### Position

If you begin a message with `~~` (two tildes), the two tildes are removed and the message is centered.

### Pairing (2.6+)

If two players are in the same position and select each other's characters using the in-game pair list (or with `/pair [id]`), they will appear alongside each other. You can set the offset of your character using the provided spinbox (or with `/offset [percentage]`).

### Non-interrupting preanimations (2.6+)

When checked, this will force text to immediately begin displaying without waiting for the preanimation to finish.

### Custom IC names (shownames) (2.6+)

You can set a custom in-character name using the provided text box. An option in the interface (or `/force_nonint_pres`) is also present to disable custom IC names for other players to prevent impersonation.

### Extended area support (2.6+)

Areas can be listed by clicking the A/M button (or `/switch_am`). The statuses of such areas are displayed (and updated automatically) if the server has 2.6+ support.

---

## Upgrade guide for 2.6

2.6 inherits features from the Case Café custom client and server. Old themes and servers will still work, but they will not expose the new additions to players.

### Server

2.6 support has only been developed for tsuserver3. serverD is currently not equipped at all for such support.

- Apply the new code changes.
- In `areas.yaml`:
	- You may add `shouts_allowed` to any of the areas to enable / disable shouts (and judge buttons, and realisation). By default, it's `shouts_allowed: true`.
	- You may add `jukebox` to any of the areas to enable the jukebox in there, but you can also use `/jukebox_toggle` in game as a mod to do the same thing. By default, it's `jukebox: false`.
	- You may add `showname_changes_allowed` to any of the areas to allow custom shownames used in there. If it's forbidden, players can't send messages or change music as long as they have a custom name set. By default, it's `showname_changes_allowed: false`.
	- You may add `abbreviation` to override the server-generated abbreviation of the area. Instead of area numbers, this server-pack uses area abbreviations in server messages for easier understanding (but still uses area IDs in commands, of course). No default here, but here is an example: `abbreviation: SIN` gives the area the abbreviation of 'SIN'.
	- You may add `noninterrupting_pres` to force users to use non-interrupting pres only. 2.6 users will see the pres play as the text goes; pre-2.6 users will not see pres at all. The default is `noninterrupting_pres: false`.

### Client themes

- You'll need the following, additional images:
	- `notguilty.gif` - Not Guilty verdict animation
	- `guilty.gif` - Guilty verdict animation
	- `notguilty.png` - Not Guilty button
	- `guilty.png` - Guilty button
	- `pair_button.png` - Pair button
	- `pair_button_pressed.png` - Pair button (selected)
- In your `courtroom_sounds.ini`:
	- Add a sound effect for `not_guilty`, for example: `not_guilty = sfx-notguilty.wav`.
	- Add a sound effect for `guilty`, for example: `guilty = sfx-guilty.wav`.
	- Add a sound effect for the case alerts. They work similarly to modcall alerts, or callword alerts. For example: `case_call = sfx-triplegavel-soj.wav`.
- In your `courtroom_design.ini`, place the following new UI elements as desired:
	- `log_limit_label`, which is a simple text that explains what the spinbox with the numbers is. Needs an X, Y, width, height number.
	- `log_limit_spinbox`, which is the spinbox for the log limit, allowing you to set the size of the log limit in-game. Needs the same stuff as above.
	- `ic_chat_name`, which is an input field for your custom showname. Needs the same stuff.
	- `ao2_ic_chat_name`, which is the same as above, but comes into play when the background has a desk.
		- Further comments on this: all `ao2_` UI elements come into play when the background has a desk. However, in AO2 nowadays, it's customary for every background to have a desk, even if it's just an empty gif. So you most likely have never seen the `ao2_`-less UI elements ever come into play, unless someone mis-named a desk or something.
	- `showname_enable` is a tickbox that toggles whether you should see shownames or not. This does not influence whether you can USE custom shownames or not, so you can have it off, while still showing a custom showname to everyone else. Needs X, Y, width, height as usual.
	- `settings` is a plain button that takes up the OS's looks, like the 'Call mod' button. Takes the same arguments as above.
		- You can also just type `/settings` in OOC.
	- `char_search` is a text input box on the character selection screen, which allows you to filter characters down to name. Needs the same arguments.
	- `char_passworded` is a tickbox, that when ticked, shows all passworded characters on the character selection screen. Needs the same as above.
	- `char_taken` is another tickbox, that does the same, but for characters that are taken.
	- `not_guilty` is a button similar to the CE / WT buttons, that if pressed, plays the Not Guilty verdict animation. Needs the same arguments.
	- `guilty` is similar to `not_guilty`, but for the Guilty verdict.
	- `pair_button` is a toggleable button, that shows and hides the pairing list and the offset spinbox. Works similarly to the mute button.
	- `pair_list` is a list of all characters in alphabetical order, shown when the user presses the Pair button. If a character is clicked on it, it is selected as the character the user wants to pair up with.
	- `pair_offset_spinbox` is a spinbox that allows the user to choose between offsets of -100% to 100%.
	- `switch_area_music` is a button with the text 'A/M', that toggles between the music list and the areas list. Though the two are different, they are programmed to take the same space.
	- `pre_no_interrupt` is a tickbox with the text 'No Intrpt', that toggles whether preanimations should delay the text or not.
	- `area_free_color` is a combination of red, green, and blue values ranging from 0 to 255. This determines the colour of the area in the Area list if it's free, and has a status of `IDLE`.
	- `area_lfp_color` determines the colour of the area if its status is `LOOKING-FOR-PLAYERS`.
	- `area_casing_color` determines the colour of the area if its status is `CASING`.
	- `area_recess_color` determines the colour of the area if its status is `RECESS`.
	- `area_rp_color` determines the colour of the area if its status is `RP`.
	- `area_gaming_color` determines the colour of the area if its status is `GAMING`.
	- `area_locked_color` determines the colour of the area if it is locked, regardless of status.
	- `ooc_default_color` determines the colour of the username in the OOC chat if the message doesn't come from the server.
	- `ooc_server_color` determines the colour of the username if the message arrived from the server.
	- `casing_button` is a button with the text 'Casing' that when clicked, brings up the Case Announcements dialog. You can give the case a name, and tick whom do you want to alert. You need to be a CM for it to go through. Only people who have at least one of the roles ticked will get the alert.
	- `casing` is a tickbox with the text 'Casing'. If ticked, you will get the case announcements alerts you should get, in accordance to the above. In the settings, you can change your defaults on the 'Casing' tab. (That's a buncha things titled 'Casing'!)

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

### Qt

This project uses Qt 5, which is licensed under the [GNU Lesser General Public License](https://www.gnu.org/licenses/lgpl-3.0.txt) with [certain licensing restrictions and exceptions](https://www.qt.io/qt-licensing-terms/). To comply with licensing requirements for static linking, object code is available if you would like to relink with an alternative version of Qt, and the source code for Qt may be found at https://github.com/qt/qtbase, http://code.qt.io/cgit/, or at https://qt.io.

Copyright (c) 2016 The Qt Company Ltd.

### BASS

This project depends on the BASS shared library. Get it here: http://www.un4seen.com/

Copyright (c) 1999-2016 Un4seen Developments Ltd. All rights reserved.
