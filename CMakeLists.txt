cmake_minimum_required(VERSION 3.10)
project(BlackWhiteConverter)

find_package(PkgConfig)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

include_directories(${GSTREAMER_INCLUDE_DIRS})

add_executable(BlackWhiteConverter main.cpp)

target_link_libraries(BlackWhiteConverter ${GSTREAMER_LIBRARIES})

target_compile_options(BlackWhiteConverter PUBLIC ${GSTREAMER_CFLAGS_OTHER})
