FROM oldmud0/mxe-qt:5.13.0-win32-static-posix
#FROM fffaraz/qt:windows

ENV TARGET_SPEC i686-w64-mingw32.static.posix

# Build libarchive statically
WORKDIR /opt/mxe
RUN make -j4 MXE_TARGETS=${TARGET_SPEC} libarchive bzip2 xz lz4 zstd nettle expat libxml2
WORKDIR /

# Build Discord RPC statically
RUN git clone https://github.com/discordapp/discord-rpc
WORKDIR discord-rpc/build
RUN /opt/mxe/usr/bin/${TARGET_SPEC}-cmake .. -DCMAKE_INSTALL_PREFIX=/opt/mxe/usr/${TARGET_SPEC}
RUN /opt/mxe/usr/bin/${TARGET_SPEC}-cmake --build . --config Release --target install
WORKDIR ../..

# Build QtApng statically
RUN git clone https://github.com/Skycoder42/QtApng
WORKDIR QtApng
# libpng contains a self-test entry point that takes precedence for some reason
# over the final build's entry point.
RUN sed -i "s/^main(/libpng_main(/g" src/3rdparty/libpng/src/pngtest.c
RUN /opt/mxe/usr/${TARGET_SPEC}/qt5/bin/qmake
RUN make && make install
WORKDIR ..