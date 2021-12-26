cmake_minimum_required(VERSION 3.13)

set(APP_VERSION 1.4.0)

project(LocalCharger
    VERSION ${APP_VERSION}
    LANGUAGES CXX)

# Set compile options
set(COMPILE_QML OFF CACHE BOOL "Compile Qt.Quick version LocalCharger" FORCE)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH $ENV{QT_CMAKE_PATH})

message(STATUS "CMake prefix path =${CMAKE_PREFIX_PATH}")

if(COMPILE_QML)
    message(STATUS "COMPILE_QML is enabled")
    add_definitions(-DENABLE_QML)
    find_package(
        Qt5
        CONFIG
        REQUIRED
        Core
        Gui
        Qml
        Quick
        Network
        Svg
        WebSockets)
else()
    message(STATUS "COMPILE_QML is NOT enabled")
    find_package(
        Qt5
        CONFIG
        REQUIRED
        Core
        Gui
        Widgets
        Network
        Svg
        WebSockets)
endif()

if(WIN32)
    # For MSVC, enable UTF-8 codec
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_definitions(-DLOCALCHARGER_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR})
    add_definitions(-DLOCALCHARGER_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR})
    add_definitions(-DLOCALCHARGER_VERSION_BUGFIX=${CMAKE_PROJECT_VERSION_PATCH})
    add_definitions(-DLOCALCHARGER_VERSION_BUILD=1)
    add_definitions(-DLOCALCHARGER_VERSION_STRING="${PROJECT_VERSION}")
endif()

include_directories(${CMAKE_PREFIX_PATH}/../../include)
include_directories(${CMAKE_PREFIX_PATH}/../../include/QtCore)
include_directories(${CMAKE_PREFIX_PATH}/../../include/QtGui)
include_directories(${CMAKE_PREFIX_PATH}/../../include/QtWidgets)
include_directories(src/)
include_directories(src/include)
include_directories(src/utils)

if(WIN32)
    add_executable(LocalCharger WIN32)
else()
    add_executable(LocalCharger)
endif()

if(COMPILE_QML)
    add_subdirectory(src/qml)
endif()
add_subdirectory(src/resource)
add_subdirectory(src/include)
add_subdirectory(src/utils)
add_subdirectory(src/core)
add_subdirectory(src/gui)

if(COMPILE_QML)
    target_link_libraries(${PROJECT_NAME}  Qt5::Core Qt5::Gui Qt5::Network Qt5::WebSockets Qt5::Qml Qt5::Quick)
else()
    target_link_libraries(${PROJECT_NAME}  Qt5::Core Qt5::Gui Qt5::Network Qt5::WebSockets Qt5::Widgets)
endif()
