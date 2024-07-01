#!/bin/sh

# Required in CI to launch correctly
chmod +x Attorney_Online
LD_LIBRARY_PATH=. ./Attorney_Online
