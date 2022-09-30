cmake_minimum_required(VERSION 3.7)
project(bgfx_test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(BX_CONFIG_DEBUG ON)
file(GLOB_RECURSE SRC_FILES src/*.cc)

add_subdirectory("3rdParty/bgfx.cmake")    
add_executable(bgfx_test ${SRC_FILES} main.cc)

target_include_directories(bgfx_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(bgfx_test bgfx bx bimg example-common)

