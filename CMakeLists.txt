cmake_minimum_required(VERSION 3.5.1...4.1.0)
project (blockattack)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set(BIN_DIR ${blockattack_SOURCE_DIR}/Game)

SET(CPACK_PACKAGE_VERSION "2.11.0-SNAPSHOT")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "11")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Block Attack - Rise of the Blocks ${CPACK_PACKAGE_VERSION}")
SET(CPACK_PACKAGE_VENDOR "Poul Sander")

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

# Platform dependencies (SDL2 family, PhysFS, gettext, Boost) are carried by
# this INTERFACE target, however each platform provides them.
add_library(blockattack_deps INTERFACE)

if(EMSCRIPTEN)
	# Data and locale are preloaded at "/" and cwd is "/", so the STANDALONE
	# layout (SHAREDIR=".", LOCALEDIR="./locale") is exactly right.
	set(STANDALONE ON)

	# SDL2 family from Emscripten ports. Same flags at compile time (headers)
	# and link time (prebuilt libraries).
	set(BLOCKATTACK_EM_PORTS -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=png,jpg
	                         -sUSE_SDL_TTF=2 -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=ogg)
	target_compile_options(blockattack_deps INTERFACE ${BLOCKATTACK_EM_PORTS})
	target_link_options(blockattack_deps INTERFACE ${BLOCKATTACK_EM_PORTS})

	# PhysFS has no port; build it from source (zip archiver only, packdata.sh
	# produces zips). Offline: -DFETCHCONTENT_SOURCE_DIR_PHYSFS=<physfs release-3.2.0 checkout>
	include(FetchContent)
	FetchContent_Declare(physfs
		GIT_REPOSITORY https://github.com/icculus/physfs.git
		GIT_TAG release-3.2.0)
	set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
	set(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)
	set(PHYSFS_BUILD_DOCS OFF CACHE BOOL "" FORCE)
	foreach(archiver 7Z GRP WAD HOG MVL QPAK SLB ISO9660 VDF)
		set(PHYSFS_ARCHIVE_${archiver} OFF CACHE BOOL "" FORCE)
	endforeach()
	set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # physfs 3.2.0 still declares CMake 3.0
	FetchContent_MakeAvailable(physfs)
	unset(CMAKE_POLICY_VERSION_MINIMUM)
	target_link_libraries(blockattack_deps INTERFACE PhysFS::PhysFS-static)

	# libintl is part of Emscripten's libc. Boost.program_options is not used
	# in the browser (see main.cpp). Exceptions are the game's error path and
	# emcc disables catching them unless asked; Asyncify needs the JS-based flavour.
	add_compile_options(-fexceptions)
	add_link_options(-fexceptions)
else()
	# Setup SDL2 and extensions
	find_package(SDL2 REQUIRED)

	# Try modern CMake approach first, fallback to pkg-config for cross-compilation (e.g., MXE)
	find_package(SDL2_image QUIET)
	find_package(SDL2_mixer QUIET)
	find_package(SDL2_ttf QUIET)

	set(SDL2_USE_PKGCONFIG OFF)
	if(NOT SDL2_image_FOUND OR NOT SDL2_mixer_FOUND OR NOT SDL2_ttf_FOUND)
		set(SDL2_USE_PKGCONFIG ON)
		find_package(PkgConfig REQUIRED)
		# When using pkg-config, always search for all SDL2 extensions
		pkg_search_module(SDL2_image REQUIRED SDL2_image)
		pkg_search_module(SDL2_mixer REQUIRED SDL2_mixer)
		pkg_search_module(SDL2_ttf REQUIRED SDL2_ttf)
	endif()

	find_package(Intl REQUIRED)

	find_package(Boost COMPONENTS program_options REQUIRED NO_MODULE)

	# Link SDL2 extensions - use imported targets if available, otherwise use pkg-config variables
	if(SDL2_USE_PKGCONFIG)
		target_include_directories(blockattack_deps INTERFACE ${SDL2_image_INCLUDE_DIRS} ${SDL2_mixer_INCLUDE_DIRS} ${SDL2_ttf_INCLUDE_DIRS})
		target_link_directories(blockattack_deps INTERFACE ${SDL2_image_LIBRARY_DIRS} ${SDL2_mixer_LIBRARY_DIRS} ${SDL2_ttf_LIBRARY_DIRS})
		target_link_libraries(blockattack_deps INTERFACE ${SDL2_image_LINK_LIBRARIES} ${SDL2_mixer_LINK_LIBRARIES} ${SDL2_ttf_LINK_LIBRARIES})
	else()
		target_link_libraries(blockattack_deps INTERFACE SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf)
	endif()
	target_include_directories(blockattack_deps INTERFACE ${Intl_INCLUDE_DIRS})
	target_link_libraries(blockattack_deps INTERFACE SDL2::SDL2 physfs ${Boost_LIBRARIES})
	if (MACOSX)
		target_link_libraries(blockattack_deps INTERFACE intl)
	endif()
