cmake_minimum_required(VERSION 3.10)
project(lvgl LANGUAGES C CXX ASM)

# Change these for your own project
set(LVGL_DIR "${CMAKE_SOURCE_DIR}/3rdparty/lvgl-sdl/lvgl")
set(LVDRV_DIR "${CMAKE_SOURCE_DIR}/3rdparty/lvgl-sdl/lv_drv")
set(PROJECT_DIR "${CMAKE_SOURCE_DIR}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Remove this for your own project
add_definitions(-DMAKE_EXAMPLE)

# Add all the source files
file(GLOB LVGL_SOURCE_FILES
    # My lvgl driver source files
    "${LVDRV_DIR}/lv_sdl_drv_input.c"
    "${LVDRV_DIR}/lv_sdl_drv_display.c"
    "${LVDRV_DIR}/lv_if_drv_filesystem.c"

    # Main lvgl source files
    "${LVGL_DIR}/src/lv_core/*.c"
    "${LVGL_DIR}/src/lv_draw/*.c"
    "${LVGL_DIR}/src/lv_font/*.c"
    "${LVGL_DIR}/src/lv_gpu/*.c"
    "${LVGL_DIR}/src/lv_hal/*.c"
    "${LVGL_DIR}/src/lv_misc/*.c"
    "${LVGL_DIR}/src/lv_themes/*.c"
    "${LVGL_DIR}/src/lv_widgets/*.c"

    # JSON
    "${PROJECT_DIR}/3rdparty/json/single_include/nlohmann/json.hpp"

    # libsemver
    "${PROJECT_DIR}/3rdparty/semver-utils/src/libsemver/c++/version.cpp"

    #
    "${PROJECT_DIR}/src/main.cpp"
    "${PROJECT_DIR}/src/App.cpp"
    "${PROJECT_DIR}/src/Downloader.cpp"
    "${PROJECT_DIR}/src/Update.cpp"
    "${PROJECT_DIR}/src/EEPROM.cpp"
    "${PROJECT_DIR}/src/Scene.cpp"
    "${PROJECT_DIR}/src/scenes/*.cpp"
    "${PROJECT_DIR}/src/lvgl-extra/*.cpp"
    "${PROJECT_DIR}/assets/*.c"
)
add_executable(lvgl_example ${LVGL_SOURCE_FILES})

# Add SDL2 lib
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(lvgl_example ${SDL2_LIBRARY})

# My example project includes (Change this for your own project)
include_directories(${PROJECT_DIR})
include_directories(${PROJECT_DIR}/include)
include_directories(${PROJECT_DIR}/src)

# My lvgl driver includes
include_directories(${LVDRV_DIR})

# Main lvgl lib includes
include_directories(${LVGL_DIR}/src/)
#include_directories(${LVGL_DIR}/../)

#
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# CPR
add_subdirectory(${PROJECT_DIR}/3rdparty/cpr)
include_directories(${PROJECT_DIR}/3rdparty/cpr/include)
target_link_libraries(lvgl_example cpr)

# libsemver
include_directories(${PROJECT_DIR}/3rdparty/semver-utils/src/libsemver/c++)
