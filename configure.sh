#!/bin/bash

set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "${SCRIPT_DIR}" || { echo "Failed to cd to pwd"; exit 1; }

# Detect platform
detect_platform() {
    unameOut="$(uname -s)"
    case "${unameOut}" in
        Linux*)     platform=Linux;;
        Darwin*)    platform=MacOS;;
        CYGWIN*|MINGW*|MSYS*) platform=Windows;;
        *)          platform="UNKNOWN:${unameOut}"
    esac
    echo "${platform}"
}

print_help() {
    echo "Usage: $0 [options]"
    echo "Options:"
    echo "  -h, --help: Print this help message"
}

# PLATFORM=$(detect_platform)
print_help
