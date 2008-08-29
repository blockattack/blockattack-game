// 
// File:   uploadReplay.h
// Author: poul
//
// Created on 26. februar 2008, 00:12
//

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

