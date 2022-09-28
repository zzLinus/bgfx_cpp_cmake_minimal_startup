cmake_minimum_required(VERSION 3.7)
project(bgfx_test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES main.cc)

add_subdirectory("3rdParty/bgfx.cmake")    
add_executable(bgfx_test ${SOURCE_FILES} )

target_link_libraries(bgfx_test bgfx example-common)
