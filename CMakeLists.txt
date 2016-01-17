cmake_minimum_required(VERSION 2.8.7)
project (blockattack)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${blockattack_SOURCE_DIR}/source/misc/cmake")
set(BIN_DIR ${blockattack_SOURCE_DIR}/Game)

# Define install paths
set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE STRING "Install executable in this directory")
set(INSTALL_DATA_DIR "${CMAKE_INSTALL_PREFIX}/share/blockattack" CACHE STRING "Install data to this directory")
set(INSTALL_MAN_DIR "/usr/share/man" CACHE STRING "Install the man page to this directory")
set(INSTALL_APPLICATIONS_DIR "/usr/share/applications" CACHE STRING "Install the .desktop file to this directory")
set(INSTALL_ICONS_DIR "/usr/share/icons/hicolor" CACHE STRING "Install the icon in a subfolder in this directory")

#The path to the data dir must be compiled into the binary
add_definitions(-DSHAREDIR=\"${INSTALL_DATA_DIR}\")

#Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

#Setup SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

#Setup things that use pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf)

#Find all sources but one
file(GLOB SOURCES "source/code/*.cpp" "source/code/*/*.cpp")
#I am still trying to get BlockGame to compile as a source file
list(REMOVE_ITEM SOURCES "${blockattack_SOURCE_DIR}/source/code/BlockGame.cpp")

#Some messages to figure out what cmake does
message("${SOURCES}")
message("${CMAKE_EXE_LINKER_FLAGS}")

#building/compiling/linking
add_executable(blockattack ${SOURCES})
target_link_libraries( blockattack ${SDL2_LIBRARY})
target_link_libraries( blockattack physfs jsoncpp)
target_link_libraries( blockattack ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
set_target_properties( blockattack PROPERTIES RUNTIME_OUTPUT_DIRECTORY Game)

#Installing
install(TARGETS blockattack RUNTIME DESTINATION ${INSTALL_BIN_DIR} )
install(FILES Game/blockattack.data DESTINATION ${INSTALL_DATA_DIR} )
install(FILES man/blockattack.6.gz DESTINATION ${INSTALL_MAN_DIR} )
install(FILES source/misc/blockattack.desktop DESTINATION ${INSTALL_APPLICATIONS_DIR} )
install(FILES Game/data/textures/yellow.png DESTINATION "${INSTALL_ICONS_DIR}/50x50" )
