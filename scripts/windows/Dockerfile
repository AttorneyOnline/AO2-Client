FROM oldmud0/mxe-qt:5.12.1-win32-static-posix
#FROM fffaraz/qt:windows

ENV TARGET_SPEC i686-w64-mingw32.static.posix

# Build Discord RPC statically
RUN git clone https://github.com/discordapp/discord-rpc
WORKDIR discord-rpc/build
RUN /opt/mxe/usr/bin/${TARGET_SPEC}-cmake .. -DCMAKE_INSTALL_PREFIX=/opt/mxe/usr/${TARGET_SPEC}
RUN /opt/mxe/usr/bin/${TARGET_SPEC}-cmake --build . --config Release --target install
WORKDIR ../..

# Build QtApng statically
RUN git clone https://github.com/Skycoder42/QtApng
WORKDIR QtApng
RUN /opt/mxe/usr/${TARGET_SPEC}/qt5/bin/qmake && make qmake_all && make && make install
WORKDIR ..
