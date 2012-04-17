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


#ifndef _UPLOADREPLAY_H
#define	_UPLOADREPLAY_H

#define HIGHSCORESERVER http://localhost/~poul/blockattackHighscores/result/

#include <curl/curl.h>
#include "SDL.h"

using namespace std;

SDL_mutex *mutDownload,*mutUpload;

bool uploadReplay_canUpload();

void uploadReplay_upload();


class OnlineHighscores
{

};

#endif	/* _UPLOADREPLAY_H */

