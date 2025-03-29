# Changelog

## Unreleased

### Added

### Changed

### Removed

## [2.9.1] - 2025-02-09

### Changed
 - Left and right on gamepad d-pad now works again. Broken in 2.6.0.

## [2.9.0] - 2024-05-09

### Added
 - Option to use 5 different basic blocks instead of 6
 - Added Swedish translation by sanchez-gayatri
 - Can now compile on Mac OS X. However: No prebuilds will be provided.
 - Themes menu allowing some additional customization.

### Changed
 - Update library versions for the compiled version:
   - SDL2: 2.0.20 to 2.30.2
   - SDL2_Image: 2.0.5 to 2.8.2
   - SDL2_Mixer: 2.0.4 to 2.8.0
   - SDL2_ttf: 2.0.15 to 2.22.0
 - Added Releases and OARS to AppStream file
 - Removed the executable bit from a lot of files that data files that should not have it
 - The CMake file now uses more target based rules

## [2.8.0] - 2022-02-18

### Added
 - CONTRIBUTING.md is now included

### Changed
 - Now uses {fmt} for internal formatting
 - Now possible to compile using local copy of PlatformFolders
 - Install the sample mod "1.3.0.bricks"
 - Update library versions for the compiled version:
   - SDL2: 2.0.16 to 2.0.20

## [2.7.0] - 2021-10-23

### Added
 - Volume for sound effect and music is now stored in the configFile
 - Menu for changing music and sound volume

### Changed
 - Linux icons have been converted to png (patch by pinotree)
 - Now requires Ubuntu 18.04 or later (or equivalent)
 - utfcpp is now an embedded library
 - Dialog boxes now uses contructed textures thus reducing tearing on some renderers
 - Garbage blocks now uses constructed textures thus reducing tearing on some renderers
 - Screen shot feature now works with hardware render
 - Update library versions for the compiled version:
   - SDL2: 2.0.12 to 2.0.16

### Removed
 - Linux standalone build no longer includes 32 bit binary

## [2.6.0] - 2020-09-26

### Added
 - Patch by bmwiedemann for sorting in the data file. Should make builds reproducible
 - Virtual keyboard on the enter name screen allowing using a mouse or a gamepad to type.

### Changed
 - Modified the controller keybind. "B" is now Cancel instead of "Y"
 - Update embedded Cerial from 1.2.2 to 1.3.0
 - Update library versions for the compiled version:
   - SDL2: 2.0.9 to 2.0.12
   - SDL2_image: 2.0.4 to 2.0.5
   - SDL2_ttf: 2.0.14 to 2.0.15
   - physfs: 2.0.3 to 3.0.2

## [2.5.0+1] - 2019-07-04

Not given a version number as the build environment is not part of the project.  
Given a +1 to show that the build environment has changed

### Fixed
 - New Windows build environment to prevent a crash on Windows 10 1809

## [2.5.0] - 2019-05-25

### Added
 - Option to always use the software renderer
 - Automatically start in software render the first time after a crash
 - Widescreen support. Only used in fullscreen by default.
 - Appstream metadata

### Fixed
 - Fixed the search order in archives
 - Screenshot feature no longer crashes in hardware acceleration. Still only works in software mode

## [2.4.0] - 2019-03-10

## Added
 - Different starting speeds for Endless
 - Dumps the about page info to "about.txt" when viewed
 - Default player 1 name is now fetched from the OS

## Changed
 - Minor code cleanup
 - Improved "About" page with technical information
 - Improved Unicode support (--homepath can now be used with Unicode chars) (Windows only)

## Removed
 - Support for any Windows operating systems before Windows 7 (Windows only)

## Fixed
 - Build against SDL 2.0.9 fixing some scaling problems and improving gamepad support (Windows only)

## [2.3.0] - 2018-07-01

