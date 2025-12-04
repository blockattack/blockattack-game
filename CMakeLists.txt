cmake_minimum_required(VERSION 3.5.1...4.1.0)
project (blockattack)
set(BIN_DIR ${blockattack_SOURCE_DIR}/Game)

SET(CPACK_PACKAGE_VERSION "2.11.0-SNAPSHOT")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "11")
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

option(BLOCKATTACK_USE_EMBEDDED_FMT "Use the embedded version of {fmt}" ON)
option(BLOCKATTACK_USE_EMBEDDED_PLATFORM_FOLDERS "Use the embedded version of PlatformFolders" ON)

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
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

# Setup SDL2 and extensions
find_package(SDL2 REQUIRED)

# Try modern CMake approach first, fallback to pkg-config for cross-compilation (e.g., MXE)
find_package(SDL2_image QUIET)
find_package(SDL2_mixer QUIET)
find_package(SDL2_ttf QUIET)

if(NOT SDL2_image_FOUND OR NOT SDL2_mixer_FOUND OR NOT SDL2_ttf_FOUND)
	find_package(PkgConfig REQUIRED)
	if(NOT SDL2_image_FOUND)
		pkg_search_module(SDL2_image REQUIRED SDL2_image)
	endif()
	if(NOT SDL2_mixer_FOUND)
		pkg_search_module(SDL2_mixer REQUIRED SDL2_mixer)
	endif()
	if(NOT SDL2_ttf_FOUND)
		pkg_search_module(SDL2_ttf REQUIRED SDL2_ttf)
	endif()
endif()


find_package(Intl REQUIRED)

find_package(Boost COMPONENTS program_options REQUIRED NO_MODULE)

#Find all sources but one
file(GLOB SOURCES CONFIGURE_DEPENDS "source/code/*.cpp" "source/code/*.inc" "source/code/*.h*" "source/code/*/*.cpp" "source/code/*/*.h*" "source/code/Libs/*.c" "source/code/Libs/*.h*" "source/code/Libs/*/*.cpp" "source/code/Libs/*/*/*.cpp")
#I am still trying to get BlockGame to compile as a source file
#list(REMOVE_ITEM SOURCES "${blockattack_SOURCE_DIR}/source/code/BlockGame.cpp")

#Some messages to figure out what cmake does
message("${SOURCES}")
message("${CMAKE_EXE_LINKER_FLAGS}")

if (BLOCKATTACK_USE_EMBEDDED_FMT)
	message("Using embedded {fmt}")
	add_subdirectory("source/misc/embedded_libs/fmt-11.0.2" EXCLUDE_FROM_ALL)
	message("Done processing {fmt}")
endif()

if (BLOCKATTACK_USE_EMBEDDED_PLATFORM_FOLDERS)
	message("Using embedded PlatformFolders")
	add_subdirectory("source/misc/embedded_libs/PlatformFolders-4.2.0" EXCLUDE_FROM_ALL)
	message("Done processing PlatformFolders")
endif()


#building/compiling/linking
add_executable(blockattack ${GUI_TYPE} ${SOURCES} ${RES_FILES}
    source/code/menudef_themes.cpp
    source/code/menudef_themes.hpp)

# Modern target-based compiler settings
target_compile_features(blockattack PRIVATE cxx_std_17)
target_compile_options(blockattack PRIVATE
	$<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
	$<$<CONFIG:Debug>:-g>
	$<$<AND:$<BOOL:${WIN32}>,$<CXX_COMPILER_ID:GNU>>:-march=i686>
)
target_compile_definitions(blockattack PRIVATE
	$<$<CONFIG:Debug>:DEBUG>
	$<$<AND:$<BOOL:${WIN32}>,$<NOT:$<BOOL:${_WIN32_WINNT}>>>:_WIN32_WINNT=0x0601>
	$<$<AND:$<BOOL:${WIN32}>,$<NOT:$<BOOL:${WINVER}>>>:WINVER=0x0601>
)

# Platform-specific definitions for data and locale directories
if (NOT WIN32 AND NOT STANDALONE)
	if (IS_ABSOLUTE ${INSTALL_DATA_DIR})
		target_compile_definitions(blockattack PRIVATE SHAREDIR="${INSTALL_DATA_DIR}")
	else()
		target_compile_definitions(blockattack PRIVATE SHAREDIR="${CMAKE_INSTALL_PREFIX}/${INSTALL_DATA_DIR}")
	endif()
	if (IS_ABSOLUTE ${INSTALL_LOCALE_DIR})
		target_compile_definitions(blockattack PRIVATE LOCALEDIR="${INSTALL_LOCALE_DIR}")
	else()
		target_compile_definitions(blockattack PRIVATE LOCALEDIR="${CMAKE_INSTALL_PREFIX}/${INSTALL_LOCALE_DIR}")
	endif()
endif()

target_include_directories(blockattack SYSTEM PRIVATE "source/code/Libs/imgui")
target_include_directories(blockattack PRIVATE "source/code/Libs/include")
target_include_directories(blockattack PRIVATE "source/code/Libs/include/cereal/external")  #Contains rapidjson
if(NOT TARGET SDL2_image::SDL2_image)
	target_include_directories(blockattack PRIVATE ${SDL2_image_INCLUDE_DIRS})
	target_include_directories(blockattack PRIVATE ${SDL2_mixer_INCLUDE_DIRS})
	target_include_directories(blockattack PRIVATE ${SDL2_ttf_INCLUDE_DIRS})
endif()
target_include_directories(blockattack PRIVATE ${Intl_INCLUDE_DIRS})

target_link_libraries(blockattack PRIVATE
	SDL2::SDL2
	fmt
	platform_folders
	physfs
	${Boost_LIBRARIES}
)

# Link SDL2 extensions - use imported targets if available, otherwise use pkg-config variables
if(TARGET SDL2_image::SDL2_image)
	target_link_libraries(blockattack PRIVATE SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf)
else()
	target_link_libraries(blockattack PRIVATE ${SDL2_image_LIBRARIES} ${SDL2_mixer_LIBRARIES} ${SDL2_ttf_LIBRARIES})
endif()

if (MACOSX)
	target_link_libraries(blockattack PRIVATE intl)
endif()
set_target_properties(blockattack PROPERTIES RUNTIME_OUTPUT_DIRECTORY Game)

#Installing
install(TARGETS blockattack RUNTIME DESTINATION ${INSTALL_BIN_DIR} )
install(FILES Game/blockattack.data DESTINATION ${INSTALL_DATA_DIR} )
install(FILES Game/mods/1.3.0.bricks.data DESTINATION "${INSTALL_DATA_DIR}/mods" )
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
