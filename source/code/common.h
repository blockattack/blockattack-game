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
https://blockattack.net
===========================================================================
*/

/*
 *This is the common.h
 *It contains some basic functions that nearly all multi platform games are going
 *to need.
 */

#ifndef _COMMON_H
#define	_COMMON_H

#include <string>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <libintl.h>
#include <ctime>


#define _(String) gettext (String)

struct commonTime
{
	unsigned int days = 0;
	unsigned int hours = 0;
	unsigned int minutes = 0;
	unsigned int seconds = 0;
};

bool strequals(const char* a, const char* b);

/**
 * str2int parses a string and returns an int with the value of the string.
 * if the string is not an int then 0 is returned instead of throwing an error
 * in that way this function will always return a usable value.
 */
int str2int(const std::string &str2parse) __attribute__((const));

void dieOnNullptr(bool, const char* msg);

/**
 * str2double parses a string and returns a double with the value of the string.
 * if the string is not a double then 0.0 is returned instead of throwing an error
 * in that way this function will always return a usable value.
 */
double str2double(const std::string &str2parse) __attribute__((const));

class TimeHandler
{
public:
	static commonTime ms2ct(unsigned int milliseconds);

	static commonTime getTime(const std::string &name);

	static commonTime peekTime(const std::string &name, const commonTime &toAdd);

	static commonTime addTime(const std::string &name, const commonTime &toAdd);
};

#define MAX_VAR_LENGTH 1024

/*This is the Config class it is a map to hold config variables.
 *It is inspired by Quake 3's CVAR system although a lot simpler.
 *All variables have keys "varName" that is used to access a variable.
 *
 *This class is a singleton
 */
class Config
{
private:
	std::map<std::string, std::string> configMap;

	static Config *instance;

	void load();

	/* tells if the user has requested a shutdown */
	long shuttingDown = 0;

	Config();


public:
	/*Config is a singleton.
	 *It is accessed like this:
	 *Config::getInstance()->method2call(parameters);
	 */
	static Config* getInstance();

	/*save()
	 *forces the config to be written to disk. This will also happened if the
	 *program terminates normally.
	 */
	void save();

	/*getString(varName)
	 *Looks in the config file and returns the string that matches the key "varName"
	 *Returns an empty string if varName does not exist.
	 */
	std::string getString(const std::string &varName);

	/*getInt(varName)
	 *Looks in the config file and returns the int that matches the key "varName"
	 *Returns "0" if varName does not exist or cannot be parsed.
	 */
	int getInt(const std::string &varName);

	/*getValue(varName)
	 *Looks in the config file and returns the double that matches the key "varName"
	 *Returns "0.0" if varName does not exist or cannot be parsed.
	 */
	double getValue(const std::string &varName);

	/*setString(varName,content)
	 *Sets the config variable with key "varName" to the value of "content"
	 */
	void setString(const std::string &varName, const std::string &content);

	/*setInt(varName,content)
	 *Sets the config variable with key "varName" to the value of "content"
	 */
	void setInt(const std::string &varName, int content);

	/**
	 * Sets a config variable to a given (double) value
	 * @param varName Name of the variable to set
	 * @param content Value to give the variable
	 */
	void setValue(const std::string &varName,double content);

	/**
	 * returns true if the key varName exists. This is used the first time 1.4.0
	 * starts so that it can see that it has to import configs from an earlier
	 * version.
	 * @param varName Name of the variable
	 * @return true if the variable exists
	 */
	bool exists(const std::string &varName) const;

	/*setDefault(varName,value)
	 *if the variable "varName" does not exist it will be created with value "value"
	 *if varName exists then this will have no effect
	 */
	/**
	 * Set default value for a variable. If the variable "varName" does not exist it will be created with value "value"
	 * if varName exists then this will have no effect
	 * @param varName Name of the variable
	 * @param content The default value
	 */
	void setDefault(const std::string &varName, const std::string &content);

	/**
	 * Should be set if the user has requested the program to shutdown.
	 * @param shuttingDown value of shutdown command. 5 = default = shutdown but allow saving
	 */
	void setShuttingDown(long shuttingDown = 5);

	/**
	 * tells if the user wants to shutdown. This can be used if the exit button is pressed deeply in the program.
	 * @return
	 */
	long isShuttingDown() const;
};

#endif	/* _COMMON_H */

