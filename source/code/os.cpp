/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2015 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://blockattack.net
===========================================================================
*/

#include "os.hpp"
#include <iostream>
#include <physfs.h>
#include "sago/platform_folders.h"
#include "version.h"
#include "common.h"

#if defined(__unix__)
#include <pwd.h>
#include <unistd.h>
#endif

#include <dirent.h>

static sago::PlatformFolders pf;

static std::string overrideSavePath = "";

/**
 * Returns the path to where all settings must be saved.
 * On unix-like systems this is the home-folder under: ~/.local/share/GAMENAME
 * In Windows it is My Documents/My Games
 * Consider changing this for Vista that has a special save games folder
 */
std::string getPathToSaveFiles() {
	if (overrideSavePath.length() > 0) {
		return overrideSavePath;
	}
	return pf.getSaveGamesFolder1()+"/"+GAMENAME;
}

std::string getPathToStateFiles() {
	return sago::getStateDir()+"/"+GAMENAME+"/state";
}

void setPathToSaveFiles(const std::string& path) {
	overrideSavePath = path;
}

#if defined(_WIN32)
#include "shlwapi.h"

static std::wstring win32_utf8_to_utf16(const char* str) {
	std::wstring res;
	// If the 6th parameter is 0 then WideCharToMultiByte returns the number of bytes needed to store the result.
	int actualSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
	if (actualSize > 0) {
		//If the converted UTF-8 string could not be in the initial buffer. Allocate one that can hold it.
		std::vector<wchar_t> buffer(actualSize);
		actualSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, &buffer[0], buffer.size());
		res = buffer.data();
	}
	if (actualSize == 0) {
		// WideCharToMultiByte return 0 for errors.
		const std::string errorMsg = "UTF8 to UTF16 failed with error code: " + GetLastError();
		throw std::runtime_error(errorMsg.c_str());
	}
	return res;
}
#endif

std::string defaultPlayerName() {
	std::string ret;
#if defined(_WIN32)
	wchar_t win_buffer[50];
	DWORD win_buffer_size = sizeof(win_buffer);
	bool success = GetUserNameW(win_buffer, &win_buffer_size);
	if (success) {
		ret = sago::internal::win32_utf16_to_utf8(win_buffer);
	}
#endif
#if defined(__unix__)
	int uid = getuid();
	struct passwd* pw = getpwuid(uid);
	if (pw && pw->pw_gecos) {
		ret = pw->pw_gecos;
		ret = ret.substr(0, ret.find_first_of(',',0));
	}
	if (pw && pw->pw_name && ret.empty()) {
		ret = pw->pw_name;
	}
#endif
	if (ret.empty()) {
		ret = _("Player 1");
	}
	return ret;
}

bool OsPathIsRelative(const std::string& path) {
#if defined(_WIN32)
	return PathIsRelativeW(win32_utf8_to_utf16(path.c_str()).c_str());
#else
	return path[0] != '/';
#endif
}

void OsCreateFolder(const std::string& path) {
	//Once all supported systems works with C++17 then we can use "std::filesystem::create_directories" instead
#if defined(_WIN32)
	//Now for Windows Vista+
	int retcode = SHCreateDirectoryExW(NULL, win32_utf8_to_utf16(path.c_str()).c_str(), NULL);
	if (retcode != ERROR_SUCCESS) {
		std::cerr << "Failed to create: " << path+"/" << "\n";
	}
#else
	std::string cmd = "mkdir -p '"+path+"/'";
	int retcode = system(cmd.c_str());
	if (retcode != 0) {
		std::cerr << "Failed to create: " << path+"/" << "\n";
	}
#endif
}

void OsCreateSaveFolder() {
	std::string path = getPathToSaveFiles();
	OsCreateFolder(path);
}

std::vector<std::string> OsGetDirFileList(const std::string& path) {
	//At the moment dirent.h is used for all OS. Should have a Unicode version on Windows.
	std::vector<std::string> ret;
	DIR* dir = opendir(path.c_str());
	if (dir) {
		struct dirent* ent;
		while ((ent = readdir (dir)) ) {
			if (ent->d_name[0] == '.') {
				continue;
			}
			ret.push_back(ent->d_name);
		}
		closedir (dir);
	}
	else {
		std::cerr << "Failed to list: " << path+"/" << "\n";
	}
	return ret;
}
