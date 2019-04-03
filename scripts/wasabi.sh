#!/bin/bash
# Updates the specified program manifest to a new archive and version
# and uploads the new archive and manifest to S3/Wasabi.
#
# Requires:
#   MANIFEST: name of the manifest file
#   S3_ACCESS_KEY, S3_SECRET_KEY: S3 credentials
#   ARCHIVE_FULL: name of the full archive (if desired)
#   ARCHIVE_INCR: name of the incremental archive (if desired)
#   VERSION: name of the new version
#   EXECUTABLE: name of the executable (if program manifest)


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

export ARCHIVE_FULL_ARG=""
export ARCHIVE_INCR_ARG=""
export EXECUTABLE_ARG=""

export LAST_TAGGED_VERSION=$(git rev-list --tags --skip=1 --max-count=1)
echo "Previous tagged version: ${LAST_TAGGED_VERSION}"
echo "Current tagged version: ${VERSION}"

if [[ -n $ARCHIVE_INCR && -n $LAST_TAGGED_VERSION ]]; then
    echo "Incremental archive: ${ARCHIVE_INCR}"

    # Get deleted files
    export DELETIONS_FILE="deletions.txt"
    git log --diff-filter=D --summary ${LAST_TAGGED_VERSION}..HEAD | \
        grep "delete mode" | cut -d' ' -f 5- > ${DELETIONS_FILE}

    # Get added/modified files
    git log --name-only --oneline --diff-filter=d ec5c..HEAD | \
        grep -v -E "^[0-9a-f]{7} " | sort -u | zip ${ARCHIVE_INCR} -@

    export ARCHIVE_INCR_ARG="-i ${ARCHIVE_INCR} ${DELETIONS_FILE}"
elif [[ -n $ARCHIVE_INCR && -z $LAST_TAGGED_VERSION ]]; then
    echo "Incremental archive was requested, but there is no previous version"
fi

if [[ -n $ARCHIVE_FULL ]]; then
    echo "Full archive: ${ARCHIVE_INCR}"
    export ARCHIVE_FULL_ARG="-f ${ARCHIVE_FULL}"
fi

if [[ -v $EXECUTABLE ]]; then
    export EXECUTABLE_ARG="-e ${EXECUTABLE}"
fi

${S3_COPY} ${S3_MANIFESTS}/${MANIFEST} .
node $(dirname $0)/update_manifest.js ${MANIFEST} ${VERSION} \
    ${ARCHIVE_FULL_ARG} ${ARCHIVE_INCR_ARG} ${EXECUTABLE_ARG}

if [[ -n $ARCHIVE_INCR_ARG ]]; then
    ${S3_COPY} ${ARCHIVE_INCR} ${S3_ARCHIVES}
fi

if [[ -n $ARCHIVE_FULL_ARG ]]; then
    ${S3_COPY} ${ARCHIVE_FULL} ${S3_ARCHIVES}
fi

${S3_COPY} ${MANIFEST} ${S3_MANIFESTS}
