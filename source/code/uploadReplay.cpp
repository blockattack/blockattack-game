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

#include "uploadReplay.h"
#include "common.h"

bool isDownloading;
bool uploadReplayInitialized;
bool thresholdUp2Date;

SDL_Thread *threadThresholdFile;

void uploadReplay_init()
{
	if(uploadReplayInitialized)
		return;
	//mutDownload = SDL_CreateMutex();
	mutUpload = SDL_CreateMutex();
	thresholdUp2Date = false;
}

int downloadThresholdFile(void *ptr)
{
	//if(SDL_mutexP(mutDownload)==-1){
	//    isDownloading = false;
	//    return -1; //We don't crash if we can't lock. Just failing to do so
	//}
	FILE *outfile;
	CURL *easyhandle = curl_easy_init();
	if(!easyhandle)
	{
		isDownloading=false;
		//SDL_mutexV(mutDownload);
		return -1;
	}
	string filename = getPathToSaveFiles()+"/scoreThreshold";
	outfile = fopen(filename.c_str(), "w");

	curl_easy_setopt(easyhandle, CURLOPT_URL, "http://localhost/~poul/blockattackHighscores/result/threshold");
	curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, outfile);

	CURLcode res = curl_easy_perform(easyhandle);

	fclose(outfile);

	if(res == 200)
		thresholdUp2Date = true;

	curl_easy_cleanup(easyhandle);
	isDownloading=false;
	//if(SDL_mutexV(mutDownload)==-1){
	//return 0; //We don't crash if we can't lock. Just failing to do so
	//}
	return 0;
}

bool uploadReplay_canUpload()
{
	if(thresholdUp2Date)
		return true;
	if(isDownloading)
		return false;
	isDownloading=true; //We mark before start the thread
	threadThresholdFile = SDL_CreateThread(downloadThresholdFile, NULL);
	return thresholdUp2Date;
}
