#!/usr/bin/env node

const fs = require("fs");
const crypto = require("crypto");

const [ _nodeExe, _jsPath, manifestFile, version, zipFile ] = process.argv;

if (!manifestFile || !version || !zipFile) {
    console.log(`Usage: update_program_manifest <manifest file> <version> <zip file>`);
    console.log(`Adds a new latest version to the manifest file based on the ` +
                `provided zip file.`);
    process.exit(1);
}

if (!fs.existsSync(manifestFile)) {
    console.error(`Manifest file '${manifestFile}' not found. Try again.`);
    process.exit(2);
}

if (!fs.existsSync(zipFile)) {
    console.error(`Zip file '${zipFile}' not found. Try again.`);
    process.exit(2);
}

const manifest = JSON.parse(fs.readFileSync(manifestFile));

manifest.versions = [{
    version,
    executable: "Attorney_Online.exe",
    full: [
        {
            action: "dl",
            url: "https://s3.wasabisys.com/ao-downloads/" + encodeURIComponent(zipFile),
            hash: crypto.createHash("sha1").update(fs.readFileSync(zipFile)).digest("hex")
        }
    ]
}, ...manifest.versions];

fs.writeFileSync(manifestFile, JSON.stringify(manifest, null, 4));