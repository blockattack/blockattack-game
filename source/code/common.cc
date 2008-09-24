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



commonTime getTotalTime()
{
    commonTime ct;
    string filename = getPathToSaveFiles()+"/totalTime";
    ifstream inFile(filename.c_str());
    if(inFile)
    {
        inFile >> ct.days;
        inFile >> ct.hours;
        inFile >> ct.minutes;
        inFile >> ct.seconds;
        inFile.close();
    }
    else
    {
        ct.days = 0;
        ct.hours = 0;
        ct.minutes = 0;
        ct.seconds = 0;
    }
    return ct;
}

commonTime addTotalTime(commonTime toAdd)
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
    
    string filename = getPathToSaveFiles()+"/totalTime";
    ofstream outFile(filename.c_str(),ios::trunc);
    if(outFile)
    {
        outFile << ct.days << endl;
        outFile << ct.hours << endl;
        outFile << ct.minutes << endl;
        outFile << ct.seconds << endl;
        outFile.close();
    }
    else
        cout << "Error writing total time to: " << filename << endl;
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
            if(key==previuskey)
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
        //outFile << statMap.size() << endl;
        map<string,string>::iterator iter;
        for(iter = configMap.begin(); iter != configMap.end(); iter++)
        {
            outFile << iter->first << " " << iter->second << endl;
        }
        outFile << "\n";
    }
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
