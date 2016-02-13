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
http://blockattack.net
===========================================================================
*/

//
// File:   stats.h
// Author: poul
//
// Created on 11. februar 2008, 20:19
//

#ifndef _STATS_H
#define	_STATS_H

#include <map>
#include <string>

class Stats
{
public:

	static Stats* getInstance();

	void save();

	unsigned int getNumberOf(const std::string &statName);

	void addOne(const std::string &statName);

	bool exists(const std::string &statName);

private:
	std::map<std::string,unsigned int> statMap;
	static Stats *instance;
	void load();
	Stats();
};

#endif	/* _STATS_H */

