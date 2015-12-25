/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2012 Poul Sander

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
http://blockattack.sf.net
===========================================================================
*/

#include "common.h"
#include <sstream>
#include <cstring>

using namespace std;
using boost::format;

static stringstream converter;

//Function to convert numbers to string
string itoa(int num) {
	converter.str(std::string());
	converter.clear();
	converter << num;
	return converter.str();
}

string double2str(double num) {
	converter.str(std::string());
	converter.clear();
	converter << num;
	return converter.str();
}

bool strequals(const char* a, const char* b) {
	return strcmp(a,b) == 0;
}

void dieOnNullptr(bool ptr, const char* msg) {
	if (!ptr) {
		cerr << "Fatal error: " << msg << endl;
		abort();
	}
}

/**
 * str2double parses a string and returns a double with the value of the string.
 * if the string is not a double then 0.0 is returned instead of throing an error
 * in that way this function will always return a useable value.
 */
double str2double(const string& str2parse) {
	try {
		converter.clear();
		converter.str(str2parse);
		double val = 0.0;
		converter >> val;
		return val;
	}
	catch (ios_base::failure& f) {
		return 0.0;
	}
}

/**
 * str2int parses a string and returns an int with the value of the string.
 * if the string is not an int then 0 is returned instead of throing an error
 * in that way this function will always return a useable value.
 */
int str2int(const string& str2parse) {
	try {
		converter.clear();
		converter.str(str2parse);
		int val = 0;
		converter >> val;
		return val;
	}
	catch (ios_base::failure& f) {
		return 0;
	}
}

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
string getPathToSaveFiles() {
#ifdef __unix__
	return (string)getenv("HOME")+(string)"/.gamesaves/"+GAMENAME;
#elif _WIN32
	return getMyDocumentsPath()+(string)"/My Games/"+GAMENAME;
#else
	return ".";
#endif
}

/**
 * Takes a number of milliseconds and returns the value in commonTime format.
 */
commonTime TimeHandler::ms2ct(unsigned int milliseconds) {
	commonTime ct;
	ct.days = 0;
	unsigned int time = milliseconds;
	ct.hours = time/(1000*60*60);
	time = time % (1000*60*60);
	ct.minutes = time/(1000*60);
	time = time % (1000*60);
	ct.seconds = time/1000;
	return ct;
}

commonTime TimeHandler::getTime(const string& name) {
	commonTime ct;
	ct.days = Config::getInstance()->getInt(name+"Days");
	ct.hours = Config::getInstance()->getInt(name+"Hours");
	ct.minutes = Config::getInstance()->getInt(name+"Minutes");
	ct.seconds = Config::getInstance()->getInt(name+"Seconds");
	return ct;
}

/**
 * Returns the total runtime with toAdd added but without writing it to config file.
 * Used for stats
 */
commonTime TimeHandler::peekTime(const string& name, const commonTime& toAdd) {
	commonTime ct = getTime(name);

	ct.seconds +=toAdd.seconds;
	ct.minutes +=ct.seconds/60;
	ct.seconds = ct.seconds%60;

	ct.minutes += toAdd.minutes;
	ct.hours += ct.minutes/60;
	ct.minutes = ct.minutes%60;

	ct.hours += toAdd.hours;
	ct.days += ct.hours/24;
	ct.hours = ct.hours%24;

	ct.days += toAdd.days;
	return ct;
}

/**
 * Same as peekTotalTime but writes the time to the config file.
 * Should only be called only once! when the program shuts down
 */
commonTime TimeHandler::addTime(const string& name, const commonTime& toAdd) {
	commonTime ct = peekTime(name,toAdd);

	Config::getInstance()->setInt(name+"Days",ct.days);
	Config::getInstance()->setInt(name+"Hours",ct.hours);
	Config::getInstance()->setInt(name+"Minutes",ct.minutes);
	Config::getInstance()->setInt(name+"Seconds",ct.seconds);
	return ct;
}

Config* Config::instance = 0;

Config::Config() {
	configMap.clear();
	load();
	shuttingDown = 0; // Not shutting down
}

void Config::load() {
	string filename = getPathToSaveFiles()+"/configFile";
	ifstream inFile(filename.c_str());
	string key;
	string previuskey;
	char value[MAX_VAR_LENGTH];
	if (inFile) {
		while (!inFile.eof()) {
			inFile >> key;
			if (key==previuskey) { //the last entry will be read 2 times if a linebreak is missing in the end
				continue;
			}
			previuskey = key;
			inFile.get(); //Read the space between the key and the content
			inFile.getline(value,MAX_VAR_LENGTH);
#if DEBUG
			cout << "Config "<< "read: " << key << " with:\"" << value << "\"" << endl;
#endif
			configMap[key] = (string)value;
		}
		inFile.close();
	}
}

Config* Config::getInstance() {
	if (Config::instance==0) {
		Config::instance = new Config();

	}
	return Config::instance;
}

void Config::save() {
	string filename = getPathToSaveFiles()+"/configFile";
	ofstream outFile(filename.c_str(),ios::trunc);

	if (outFile) {
		map<string,string>::iterator iter;
		for (iter = configMap.begin(); iter != configMap.end(); iter++) {
			outFile << iter->first << " " << iter->second << endl;
		}
		outFile << "\n"; //The last entry in the file will be read double if a linebreak is missing
		//This is checked on load too in case a user changes it himself.
	}
	outFile.close();
}

bool Config::exists(const string& varName) const {
	//Using that find returns an iterator to the end of the map if not found
	return configMap.find(varName) != configMap.end();
}

void Config::setDefault(const string& varName,const string& content) {
	if (exists(varName)) {
		return;    //Already exists do not change
	}
	setString(varName,content);
}

void Config::setShuttingDown(long shuttingDown) {
	this->shuttingDown = shuttingDown;
}

long Config::isShuttingDown() const {
	return shuttingDown;
}

void Config::setString(const string& varName, const string& content) {
	configMap[varName] = content;
}

void Config::setInt(const string& varName, int content) {
	configMap[varName] = itoa(content);
}

void Config::setValue(const string& varName,double content) {
	configMap[varName] = double2str(content);
}

string Config::getString(const string& varName) {
	if (exists(varName)) {
		return configMap[varName];
	}
	else {
		return "";
	}
}

int Config::getInt(const string& varName) {
	if (exists(varName)) {
		return str2int(configMap[varName]);
	}
	else {
		return 0;
	}
}

double Config::getValue(const string& varName) {
	if (exists(varName)) {
		return str2double(configMap[varName]);
	}
	else {
		return 0.0;
	}
}
