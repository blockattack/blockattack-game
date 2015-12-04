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
http://www.blockattack.net
===========================================================================
*/

#ifndef SCOPEHELPERS_HPP
#define	SCOPEHELPERS_HPP

class SDLUnicodeScope {
private:
	int oldStatus;
public:
	SDLUnicodeScope() {
		oldStatus = SDL_EnableUNICODE(1);
	}
	
	void Release() {
		SDL_EnableUNICODE(oldStatus);
	}
	
	~SDLUnicodeScope() {
		Release();
	}
};

#endif	/* SCOPEHELPERS_HPP */

