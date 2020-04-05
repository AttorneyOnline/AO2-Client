#!/bin/sh
# Updates the specified program manifest to a new archive and version
# and uploads the new archive and manifest to S3/Wasabi.
#
# Requires:
#   MANIFEST: name of the manifest file
#   ARTIFACT_SUFFIX: suffix of the archive to be uploaded (including extension)
#   S3_ACCESS_KEY and S3_SECRET_KEY


# -E: inherit ERR trap by shell functions
# -e: stop script on ERR trap
# -u: stop script on unbound variables
# -x: print command before running it
# -o pipefail: fail if any command in a pipeline fails
set -Eeuxo pipefail

aws configure set aws_access_key_id ${S3_ACCESS_KEY}
aws configure set aws_secret_access_key ${S3_SECRET_KEY}
aws configure set default.region us-east-1

export S3_COPY="aws s3 cp --endpoint-url=https://s3.wasabisys.com"
export S3_MANIFESTS="s3://ao-manifests"
export S3_ARCHIVES="s3://ao-downloads"

export VERSION=$(git describe --tags)
export ARCHIVE="Attorney_Online_${VERSION}_${ARTIFACT_SUFFIX}"

${S3_COPY} ${S3_MANIFESTS}/${MANIFEST} .
node $(dirname $0)/update_manifest.js ${MANIFEST} ${VERSION} \
    -f ${ARCHIVE} -e Attorney_Online.exe
${S3_COPY} ${ARCHIVE} ${S3_ARCHIVES}
${S3_COPY} ${MANIFEST} ${S3_MANIFESTS}

rm -f ${MANIFEST}