## Changed
 - Several keyboard keys are now translatable
 - Window size is now restored on next startup
 - The back-button on the gamepad now goes back in menus
 - Gamepad L2 and R2 can now be used to push lines
 - Game Over, Winner and Draw are now translatable

## [2.2.0] - 2018-05-20

## Added
 - Help menu with rule explanations

## Changed
 - New font system
 - Better scaling
 - Works out of the box on Windows with high resolution displays

## [2.1.2] - 2018-01-12

### Fixed
 - The Windows version would not save correctly if "My Games" did not already exist in the Documents folder.

## [2.1.1] - 2017-04-01

This has the first gameplay change in a long time. In the previous version the rise would stop
while clearing. It could stop for several seconds. This is no longer the case. The time is now
bound and will lower as the game progresses.

The update of the model has also been changed. This should make it more smooth in some of the
high speed Stage Clear levels.

### Added
 - Docker files are now provided to make compiling even easier

### Changed
 - There is now an upper limit on how long the rise will stop
 - The internal updated of the game has been updated from 20 times a second to 100 times a second
 - Menu fonts have been changed
 - Window is now resizeable by default
 - Code cleanup
 - Translation strings changed
 - Updated libraries

### Removed
 * jsoncpp is no longer a dependency

## [2.1.0] - 2017-04-01 [YANKED]

## [2.0.1] - 2016-10-01

### Fixed
 - The "Get Ready" animation used the wrong sprite
 - My homepath was hardcoded in the Makefile (Linux only)


## [2.0.0] - 2016-05-01

### Added
 - TTF font support
 - gettext based translation support (includes a Danish translation)

### Changed
 - Uses SDL2. This means textures and sprites instead of old surfaces.
 - New config file format
 - Linux saves are now stored under $HOME/.local/share/blockattack
 - New keyboard and game controller friendly menu system
 - Better behind the scenes handling
 - Now uses a CMake build system
 - Better game controller support (although limited to SDL2 supported controllers)
 - Better mouse play
 - More modern icon
 - Old config and some saves are lost then converting to 2.0.0+

### Removed
 - Replay functionality
 - Network support
   - Including dependency on Enet
 - Themes support
 - Support for non-SDL2 game controllers


## [2.0.0-SNAPSHOT-2016-03-20] - 2016-03-20

Preview release for SDL2 support

## [1.4.2] - 2014-09-11

### Fixed
 - The Windows uninstaller now works on Vista/7/8 (Windows only)

## [1.4.1] - 2009-11-19

### Added
 - PhysFS is now a requirement
 - Theme support

## [1.4.0] - 2009-04-07

### Changed
 - New graphics by Qubodup

## [1.3.2] - 2008-01-17

### Changed
 - Enet updated from 1.0 to 1.1

## [1.3.1] - 2007-01-17

### Fixed
 - Windows 9x know works again (Windows only)
 - Puzzle mode has been fixed (Windows only)

## [1.3.0] - 2006-08-29

### Added
 - gray garbage blocks
 - "." can be entered while typing an IP address

## [1.2.2] - 2006-08-09

### Added
 - 64 bit support

## [1.2.0] - 2006-06-19

### Added
 - Network play
 - Ability to save replays up to 5 minutes long

## [1.1.2] - 2005-11-20

### Added
 - Small numbers while chaining

## [1.1.1] - 2005-10-29

### Added
 - Handicap in multiplayer matches
 - Basic AI

### Changed
 - Now always uses software surfaces
 - Better puzzle management
 - Better controls
 - New Makefile
 - Now hosted on blockattack.sf.net

### Removed
 - Windows 9x support

## [1.1.0] - 2005-06-12

### Changed
 - Radically changed gameplay
 - Blocks no longer falls instantly
 - Blocks will not match while falling
 - Chains give more points
 - Combos give less points

## [1.0.6] - 2005-05-08

### Added
 - Mouse play
 - Joypad support

### Changed
 - Renamed to "Block Attack - Rise of the Blocks"

## [1.0.5] - 2005-05-02
First non beta release
