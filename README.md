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
- **Use the in-game settings button:**
	- If the theme supports it, you may have a Settings button on the client now, but you can also just type `/settings` in the OOC.
	- Modify the contents of your `config.ini` and `callwords.ini` from inside the game!
	- Some options may need a restart to take effect.
- **Custom Discord RPC icon and name!**
- **Enhanced character selection screen:**
	- The game preloads the characters' icons available on the server, avoiding lag on page switch this way.
	- As a side-effect of this, characters can now easily be filtered down to name, whether they are passworded, and if they're taken.
- **Server-supported features:** These will require the modifications in the `server/` folder applied to the server.
	- Call mod reason: allows you to input a reason for your modcall.
		- Modcalls can be cancelled, if needed.
	- Shouts can be disabled serverside (in the sense that they can still interrupt text, but will not make a sound or make the bubble appear).
	- The characters' shownames can be changed.
		- This needs the server to specifically approve it in areas.
		- The client can also turn off the showing of changed shownames if someone is maliciously impersonating someone.
	- Any character in the 'jud' position can make a Guilty / Not Guilty text appear with the new button additions.
		- These work like the WT / CE popups.
	- Capitalisation ignored for server commands. `/getarea` is exactly the same as `/GEtAreA`!
	- Various quality-of-life changes for mods, like `/m`, a server-wide mods-only chat.
	- Disallow blankposting using `/allow_blankposting`.
	- Avoid cucking by setting a jukebox using `/jukebox_toggle`.
		- Check the contents of the jukbox with `/jukebox`.
		- If you're a mod or the CM, skip the current jukebox song using `/jukebox_skip`.
	- Pair up with someone else!
		- If two people select eachother's character's character ID using the in-game pair button, or with `/pair [id]`, they will appear on the same screen (assuming they're both in the same position).
		- When you appear alongside someone else, you can offset your character to either side using the in-game spinbox, or with `/offset [percentage]`. The percentage can go from -100% (one whole screen's worth to the left) to 100% (one whole screen's worth to the right).
	- Areas can have multiple CMs, and these CMs can be anywhere on the server!
		- CMs away from the areas they CM in can still see IC and OOC messages coming from there.
		- They can also remotely send messages with the `/a [area_id]` command (works both IC and OOC!) or the `/s` command, if they want to message all areas.
		- A CM can add other CMs using `/cm [id]`.
	- Tired of waiting for pres to finish? Try non-interrupting pres!
		- Tired of waiting for OTHERS' pres to finish? `/force_nonint_pres` that thing!
	- Also tired of filling evidence up one-by-one? Try `/load_case`!
	- Additional juror and seance positions for your RPing / casing needs.
	- Areas can be set to locked and spectatable.
		- Spectatable areas (using `/area_spectate`) allow people to join, but not talk if they're not on the invite list.
		- Locked areas (using `/area_lock`) forbid people not on the invite list from even entering.
	- Can't find people to case with? Try the case alert system!
		- 
- **Area list:**
	- The client automatically filters out areas from music if applicable, and these appear in their own list.
	- Use the in-game A/M button, or the `/switch_am` command to switch between them.
	- If the server supports it, you can even get constant updates about changes in the areas, like players leaving, CMs appearing, statuses changing, etc.
- **Features not mentioned in here?**
	- Check the link given by the `/help` function.
	- Alternatively, assuming you're reading this on the Github page, browse the wiki!

## Modifications that need to be done

Since this custom client, and the server files supplied with it, add a few features not present in Vanilla, some modifications need to be done to ensure that you can use the full extent of them all. These are as follows:

- **In `areas.yaml`:** (assuming you are the server owner)
	- You may add `shouts_allowed` to any of the areas to enable / disable shouts (and judge buttons, and realisation). By default, it's `shouts_allowed: true`.
	- You may add `jukebox` to any of the areas to enable the jukebox in there, but you can also use `/jukebox_toggle` in game as a mod to do the same thing. By default, it's `jukebox: false`.
	- You may add `showname_changes_allowed` to any of the areas to allow custom shownames used in there. If it's forbidden, players can't send messages or change music as long as they have a custom name set. By default, it's `showname_changes_allowed: false`.
	- You may add `abbreviation` to override the server-generated abbreviation of the area. Instead of area numbers, this server-pack uses area abbreviations in server messages for easier understanding (but still uses area IDs in commands, of course). No default here, but here is an example: `abbreviation: SIN` gives the area the abbreviation of 'SIN'.
	- You may add `noninterrupting_pres` to force users to use non-interrupting pres only. CCCC users will see the pres play as the text goes, Vanilla users will not see pres at all. The default is `noninterrupting_pres: false`.
- **In your themes:**
	- You'll need the following, additional images:
		- `notguilty.gif`, which is a gif of the Not Guilty verdict being given.
		- `guilty.gif`, which is a gif of the Guilty verdict being given.
		- `notguilty.png`, which is a static image for the button for the Not Guilty verdict.
		- `guilty.png`, which is a static image for the button for the Guilty verdict.
		- `pair_button.png`, which is a static image for the Pair button, when it isn't pressed.
		- `pair_button_pressed.png`, which is the same, but for when the button is pressed.
	- In your `lobby_design.ini`:
		- Extend the width of the `version` label to a bigger size. Said label now shows both the underlying AO's version, and the custom client's version.
	- In your `courtroom_sounds.ini`:
		- Add a sound effect for `not_guilty`, for example: `not_guilty = sfx-notguilty.wav`.
		- Add a sound effect for `guilty`, for example: `guilty = sfx-guilty.wav`.
		- Add a sound effect for the case alerts. They work similarly to modcall alerts, or callword alerts. For example: `case_call = sfx-triplegavel-soj.wav`.
	- In your `courtroom_design.ini`, place the following new UI elements as and if you wish:
		- `log_limit_label`, which is a simple text that exmplains what the spinbox with the numbers is. Needs an X, Y, width, height number.
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
		- `area_free_color` is a combination of red, green, and blue values ranging from 0 to 255. This determines the colour of the area in the Area list if it's free, and has a status of IDLE.
		- `area_lfp_color` determines the colour of the area if its status is LOOKING-FOR-PLAYERS.
		- `area_casing_color` determines the colour of the area if its status is CASING.
		- `area_recess_color` determines the colour of the area if its status is RECESS.
		- `area_rp_color` determines the colour of the area if its status is RP.
		- `area_gaming_color` determines the colour of the area if its status is GAMING.
		- `area_locked_color` determines the colour of the area if it is locked, REGARDLESS of status.
		- `ooc_default_color` determines the colour of the username in the OOC chat if the message doesn't come from the server.
		- `ooc_server_color` determines the colour of the username if the message arrived from the server.
		- `casing_button` is a button with the text 'Casing' that when clicked, brings up the Case Announcements dialog. You can give the case a name, and tick whom do you want to alert. You need to be a CM for it to go through. Only people who have at least one of the roles ticked will get the alert.
		- `casing` is a tickbox with the text 'Casing'. If ticked, you will get the case announcements alerts you should get, in accordance to the above. In the settings, you can change your defaults on the 'Casing' tab. (That's a buncha things titled 'Casing'!)

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
