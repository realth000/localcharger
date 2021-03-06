﻿cmake_minimum_required(VERSION 3.13)

set(APP_VERSION 1.8.3)

project(LocalCharger
    VERSION ${APP_VERSION}
    LANGUAGES CXX)

# Set compile options
set(COMPILE_GUI    ON CACHE BOOL "Compile GUI version(LocalCharger)" FORCE)
set(COMPILE_DAEMON ON CACHE BOOL "Compile DBus-daemon(localcharger-daemon)" FORCE)
set(COMPILE_CLI    ON CACHE BOOL "Compile CLI version(LocalChargerCli), use with dbus" FORCE)
set(COMPILE_QML    OFF CACHE BOOL "Compile Qt.Quick version LocalCharger" FORCE)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find Qt5 packages
find_package(Qt5 CONFIG REQUIRED Core)

if(COMPILE_GUI)
    find_package(Qt5 CONFIG REQUIRED Gui Network Svg WebSockets LinguistTools)
    if(COMPILE_QML)
        message(STATUS "COMPILE_QML is enabled")
        add_definitions(-DENABLE_QML)
        find_package(Qt5 CONFIG REQUIRED Qml Quick)
    else()
        message(STATUS "COMPILE_QML is NOT enabled")
        find_package(Qt5 CONFIG REQUIRED Widgets)
    endif()
endif()

if(UNIX AND COMPILE_DAEMON)
    find_package(Qt5 CONFIG REQUIRED Core Network WebSockets DBus)
endif()

if(UNIX AND COMPILE_CLI)
    find_package(Qt5 CONFIG REQUIRED DBus)
endif()

add_definitions(-DLOCALCHARGER_VERSION_STRING="${PROJECT_VERSION}")

if(WIN32)
    # For MSVC, enable UTF-8 codec
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_definitions(-DLOCALCHARGER_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR})
    add_definitions(-DLOCALCHARGER_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR})
    add_definitions(-DLOCALCHARGER_VERSION_BUGFIX=${CMAKE_PROJECT_VERSION_PATCH})
    add_definitions(-DLOCALCHARGER_VERSION_BUILD=1)
elseif(UNIX)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c++11 -s -Wall -O2 -g -Wl,-z,relro,-z,now -fstack-protector-all -Wl,-z,noexecstack -fPIE -pie -D_FORTIFY_SOURCE=2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -s -Wall -O2 -g -Wl,-z,relro,-z,now -fstack-protector-all -Wl,-z,noexecstack -fPIE -pie -D_FORTIFY_SOURCE=2")
endif()

include_directories(src/)
include_directories(src/include)
include_directories(src/utils)

if(COMPILE_GUI)
    if(WIN32)
        add_executable(LocalCharger WIN32 ${QM_FILES})
    else()
        add_executable(LocalCharger ${QM_FILES})
    endif()
endif()

if(UNIX AND COMPILE_DAEMON)
    add_executable(localcharger-daemon)
endif()

if(UNIX AND COMPILE_CLI)
    add_executable(LocalChargerCli)
endif()

# Generate translations
if(COMPILE_GUI)
    set(LOCALCHARGER_TS_FILES
        ${CMAKE_SOURCE_DIR}/src/resource/translation/localcharger_en.ts
        ${CMAKE_SOURCE_DIR}/src/resource/translation/localcharger_zh_CN.ts)
    set_source_files_properties(${LONOCALCHARGER_TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_SOURCE_DIR}/src/resource/translation")
    qt5_add_translation(QM_FILES ${LOCALCHARGER_TS_FILES})
endif()



### old pos


if(COMPILE_GUI)
    if(COMPILE_QML)
        add_subdirectory(src/qml)
    else()
        add_subdirectory(src/gui)
    endif()
endif()
add_subdirectory(src/resource)
add_subdirectory(src/include)
if(COMPILE_GUI OR COMPILE_DAEMON)
    add_subdirectory(src/utils)
    add_subdirectory(src/core)
endif()


if(UNIX)
    if(COMPILE_CLI)
        add_subdirectory(src/cli)
    endif()
    if(COMPILE_DAEMON)
        add_subdirectory(src/daemon)
    endif()
endif()

if(COMPILE_GUI)
    if(COMPILE_QML)
        target_link_libraries(LocalCharger  Qt5::Core Qt5::Gui Qt5::Network Qt5::WebSockets Qt5::Qml Qt5::Quick)
    else()
        target_link_libraries(LocalCharger  Qt5::Core Qt5::Gui Qt5::Network Qt5::WebSockets Qt5::Widgets)
    endif()
endif()
