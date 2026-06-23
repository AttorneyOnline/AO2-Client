# Override for Qt's FindWrapOpenGL.cmake on macOS.
#
# Qt 6.5–6.8's bundled FindWrapOpenGL.cmake unconditionally adds
# `-framework AGL` to QtGui's public link interface. AGL was removed from
# the macOS SDK around Xcode 16 / macOS 15, so the link fails with
# "framework 'AGL' not found". Fixed upstream in Qt 6.9.
#
# This file is a copy of Qt's module with the single AGL target_link_libraries
# line removed. It is picked up via CMAKE_MODULE_PATH before Qt's copy.
#
# Original: Copyright (C) 2022 The Qt Company Ltd. SPDX BSD-3-Clause.

if(TARGET WrapOpenGL::WrapOpenGL)
    set(WrapOpenGL_FOUND ON)
    return()
endif()

set(WrapOpenGL_FOUND OFF)

find_package(OpenGL ${WrapOpenGL_FIND_VERSION})

if (OpenGL_FOUND)
    set(WrapOpenGL_FOUND ON)

    add_library(WrapOpenGL::WrapOpenGL INTERFACE IMPORTED)
    if(APPLE)
        get_target_property(__opengl_fw_lib_path OpenGL::GL IMPORTED_LOCATION)
        if(__opengl_fw_lib_path AND NOT __opengl_fw_lib_path MATCHES "/([^/]+)\\.framework$")
            get_filename_component(__opengl_fw_path "${__opengl_fw_lib_path}" DIRECTORY)
        endif()

        if(NOT __opengl_fw_path)
            set(__opengl_fw_path "-framework OpenGL")
        endif()

        target_link_libraries(WrapOpenGL::WrapOpenGL INTERFACE ${__opengl_fw_path})
    else()
        target_link_libraries(WrapOpenGL::WrapOpenGL INTERFACE OpenGL::GL)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WrapOpenGL DEFAULT_MSG WrapOpenGL_FOUND)
