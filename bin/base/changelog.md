# Luna 1.0.0 RC1
**Attorney Online Golden** is the next iteration branched off from *Attorney Online 2.10.1* and a new continuation for the client.
Since AO2 is no longer actively developed, we aim to provide users and communities a more user-friendly experience with new features and Quality of Life additions, as well as full retro-compatibility with older and existing content from AO2.
## The Full Changelog from 2.10.1
There's been an increasingly ammount of changes moving on from 2.10.1. It started in the original conception of this client (originally known as AO Triplex), all until Luna 1.0, the former first official release.
Changes will be separated in three categories for easier redaction: **Primary**, **Secondary** and **Tertiary**.
### Primary changes
- **Triple Pairing**: Users are now able to pair with 2 more people on screen *if* the server supports it.
- **Emote Queue**: Ever wanted to change your emote mid-sentence? Right-clicking on the emote button you want, you'll find the "Add to Emote Queue" option. It's a quick method to change them on the fly.
- **Typing icon**: A small pencil icon featuring other people's character icons will appear on the corner of the viewport indicating when they're typing. If you *don't* want others to know when you're typing or you don't want to see them, you can disable this in Settings.
- **DL Manager and Download.ini**: Users and servers alike are now able to share Download Links for everyone to see, showing up in the *DL Manager* tab in Settings. Servers can share official links with server content or updates, and users are now able to create a __download.ini__ inside their character's folder of choice with a given download URL.
- **Auto-Character Downloader**: If an *Asset URL* is available in a server, you can directly download missing content from the Character Selection screen. Just click a character you're missing and choose "Download"!
- **Menu Bar Implementation**: *Luna 1.0* contains its first prototype, and right now there are four tabs available: Main, Character, Roleplay and Quick-Swapping. The first three contain shortcuts for buttons and a special addition for download.ini's creation. The fourth will be explained below.
- **Quick Swapping Tab ~ Character Sets**: Tired of having to scroll through dozens of entries in your iniswap dropdown to find the one you're looking for? Managing an RP feels annoying when you constantly need to swap to different characters and there's no organization whatsoever? With **character sets**, you'll be able to define (in a .ini file) categories for each characters you want for easier and faster character swap. Quick-Swapping supports custom .ini files, meaning you can load a custom set containing unique categories. *You can also tear off the QSwapping tab and have it as a floating window.*
- **Character Streaming**: While only supporting PNG in the first AOG release, with *character streaming* (aka. Asset Streaming) you will now be able to see other people's characters even if you don't have them downloaded. This feature is currently unstable, so crashes are expected to happen.
### Secondary changes
- **Auto-Additive**: A *Quality of Life* addition for faster Additive. Starting your message with a blankspace will automatically continue your last message where you left it off.
- **Hide & Narrate button**: If you ever wanted a shortcut to narrate or hide your character, now you can! These toggles are now included in the Character tab from the menu bar for quick access.
- **Clock Feature Support**: Fully compatible with DRO. To use it, create a "hours/" folder in the theme you wish to use and set its position adding "rp_clock = x, y, width, height" in your courtroom_design.ini. By default, it is masked to the viewport.
- **Keyboard Shortcuts**: Reload Theme [CTRL + R] and Change Character [CTRL + T] now have a shortcut associated.
- **Emote Buttons Shortcut**: A new QoL addition is that [Alt + Left Key / Right Key] will select your next or previous button automatically. [Alt + Down Key / Up Key] will go one page forward or one page back as well.
### Tertiary changes
- **Reconnect Button**: Whenever you're disconnected from the server, you are now able to reconnect with the same character you left.
- **Lobby Remake**: Lobby is now remade with a more modern look. It also contains a *Changelog* tab, something older versions were missing. Wasn't officially announced before in 2.10.1, but the lobby is resizeable as well.
- **Restored Demo's Load Button**: An old feature gone in 2.10.1, now given a proper button.
- **Crossfade**: A Visual Novel-like effect when changing between different sprites. You can toggle it in Settings.
- **Background Packet Expansion**: *Custom overlays* are no longer hardcoded and were given a proper network implementation. The server can now send a background with a custom overlay included.