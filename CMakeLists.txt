cmake_minimum_required(VERSION 2.8.9)
project (blockattack)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${blockattack_SOURCE_DIR}/source/misc/cmake")
set(BIN_DIR ${blockattack_SOURCE_DIR}/Game)


#Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")





find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
file(GLOB SOURCES "source/code/*.cpp" "source/code/*/*.cpp")
#I am still trying to get BlockGame to compile as a source file
list(REMOVE_ITEM SOURCES "${blockattack_SOURCE_DIR}/source/code/BlockGame.cpp")
message("${SOURCES}")
add_executable(blockattack ${SOURCES})
target_link_libraries( blockattack ${SDL2_LIBRARY})
target_link_libraries( blockattack SDL2_image SDL2_mixer SDL2_ttf physfs jsoncpp)
set_target_properties( blockattack PROPERTIES RUNTIME_OUTPUT_DIRECTORY Game)
