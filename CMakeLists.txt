cmake_minimum_required(VERSION 3.1.0)
cmake_policy(SET CMP0076 NEW) # regarding target_sources() pathing

project(Attorney_Online
	VERSION 2.9.0.0
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

find_package(Qt5
    COMPONENTS Gui Multimedia Network UiTools
    REQUIRED)

string(CONCAT CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} "
    "-Os -ffunction-sections -fdata-sections -fvisibility=hidden -Wl,--gc-sections"
)
string(CONCAT CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} "
    "--enable-stdcall-fixup"
)

# TODO add RC file for win32 icon
add_executable(Attorney_Online
    resources.qrc
)
target_include_directories(Attorney_Online
    PRIVATE include)

find_library(BASS bass
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

find_library(DISCORD_RPC discord-rpc
    HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_link_libraries(Attorney_Online
    Qt5::Gui
    Qt5::Multimedia
    Qt5::Network
    Qt5::UiTools
    ${BASS}
    ${DISCORD_RPC}
)

add_subdirectory(src)
add_subdirectory(include)