endif()

#Find all sources but one
file(GLOB SOURCES CONFIGURE_DEPENDS "source/code/*.cpp" "source/code/*.inc" "source/code/*.h*" "source/code/*/*.cpp" "source/code/*/*.h*" "source/code/Libs/*.c" "source/code/Libs/*.h*" "source/code/Libs/*/*.cpp" "source/code/Libs/*/*/*.cpp")
#I am still trying to get BlockGame to compile as a source file
#list(REMOVE_ITEM SOURCES "${blockattack_SOURCE_DIR}/source/code/BlockGame.cpp")

#Some messages to figure out what cmake does
message("${SOURCES}")
message("${CMAKE_EXE_LINKER_FLAGS}")

if (BLOCKATTACK_USE_EMBEDDED_FMT)
	message("Using embedded {fmt}")
	add_subdirectory("source/misc/embedded_libs/fmt-12.1.0" EXCLUDE_FROM_ALL)
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
target_compile_features(blockattack PRIVATE cxx_std_20)
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

target_link_libraries(blockattack PRIVATE
	blockattack_deps
	fmt
	platform_folders
)
set_target_properties(blockattack PROPERTIES RUNTIME_OUTPUT_DIRECTORY Game)

if(EMSCRIPTEN)
	set(BLOCKATTACK_WEB_SHELL "${blockattack_SOURCE_DIR}/source/misc/web/shell.html")
	set(BLOCKATTACK_DATA_FILE "${blockattack_SOURCE_DIR}/Game/blockattack.data")
	set(BLOCKATTACK_MOD_FILE "${blockattack_SOURCE_DIR}/Game/mods/1.3.0.bricks.data")
	set(BLOCKATTACK_LOCALE_DIR "${blockattack_SOURCE_DIR}/source/misc/translation/locale")
	foreach(f IN ITEMS "${BLOCKATTACK_DATA_FILE}" "${BLOCKATTACK_MOD_FILE}" "${BLOCKATTACK_LOCALE_DIR}")
		if(NOT EXISTS "${f}")
			message(FATAL_ERROR "${f} is missing. Run ./packdata.sh before configuring the web build.")
		endif()
	endforeach()
	# One page: index.html (from the shell, which sizes the canvas to the
	# browser window) loads index.js, index.wasm and index.data. ASYNCIFY lets
	# the blocking SDL_Delay game loops yield to the browser each frame instead
	# of being rewritten around a callback; its stack holds the locals of every
	# unwound frame, so it is sized well above the default 4 KiB. IDBFS backs
	# the save folder (see os.cpp).
	set_target_properties(blockattack PROPERTIES
		OUTPUT_NAME index
		SUFFIX ".html"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/web")
	target_link_options(blockattack PRIVATE
		-sASYNCIFY
		-sASYNCIFY_STACK_SIZE=1048576
		-sALLOW_MEMORY_GROWTH=1
		-lidbfs.js
		$<$<CONFIG:Debug>:-sASSERTIONS=1>
		"SHELL:--shell-file ${BLOCKATTACK_WEB_SHELL}"
		"SHELL:--preload-file ${BLOCKATTACK_DATA_FILE}@/blockattack.data"
		"SHELL:--preload-file ${BLOCKATTACK_MOD_FILE}@/mods/1.3.0.bricks.data"
		"SHELL:--preload-file ${BLOCKATTACK_LOCALE_DIR}@/locale")
	set_property(TARGET blockattack APPEND PROPERTY LINK_DEPENDS
		"${BLOCKATTACK_WEB_SHELL}" "${BLOCKATTACK_DATA_FILE}" "${BLOCKATTACK_MOD_FILE}")
endif()

#Installing
include(CPack)
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
