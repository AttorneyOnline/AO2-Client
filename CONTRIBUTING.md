## Compiling

The traditional route is by undergoing the [AO2 Rite of Passage](https://gist.github.com/oldmud0/6c645bd1667370c3e92686f7d0642c38).

However, these days it is easy to get away with using the default Qt toolchain on any platform, which creates a dynamic executable. (Don't forget to invoke windeployqt when creating a release.)

### Dependencies

- [BASS](http://un4seen.com) (optional; may use Qt Multimedia instead)
- [Discord Rich Presence](https://github.com/discordapp/discord-rpc) (optional)

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
