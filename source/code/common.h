/*
Block Attack - Rise of the Blocks, SDL game, besed on Nintendo's Tetris Attack
Copyright (C) 2008 Poul Sander

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Poul Sander
    R�vehjvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
    http://blockattack.sf.net
*/

/*
 *This is the common.h
 *It contains some basic functions that nearly all multiplatform games are going
 *to need.
 */

#ifndef _COMMON_H
#define	_COMMON_H

/*
 *Files will be saved in:
 * HOME/.gamesaves/"+GAMENAME (unix)
 *or DOCUMENTS/My Games/GAMENAME (Windows)
 */
#define GAMENAME "blockattack"

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <stdlib.h>
#ifdef WIN32
#include "windows.h"
#include "shlobj.h"
#endif

using namespace std;

struct commonTime{
    unsigned int days;
    unsigned int hours;
    unsigned int minutes;
    unsigned int seconds;
};

string itoa(int num);

string getPathToSaveFiles();

#ifdef WIN32
string getMyDocumentsPath();
#endif

class TimeHandler
{
public:
	static commonTime ms2ct(unsigned int milliseconds);
	
	static commonTime getTime(string name);
	
	static commonTime peekTime(string name, commonTime toAdd);
	
	static commonTime addTime(string name, commonTime toAdd);
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
    map<string,string> configMap;

    static Config *instance;
    
    void load();
protected:
    
    Config();
    
    
public:
    /*Config is a singleton.
     *It is accessed like this:
     *Config::getInstance()->method2call(paramters);
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
    string getString(string varName);
    
    /*getInt(varName)
     *Looks in the config file and returns the int that matches the key "varName"
     *Returns "0" if varName does not exist or cannot be parsed.
     */
    int getInt(string varName);
    
    /*getValue(varName)
     *Looks in the config file and returns the double that matches the key "varName"
     *Returns "0.0" if varName does not exist or cannot be parsed.
     */
    double getValue(string varName);
    
    /*setString(varName,content)
     *Sets the config variable with key "varName" to the value of "content"
     */
    void setString(string varName,string content);
    
    /*setInt(varName,content)
     *Sets the config variable with key "varName" to the value of "content"
     */
    void setInt(string varName,int content);
    
    /*setValue(varName,content)
     *Sets the config variable with key "varName" to the value of "content"
     */
    void setValue(string varName,double content);
    
    /*exists(varName)
     *returns true if the key varName exists. This is used the first time 1.4.0
     *starts so that it can see that it has to import configs from an earlier 
     *version.
     */
    bool exists(string varName);
    
    /*setDefault(varName,value)
     *if the variable "varName" does not exist it will be created with value "value"
     *if varName exists then this will have no effect
     */
    void setDefault(string varName, string content);
};

#endif	/* _COMMON_H */

