cmake_minimum_required(VERSION 2.8.7)
project (blockattack)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${blockattack_SOURCE_DIR}/source/misc/cmake")
set(BIN_DIR ${blockattack_SOURCE_DIR}/Game)

# Define install paths
set(INSTALL_BINARY_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE STRING "Install executable in this directory")
set(INSTALL_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/blockattack" CACHE STRING "Install data to this directory")
set(INSTALL_MAN_DIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE STRING "Install the man page to this directory")

#Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")





find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf)
#string(REPLACE ";" " " SDL2EXTRA_LDFLAGS "${SDL2EXTRA_LDFLAGS}") 
#string(REPLACE ";" " " SDL2EXTRA_LIBRARIES "${SDL2EXTRA_LIBRARIES}") 
#string(REPLACE ";" " " SDL2MIXER_LDFLAGS "${SDL2MIXER_LDFLAGS}") 
#string(REPLACE ";" " " SDL2IMAGE_LIBRARIES "${SDL2IMAGE_LIBRARIES}") 
#string(REPLACE ";" " " SDL2TTF_LIBRARIES "${SDL2TTF_LIBRARIES}") 
#SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${SDL2MIXER_LDFLAGS}" )

file(GLOB SOURCES "source/code/*.cpp" "source/code/*/*.cpp")
#I am still trying to get BlockGame to compile as a source file
list(REMOVE_ITEM SOURCES "${blockattack_SOURCE_DIR}/source/code/BlockGame.cpp")
message("${SOURCES}")
message("${CMAKE_EXE_LINKER_FLAGS}")
add_executable(blockattack ${SOURCES})
target_link_libraries( blockattack ${SDL2_LIBRARY})
target_link_libraries( blockattack physfs jsoncpp)
target_link_libraries( blockattack ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
set_target_properties( blockattack PROPERTIES RUNTIME_OUTPUT_DIRECTORY Game)
