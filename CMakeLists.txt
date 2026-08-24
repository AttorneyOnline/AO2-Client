cmake_minimum_required(VERSION 3.7.0)

project(AttorneyOnline VERSION 2.11.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(AO_BUILD_TESTS "Build test programs" ON)
option(AO_ENABLE_DISCORD_RPC "Enable Discord Rich Presence" ON)

find_package(QT NAMES Qt6)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets Concurrent WebSockets UiTools)

qt_add_executable(Attorney_Online
  src/aoapplication.cpp
  src/aoapplication.h
  src/aoblipplayer.cpp
  src/aoblipplayer.h
  src/aobutton.cpp
  src/aobutton.h
  src/aocharbutton.cpp
  src/aocharbutton.h
  src/aoclocklabel.cpp
  src/aoclocklabel.h
  src/aoemotebutton.cpp
  src/aoemotebutton.h
  src/aoemotepreview.cpp
  src/aoemotepreview.h
  src/aoevidencebutton.cpp
  src/aoevidencebutton.h
  src/aoevidencedisplay.cpp
  src/aoevidencedisplay.h
  src/aoimage.cpp
  src/aoimage.h
  src/animationlayer.cpp
  src/animationlayer.h
  src/animationloader.h
  src/animationloader.cpp
  src/aomusicplayer.cpp
  src/aomusicplayer.h
  src/aopacket.cpp
  src/aopacket.h
  src/aosfxplayer.cpp
  src/aosfxplayer.h
  src/aotextarea.cpp
  src/aotextarea.h
  src/aotextboxwidgets.cpp
  src/aotextboxwidgets.h
  src/aoutils.cpp
  src/aoutils.h
  src/charselect.cpp
  src/chatlogpiece.cpp
  src/chatlogpiece.h
  src/courtroom.cpp
  src/courtroom.h
  src/datatypes.h
  src/debug_functions.cpp
  src/debug_functions.h
  src/demoserver.cpp
  src/demoserver.h
  src/discord_rich_presence.cpp
  src/discord_rich_presence.h
  src/emotes.cpp
  src/eventfilters.cpp
  src/eventfilters.h
  src/evidence.cpp
  src/file_functions.cpp
  src/file_functions.h
  src/gui_utils.h
  src/hardware_functions.cpp
  src/hardware_functions.h
  src/lobby.cpp
  src/lobby.h
  src/main.cpp
  src/network/websocketconnection.cpp
  src/network/websocketconnection.h
  src/networkmanager.cpp
  src/networkmanager.h
  src/options.cpp
  src/options.h
  src/packet_distribution.cpp
  src/path_functions.cpp
  src/scrolltext.cpp
  src/scrolltext.h
  src/serverdata.cpp
  src/serverdata.h
  src/text_file_functions.cpp
  src/widgets/aooptionsdialog.cpp
  src/widgets/aooptionsdialog.h
  src/widgets/direct_connect_dialog.cpp
  src/widgets/direct_connect_dialog.h
  src/widgets/server_editor_dialog.cpp
  src/widgets/server_editor_dialog.h
  data.qrc
  src/widgets/playerlistwidget.h src/widgets/playerlistwidget.cpp
  src/widgets/moderator_dialog.h src/widgets/moderator_dialog.cpp
  src/screenslidetimer.h src/screenslidetimer.cpp
  src/moderation_functions.h src/moderation_functions.cpp
  src/network/serverinfo.h src/network/serverinfo.cpp
)

set_target_properties(Attorney_Online PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if(WIN32)
  set_target_properties(Attorney_Online PROPERTIES OUTPUT_NAME "Attorney Online")
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET Attorney_Online PROPERTY WIN32_EXECUTABLE true)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/data/logo-client.rc")
    target_sources(Attorney_Online PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
  endif()
endif()

target_include_directories(Attorney_Online PRIVATE src lib)
target_link_directories(Attorney_Online PRIVATE lib)
target_link_libraries(Attorney_Online PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Concurrent
  Qt${QT_VERSION_MAJOR}::WebSockets
  Qt${QT_VERSION_MAJOR}::UiTools
  bass
  bassopus
)

if(AO_ENABLE_DISCORD_RPC)
  target_compile_definitions(Attorney_Online PRIVATE AO_ENABLE_DISCORD_RPC)
  target_link_libraries(Attorney_Online PRIVATE discord-rpc)
endif()

if(AO_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

set_target_properties(Attorney_Online PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>
        RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_LIST_DIR}/bin>)

if(APPLE)
  set(MACOSX_BUNDLE_ICON "${CMAKE_CURRENT_LIST_DIR}/data/logo-client.icns")
  target_sources(Attorney_Online PRIVATE ${MACOSX_BUNDLE_ICON})
  set_source_files_properties(${MACOSX_BUNDLE_ICON} PROPERTIES
          MACOSX_PACKAGE_LOCATION Resources)

  set_target_properties(Attorney_Online PROPERTIES
          OUTPUT_NAME "Attorney Online"
          MACOSX_BUNDLE TRUE
          MACOSX_BUNDLE_BUNDLE_NAME "Attorney Online"
          MACOSX_BUNDLE_GUI_IDENTIFIER "org.attorneyonline.Attorney_Online"
          MACOSX_BUNDLE_BUNDLE_VERSION "2"
          MACOSX_BUNDLE_SHORT_VERSION_STRING "2.0"
          MACOSX_BUNDLE_ICON_FILE "logo-client.icns")

  # configure.sh builds the APNG image plugin into bin/imageformats. Qt searches
  # a bundle's Contents/PlugIns, not bin/, so copy it in at build time. Otherwise
  # a plain build fails at launch with "Missing image formats: apng".
  # (webp/gif come from the Qt install's own plugins.)
  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/bin/imageformats/libqapng.dylib")
    add_custom_command(TARGET Attorney_Online POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_BUNDLE_DIR:Attorney_Online>/Contents/PlugIns/imageformats"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${CMAKE_CURRENT_LIST_DIR}/bin/imageformats/libqapng.dylib"
                    "$<TARGET_BUNDLE_DIR:Attorney_Online>/Contents/PlugIns/imageformats/"
            VERBATIM)
  endif()

  # Bundle the default base (themes, fetched by configure.sh) as read-only
  # defaults in Contents/Resources. On first run the app copies it to the
  # writable base in Application Support (see seed_base_if_missing).
  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/bin/base")
    add_custom_command(TARGET Attorney_Online POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_CURRENT_LIST_DIR}/bin/base"
                    "$<TARGET_BUNDLE_DIR:Attorney_Online>/Contents/Resources/base"
            VERBATIM)
  endif()
endif()
