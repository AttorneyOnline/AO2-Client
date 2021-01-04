# Configure cmake
cmake_minimum_required(VERSION 3.1.0)

project(AttorneyOnline)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# AO
add_executable(Attorney_Online
# resources
resources.qrc

# src
src/aoapplication.cpp
src/aoblipplayer.cpp
src/aobutton.cpp
src/aocaseannouncerdialog.cpp
src/aocharbutton.cpp
src/aocharmovie.cpp
src/aoemotebutton.cpp
src/aoevidencebutton.cpp
src/aoevidencedisplay.cpp
src/aoimage.cpp
src/aolineedit.cpp
src/aomovie.cpp
src/aomusicplayer.cpp
src/aooptionsdialog.cpp
src/aopacket.cpp
src/aoscene.cpp
src/aosfxplayer.cpp
src/aotextarea.cpp
src/aotextedit.cpp
src/charselect.cpp
src/chatlogpiece.cpp
src/courtroom.cpp
src/debug_functions.cpp
src/discord_rich_presence.cpp
src/emotes.cpp
src/evidence.cpp
src/file_functions.cpp
src/hardware_functions.cpp
src/lobby.cpp
src/main.cpp
src/misc_functions.cpp
src/networkmanager.cpp
src/packet_distribution.cpp
src/path_functions.cpp
src/scrolltext.cpp
src/text_file_functions.cpp

# include
include/aoapplication.h
include/aoblipplayer.h
include/aobutton.h
include/aocaseannouncerdialog.h
include/aocharbutton.h
include/aocharmovie.h
include/aoemotebutton.h
include/aoevidencebutton.h
include/aoevidencedisplay.h
include/aoimage.h
include/aolineedit.h
include/aomovie.h
include/aomusicplayer.h
include/aooptionsdialog.h
include/aopacket.h
include/aoscene.h
include/aosfxplayer.h
include/aotextarea.h
include/aotextedit.h
include/bass.h
include/bassopus.h
include/chatlogpiece.h
include/courtroom.h
include/datatypes.h
include/debug_functions.h
include/discord-rpc.h
include/discord_register.h
include/discord_rich_presence.h
include/discord_rpc.h
include/file_functions.h
include/hardware_functions.h
include/lobby.h
include/misc_functions.h
include/networkmanager.h
include/scrolltext.h
include/text_file_functions.h
)

# Target Include
target_include_directories(Attorney_Online PRIVATE include)

# Target Lib
find_package(Qt5 COMPONENTS Core Gui Network Widgets REQUIRED)
target_link_directories(Attorney_Online PRIVATE lib)
target_link_libraries(Attorney_Online PRIVATE Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets
    bass bassopus discord-rpc)

# Tests
add_subdirectory(test)
