FROM oldmud0/mxe-qt:5.13.0-win32-static-posix
#FROM fffaraz/qt:windows

ENV TARGET_SPEC i686-w64-mingw32.static.posix

# Build Discord RPC statically
RUN git clone https://github.com/discordapp/discord-rpc
WORKDIR discord-rpc/build
RUN /opt/mxe/usr/bin/${TARGET_SPEC}-cmake .. -DCMAKE_INSTALL_PREFIX=/opt/mxe/usr/${TARGET_SPEC}
RUN /opt/mxe/usr/bin/${TARGET_SPEC}-cmake --build . --config Release --target install
WORKDIR ../..

# NOTE: Do not build QtApng statically! libpng contains a self-test entry point that
# takes precedence for some reason over the final build's entry point.
