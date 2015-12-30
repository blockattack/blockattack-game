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
http://blockattack.net
===========================================================================
*/

#include "os.hpp"


/*
 *Files will be saved in:
 * HOME/.gamesaves/"+GAMENAME (unix)
 *or DOCUMENTS/My Games/GAMENAME (Windows)
 */
#define GAMENAME "blockattack"

#ifdef _WIN32
//Returns path to "my Documents" in windows:
string getMyDocumentsPath() {
	TCHAR pszPath[MAX_PATH];
	//if (SUCCEEDED(SHGetSpecialFolderPath(nullptr, pszPath, CSIDL_PERSONAL, FALSE))) {
	if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_PERSONAL, nullptr, 0, pszPath))) {
		// pszPath is now the path that you want
#if DEBUG
		cout << "MyDocuments Located: " << pszPath << endl;
#endif
		string theResult= pszPath;
		return theResult;
	}
	else {
		cout << "Warning: My Documents not found!" << endl;
		string theResult ="";
		return theResult;
	}
}

#endif

/**
 * Returns the path to where all settings must be saved.
 * On unix-like systems this is the home-folder under: ~/.gamesaves/GAMENAME
 * In Windows it is My Documents/My Games
 * Consider changing this for Vista that has a special save games folder
 */
std::string getPathToSaveFiles() {
#ifdef __unix__
	return (std::string)getenv("HOME")+(std::string)"/.gamesaves/"+GAMENAME;
#elif _WIN32
	return getMyDocumentsPath()+(string)"/My Games/"+GAMENAME;
#else
	return ".";
#endif
}