cmake_minimum_required(VERSION 3.0)

project(RayMarch VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC ${ROOT}/src)
set(BIN ${ROOT}/bin)
set(DEPS ${ROOT}/deps)

file(GLOB_RECURSE sources "${SRC}/*.h" "${SRC}/*.cpp")
add_executable(RayMarch ${sources})
set(EXECUTABLE_OUTPUT_PATH ${BIN})

target_include_directories(RayMarch PRIVATE ${SRC} ${DEPS}/glfw/include ${DEPS}/ctpl)
target_link_libraries(RayMarch PRIVATE glfw glad)

add_subdirectory(${DEPS}/glfw)
add_subdirectory(${DEPS}/glad)