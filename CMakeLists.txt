# Configure cmake
cmake_minimum_required(VERSION 3.1.0)
cmake_policy(SET CMP0076 NEW) # silence warning

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
add_executable(Attorney_Online resources.qrc)

# Target Include
target_include_directories(Attorney_Online PRIVATE include)

# Target Lib
find_package(Qt5 COMPONENTS Core Gui Network Widgets REQUIRED)
target_link_directories(Attorney_Online PRIVATE lib)
target_link_libraries(Attorney_Online PRIVATE Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets
    bass bassopus discord-rpc)

# Subdirectories
add_subdirectory(test)
add_subdirectory(src)
add_subdirectory(include)
