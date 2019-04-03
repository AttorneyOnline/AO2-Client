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
    metavar: ["<incremental zip file>", "<file containing list of deleted files>"]
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

const [incrementalZipFile, deletionsFile] = incrementalArgs || [];
const [fullZipFile] = fullZipFileArgs || [];
const [executable] = executableArgs || [];

// Do one final check
if (!incrementalZipFile && !fullZipFile) {
    console.error("No download archive specified! Abort.");
    process.exit(1);
}

const manifest = JSON.parse(fs.readFileSync(manifestFile));

const deleteActions = deletionsFile ? fs.readFileSync(deletionsFile)
    .split("\n").map(file => {
        // XXX: This does not delete empty directories. Finding them would
        // actually be a substantial amount of work because Git does not
        // give us a good way of finding directories that were deleted.
        return { action: "delete", target: file };
    }) : [];

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
        ...deleteActions,
        {
            action: "dl",
            url: urlBase + encodeURIComponent(path.basename(incrementalZipFile)),
            hash: crypto.createHash("sha1")
                .update(fs.readFileSync(incrementalZipFile))
                .digest("hex")
        }
    ] : undefined
};

const existingVersion = manifest.versions.filter(v => v.version == version);
if (existingVersion) {
    console.warn(`Warning: version ${version} already exists. Adding new values.`);

    // Don't overwrite prev - it will cause headaches
    delete versionEntry.prev;

    Object.assign(existingVersion, versionEntry);
} else {
    manifest.versions = [versionEntry, ...manifest.versions];
}

fs.writeFileSync(manifestFile, JSON.stringify(manifest, null, 4));
