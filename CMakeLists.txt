cmake_minimum_required(VERSION 2.8.7)
project (blockattack)
set(BIN_DIR ${blockattack_SOURCE_DIR}/Game)

SET(CPACK_PACKAGE_VERSION "2.7.0-SNAPSHOT")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "7")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Block Attack - Rise of the Blocks ${CPACK_PACKAGE_VERSION}")
SET(CPACK_PACKAGE_VENDOR "Poul Sander")
INCLUDE(CPack)

# Define install paths
set(INSTALL_BIN_DIR "bin" CACHE STRING "Install executable in this directory")
set(INSTALL_DATA_DIR "share/blockattack" CACHE STRING "Install data to this directory")
set(INSTALL_MAN_DIR "share/man/man6" CACHE STRING "Install the man page to this directory")
set(INSTALL_APPLICATIONS_DIR "share/applications" CACHE STRING "Install the .desktop file to this directory")
set(INSTALL_ICONS_DIR "share/icons/hicolor" CACHE STRING "Install the icon in a subfolder in this directory")
set(INSTALL_LOCALE_DIR "${INSTALL_DATA_DIR}/locale/" CACHE STRING "Install translation to this dir")
set(INSTALL_METAINFO_DIR "share/metainfo" CACHE STRING "Install appstream matedata to this directory")

# This sets up the exe icon for windows under mingw.
# Taken from https://hansonry.wordpress.com/2010/12/15/windows-application-icon-using-mingw-and-cmake/
set(RES_FILES "")
if(MINGW)
 set(RES_FILES "${blockattack_SOURCE_DIR}/source/code/winicon.rc")
 set(CMAKE_RC_COMPILER_INIT windres)
 ENABLE_LANGUAGE(RC)
 SET(CMAKE_RC_COMPILE_OBJECT
 "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
endif(MINGW)

if (WIN32)
      SET(GUI_TYPE WIN32)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")
	if(NOT _WIN32_WINNT AND NOT WINVER)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 -DWINVER=0x0601")
	endif()
endif()

if (NOT WIN32 AND NOT STANDALONE)
	#The path to the data dir must be compiled into the binary
	if (IS_ABSOLUTE ${INSTALL_DATA_DIR})
		add_definitions(-DSHAREDIR=\"${INSTALL_DATA_DIR}\")
	else()
		add_definitions(-DSHAREDIR=\"${CMAKE_INSTALL_PREFIX}/${INSTALL_DATA_DIR}\")
	endif()
	if (IS_ABSOLUTE ${INSTALL_DATA_DIR})
		add_definitions(-DLOCALEDIR=\"${INSTALL_LOCALE_DIR}\")
	else()
		add_definitions(-DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/${INSTALL_LOCALE_DIR}\")
	endif()
endif()

#Compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

#Setup SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

#Setup things that use pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf)

include_directories(${SDL2MIXER_INCLUDE_DIRS})
include_directories(${SDL2IMAGE_INCLUDE_DIRS})
include_directories(${SDL2TTF_INCLUDE_DIRS})

find_package(Boost COMPONENTS program_options REQUIRED)

#Find all sources but one
file(GLOB SOURCES "source/code/*.cpp" "source/code/*/*.cpp" "source/code/Libs/*.c")
#I am still trying to get BlockGame to compile as a source file
#list(REMOVE_ITEM SOURCES "${blockattack_SOURCE_DIR}/source/code/BlockGame.cpp")

#Some messages to figure out what cmake does
message("${SOURCES}")
message("${CMAKE_EXE_LINKER_FLAGS}")

include_directories("source/code/Libs/include")
include_directories("source/code/Libs/include/cereal/external")  #Contains rapidjson

#building/compiling/linking
add_executable(blockattack ${GUI_TYPE} ${SOURCES} ${RES_FILES})
target_link_libraries( blockattack ${SDL2_LIBRARIES})
target_link_libraries( blockattack physfs)
target_link_libraries( blockattack ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})
target_link_libraries( blockattack ${Boost_LIBRARIES} )
set_target_properties( blockattack PROPERTIES RUNTIME_OUTPUT_DIRECTORY Game)

#Installing
install(TARGETS blockattack RUNTIME DESTINATION ${INSTALL_BIN_DIR} )
install(FILES Game/blockattack.data DESTINATION ${INSTALL_DATA_DIR} )
install(FILES man/blockattack.6.gz DESTINATION ${INSTALL_MAN_DIR} )
install(FILES source/misc/blockattack.desktop DESTINATION ${INSTALL_APPLICATIONS_DIR} )
install(FILES source/misc/net.blockattack.game.metainfo.xml DESTINATION ${INSTALL_METAINFO_DIR} )
install(FILES source/misc/icons/blockattack16.png DESTINATION "${INSTALL_ICONS_DIR}/16x16/apps" RENAME blockattack.png )
install(FILES source/misc/icons/blockattack48.png DESTINATION "${INSTALL_ICONS_DIR}/48x48/apps" RENAME blockattack.png )
install(FILES source/misc/icons/blockattack128.png DESTINATION "${INSTALL_ICONS_DIR}/128x128/apps" RENAME blockattack.png )
install(FILES source/misc/icons/blockattack256.png DESTINATION "${INSTALL_ICONS_DIR}/256x256/apps" RENAME blockattack.png )
install(FILES source/misc/icons/blockattack64.png DESTINATION "${INSTALL_ICONS_DIR}/64x64/apps" RENAME blockattack.png )
install(FILES source/misc/icons/blockattack32.png DESTINATION "${INSTALL_ICONS_DIR}/32x32/apps" RENAME blockattack.png )
install(DIRECTORY source/misc/translation/locale/ DESTINATION "${INSTALL_LOCALE_DIR}")
