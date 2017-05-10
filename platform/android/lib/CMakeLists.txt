# Global options
cmake_minimum_required(VERSION 3.6.0)
set(CMAKE_VERBOSE_MAKEFILE od)

set(MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_DIR ${MODULE_DIR}/..)
set(ROOT_DIR ${PROJECT_DIR}/../..)
set(DISTRIBUTION_DIR ${PROJECT_DIR}/dist/${CMAKE_BUILD_TYPE}/${ANDROID_ABI})
set("ndk-stl_DIR" "${CMAKE_CURRENT_SOURCE_DIR}")

include(${PROJECT_DIR}/lib/clientlib.cmake)
# Android Studio 2.x with CMake support does not pack stl shared libraries,
# so app needs to pack the right shared lib into APK. This sample uses solution
# from https://github.com/jomof/ndk-stl to find the right stl shared lib to use
# and copy it to the right place for Android Studio to pack
# Usage: download ndk-stl-config.cmake into app's directory hosting CMakeLists.txt
#        and just use it with the following line
include(${PROJECT_DIR}/lib/ndk-stl-config.cmake)

# Compiler options
add_definitions("-DZLIB_ONLY")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fpic -frtti -fexceptions -fpermissive -Wall")

include_directories(
    ${CLIENTLIB_DIR}
    ${CLIENTLIB_DIR}/public
    ${THIRDPARTY_DIR}
    ${MODULE_DIR}/src/clientlib/cpp/)

# Build clientlib without JNI
add_library("clientlib" STATIC ${CLIENTLIB_ALL_SOURCES})

# Build shared library with JNI
file(GLOB CLIENTLIB_JNI_SOURCES ${MODULE_DIR}/src/clientlib/cpp/*.cpp)

add_library("lcp-min" SHARED ${CLIENTLIB_JNI_SOURCES})
target_link_libraries("lcp-min" "clientlib")

# LCP with content filter
add_definitions("-DFEATURES_READIUM -DREADIUM_GCC")
include(${PROJECT_DIR}/lib/contentfilter.cmake)
file(GLOB CONTENTFILTER_JNI_SOURCES ${MODULE_DIR}/src/contentfilter/cpp/*.cpp)
include_directories(
    ${CONTENTFILTER_DIR}
    ${CONTENTFILTER_DIR}/public
    ${RSDK_INCLUDE_DIR}
    ${MODULE_DIR}/src/contentfilter/cpp/)

# Build contentfilter without JNI
add_library("contentfilter" STATIC ${CONTENTFILTER_SOURCES})

add_library("lcp" SHARED
    ${CLIENTLIB_JNI_SOURCES}
    ${CONTENTFILTER_JNI_SOURCES})

# Link to lcp-client-lib, lcp-content-filter and rsdk
target_link_libraries("lcp" "clientlib" "contentfilter" ${RSDK_LIB_DIR}/${ANDROID_ABI}/libepub3.so)
