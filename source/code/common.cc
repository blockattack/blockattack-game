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

#include "common.h"
#include <sstream>

//Function to convert numbers to string
string itoa(int num)
{
    stringstream converter;
    converter << num;
    return converter.str();
}

string double2str(double num)
{
    stringstream converter;
    converter << num;
    return converter.str();
}        

/*
 * str2double parses a string and returns a double with the value of the string
 * if the string is not a double then 0.0 is returned instead of throing an error
 * in that way this function will always return a useable value.
 */
double str2double(string str2parse)
{
    try{
        stringstream converter(str2parse);
        double val = 0.0;
        converter >> val;
        return val;
    }catch(ios_base::failure f)
    {
        return 0.0;
    }
}

/*
 * str2int parses a string and returns an int with the value of the string
 * if the string is not an int then 0 is returned instead of throing an error
 * in that way this function will always return a useable value.
 */
int str2int(string str2parse)
{
    try{
        stringstream converter(str2parse);
        int val = 0;
        converter >> val;
        return val;
    }catch(ios_base::failure f)
    {
        return 0;
    }
}

#ifdef WIN32
//Returns path to "my Documents" in windows:
string getMyDocumentsPath()
{
    TCHAR pszPath[MAX_PATH];
    if (SUCCEEDED(SHGetSpecialFolderPath(NULL, pszPath, CSIDL_PERSONAL, FALSE))) {
        // pszPath is now the path that you want
#if DEBUG
        cout << "MyDocuments Located: " << pszPath << endl;
#endif
        string theResult= pszPath;
        return theResult;
    }
    else
    {
        cout << "Warning: My Documents not found!" << endl;
        string theResult ="";
        return theResult;
    }
}

#endif

string getPathToSaveFiles()
{
#ifdef __unix__
    return (string)getenv("HOME")+(string)"/.gamesaves/"+GAMENAME;
#elif WIN32
    return getMyDocumentsPath()+(string)"/My Games/"+GAMENAME;
#else
    return ".";
#endif
}

commonTime ms2ct(unsigned int milliseconds)
{
    commonTime ct;
    ct.days = 0;
    unsigned int time = milliseconds;
    ct.hours = time/(1000*60*60);
    time = time % (1000*60*60);
    ct.minutes = time/(1000*60);
    time = time % (1000*60);
    ct.seconds = time/1000;
}

commonTime getTotalTime()
{
    commonTime ct;
    ct.days = Config::getInstance()->getInt("totalTimeDays");
    ct.hours = Config::getInstance()->getInt("totalTimeHours");
    ct.minutes = Config::getInstance()->getInt("totalTimeMinutes");
    ct.seconds = Config::getInstance()->getInt("totalTimeSeconds");
    return ct;
}

/*
 * peekTotalTime
 * Returns the total runtime with toAdd added but without writing it to config file. Used for stats
 */
commonTime peekTotalTime(commonTime toAdd)
{
    commonTime ct = getTotalTime();

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

/*
 * addTotalTime
 * Same as peekTotalTime but writes the time to the config file.
 * Should only be called once... when the program shuts down
 */
commonTime addTotalTime(commonTime toAdd)
{
    commonTime ct = peekTotalTime(toAdd);
    
    Config::getInstance()->setInt("totalTimeDays",ct.days);
    Config::getInstance()->setInt("totalTimeHours",ct.hours);
    Config::getInstance()->setInt("totalTimeMinutes",ct.minutes);
    Config::getInstance()->setInt("totalTimeSeconds",ct.seconds);
    return ct;
}

Config* Config::instance = 0;

Config::Config()
{
    configMap.clear();
    load();
}

void Config::load()
{
    string filename = getPathToSaveFiles()+"/configFile";
    ifstream inFile(filename.c_str());
    string key;
    string previuskey;
    char value[MAX_VAR_LENGTH];
    if(inFile)
    {
        while(!inFile.eof())
        {
            inFile >> key;
            if(key==previuskey) //the last entry will be read 2 times if a linebreak is missing in the end
                    continue; 
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

Config* Config::getInstance()
{
    if(Config::instance==0)
    {
        Config::instance = new Config();
        
    }
    return Config::instance; 
}

void Config::save()
{
    string filename = getPathToSaveFiles()+"/configFile";
    ofstream outFile(filename.c_str(),ios::trunc);
    
    if(outFile)
    {
        map<string,string>::iterator iter;
        for(iter = configMap.begin(); iter != configMap.end(); iter++)
        {
            outFile << iter->first << " " << iter->second << endl;
        }
        outFile << "\n"; //The last entry in the file will be read double if a linebreak is missing
        //This is checked on load too in case a user changes it himself.
    }
    outFile.close();
}

bool Config::exists(string varName)
{
    //Using that find returns an iterator to the end of the map if not found
    return configMap.find(varName) != configMap.end();
}

void Config::setString(string varName, string content)
{
    configMap[varName] = content;
}

void Config::setInt(string varName, int content)
{
    configMap[varName] = itoa(content);
}

void Config::setValue(string varName,double content)
{
    configMap[varName] = double2str(content);
}

string Config::getString(string varName)
{
    if(exists(varName))
    {
        return configMap[varName];
    }
    else
        return "";
}

int Config::getInt(string varName)
{
    if(exists(varName))
    {
        return str2int(configMap[varName]);
    }
    else
        return 0;
}

double Config::getValue(string varName)
{
    if(exists(varName))
    {
        return str2double(configMap[varName]);
    }
    else
        return 0.0;
}
