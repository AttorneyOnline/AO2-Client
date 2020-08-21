#!/usr/bin/env node

const fs = require("fs");
const crypto = require("crypto");
const path = require("path");
const ArgumentParser = require("argparse").ArgumentParser;

function isFile(file) {
    if (!fs.existsSync(file)) {
        console.error(`File '${file}' not found. Try again.`);
        throw Error();
    }
    return file;
}

const argParser = new ArgumentParser({
    addHelp: true,
    description: "Adds a new latest version to the manifest file based on the " +
        "provided zip file, including an incremental update."
});
argParser.addArgument("manifestFile", {
    metavar: "<manifest file>", type: isFile
});
argParser.addArgument("version", {
    metavar: "<version>"
});
argParser.addArgument([ "-f", "--full" ], {
    metavar: "<full zip file>", type: isFile, nargs: 1,
    dest: "fullZipFileArgs"
});
argParser.addArgument([ "-i", "--incremental" ], {
    type: isFile, nargs: 2, dest: "incrementalArgs",
    metavar: ["<incremental zip file>", "<file containing list of changed files>"]
});
argParser.addArgument([ "-e", "--executable" ], {
    metavar: "[executable file]", nargs: 1,
    dest: "executableArgs"
});

const {
    manifestFile,
    version,
    fullZipFileArgs,
    incrementalArgs,
    executableArgs
} = argParser.parseArgs();

const [incrementalZipFile, changesFile] = incrementalArgs || [];
const [fullZipFile] = fullZipFileArgs || [];
const [executable] = executableArgs || [];

// Do one final check
if (!incrementalZipFile && !fullZipFile) {
    console.error("No download archive specified! Abort.");
    process.exit(1);
}

// Do a quick litmus test to prevent deleting everything incorrectly
if (changesFile && !fs.existsSync("base")) {
    console.error("The working directory must be set to an " +
                  "asset folder in order for deleted directories " +
                  "to be calculated correctly. Abort.");
    process.exit(1);
}

const manifest = JSON.parse(fs.readFileSync(manifestFile));

const dirsDeleted = new Set();
const specialActions = changesFile ?
    fs.readFileSync(changesFile)
    .toString()
    .trim()
    .split("\n")
    .map(line => line.split("\t"))
    .map(([mode, target, source]) => {
        switch (mode[0]) {
            case "D": // Deleted
                // Check if the folder exists relative to the working
                // directory, and if not, add it to the dirsDeleted list.
                // Keep going up the tree to see how many directories were
                // deleted.
                let dir = path.dirname(target);
                while (!dirsDeleted.has(dir) && !fs.existsSync(dir)) {
                    dirsDeleted.add(dir);
                    dir = path.dirname(dir);
                }

                return { action: "delete", target };
            case "R": // Renamed
                // NOTE: Make sure that the launcher's implementation of
                // the move action also creates directories when needed.
                return { action: "move", source, target};
            default:
                return null;
        }
    })
    // Remove ignored file mode changes
    .filter(action => action !== null)
    // Create actions based on directories to be deleted.
    // Always have deeper directories first, to guarantee that deleting
    // higher-level directories will succeed.
    .concat(Array.from(dirsDeleted.values())
        .sort((a, b) => b.split("/").length - a.split("/").length)
        .map(dir => ({ action: "deleteDir", target: dir })))
    : [];

const urlBase = "https://s3.wasabisys.com/ao-downloads/";

const versionEntry = {
    version,
    executable,
    prev: manifest.versions[0] ? manifest.versions[0].version : undefined,
    full: fullZipFile ? [
        {
            action: "dl",
            url: urlBase + encodeURIComponent(path.basename(fullZipFile)),
            hash: crypto.createHash("sha1")
                .update(fs.readFileSync(fullZipFile))
                .digest("hex")
        }
    ] : undefined,
    update: incrementalArgs ? [
        ...specialActions,
        {
            action: "dl",
            url: urlBase + encodeURIComponent(path.basename(incrementalZipFile)),
            hash: crypto.createHash("sha1")
                .update(fs.readFileSync(incrementalZipFile))
                .digest("hex")
        }
    ] : undefined
};

console.log("Generated version entry:", versionEntry);

const existingVersions = manifest.versions.filter(v => v.version == version);
if (existingVersions.length > 0) {
    console.warn(`Warning: version ${version} already exists. Adding new values.`);

    // Don't overwrite prev - it will cause headaches
    delete versionEntry.prev;

    Object.assign(existingVersions[0], versionEntry);
    console.log("Merged version entry:", existingVersions[0]);
} else {
    manifest.versions = [versionEntry, ...manifest.versions];
}

fs.writeFileSync(manifestFile, JSON.stringify(manifest, null, 4));
