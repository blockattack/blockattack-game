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


#include <string.h>





#ifndef VERSION_NUMBER
    #define VERSION_NUMBER "version 1.4.1 BETA"
#endif

//If DEBUG is defined: AI info and FPS will be written to screen
#ifndef DEBUG
    #define DEBUG 0
#endif

//If NETWORK id defined: enet will be used
#ifndef NETWORK
    #define NETWORK 1
#endif

//Abstract layer is experimental and appears to cause trouble in some cercumstances. And it is not implemented
#ifndef USE_ABSTRACT_FS
    #define USE_ABSTRACT_FS 1
#endif

//Build-in level editor is still experimental!
#ifndef LEVELEDITOR
    #define LEVELEDITOR 0
#endif

//Macros to convert surfaces (ffor faster drawing)
#define CONVERT(n) tmp = SDL_DisplayFormat(n); SDL_FreeSurface(n); n = tmp
#define CONVERTA(n) tmp = SDL_DisplayFormatAlpha(n); SDL_FreeSurface(n); n = tmp

#include <iostream>
#include <stdlib.h>
#include <time.h>           //Used for srand()
#include <sstream>          //Still used by itoa2
#include <string>
#include "SDL.h"            //The SDL libary, used for most things
#include <SDL_mixer.h>      //Used for sound & music
#include <SDL_image.h>      //To load PNG images!
#if USE_ABSTRACT_FS
    #include <physfs.h>         //Abstract file system. To use containers
    #include "physfs_stream.hpp" //To use C++ style file streams
#endif
//#include "ttfont.h"        //To use True Type Fonts in SDL
//#include "config.h"
#include <vector>
#include <SDL/SDL_timer.h>
//#include "MenuSystem.h"

//if SHAREDIR is not used we look in current directory
#ifndef SHAREDIR
#define SHAREDIR "."
#endif



//enet things
#if NETWORK
#include "enet/enet.h"
#endif
//enet things end

#include "SFont.h"          //Used to write on screen
#include "highscore.h"      //Stores highscores
#include "ReadKeyboard.h"   //Reads text from keyboard
#include "joypad.h"         //Used for joypads
#include "listFiles.h"	    //Used to show files on screen
#include "replay.h"			//Used for replays
#include "stats.h"          //Saves general stats 
#if LEVELEDITOR
#include "editor/editorMain.hpp" //The level editor
#endif
//#include "uploadReplay.h"   //Takes care of everything libcurl related

#include "common.h"

/*******************************************************************************
* All variables and constant has been moved to mainVars.hpp for the overview.  *
*******************************************************************************/
#include "mainVars.hpp"

void MakeBackground(int,int);

void closeAllMenus()
{
    bNewGameOpen=false;          //show sub menues
    bOptionsOpen=false;          //Show OptionsMenu (Configure and Select Puzzle)
    b1playerOpen=false;			//show submenu
    b2playersOpen=false;
    bReplayOpen = false;
#if NETWORK
    bNetworkOpen = false;
#endif
    showOptions = false;

}


#if USE_ABSTRACT_FS
SDL_Surface * IMG_Load2(char* path)
{
    if (!PHYSFS_exists(path))
    {
        cout << "File not in blockattack.data: " << path << endl;
        return NULL; //file doesn't exist
    }

    PHYSFS_file* myfile = PHYSFS_openRead(path);

    // Get the lenght of the file
    unsigned int m_size = PHYSFS_fileLength(myfile);

    // Get the file data.
    char *m_data = new char[m_size];

    int length_read = PHYSFS_read (myfile, m_data, 1, m_size);

    if (length_read != (int)m_size)
    {
            delete [] m_data;
            m_data = 0;
            PHYSFS_close(myfile);
            cout << "Error. Curropt data file!" << endl;
            return NULL;
    }

    PHYSFS_close(myfile);

// And this is how you load an image from a memory buffer with SDL
    SDL_RWops *rw = SDL_RWFromMem (m_data, m_size);

    //The above might fail an return null.
    if(!rw)
    {
        delete [] m_data;
        m_data = 0;
        PHYSFS_close(myfile);
        cout << "Error. Curropt data file!" << endl;
        return NULL;
    }

    SDL_Surface* surface = IMG_Load_RW(rw,true); //the second argument tells the function to three RWops

    return surface;
}
#else
//Because we use the SHAREDIR we need:
SDL_Surface * IMG_Load2(char* path) {

    string tmp = "";
    SDL_Surface * ret=NULL;
    tmp = (string)""+sharedir+(string)"/"+path;
#if DEBUG
    cout << "loading " << tmp << endl;
#endif
    if (!(ret = IMG_Load(tmp.c_str())))
        ret = IMG_Load(path);
    return ret;
}
#endif

void UnloadImages();
int InitImages();

#if USE_ABSTRACT_FS
static string oldThemePath = "default";

void loadTheme(string themeName)
{
    if(themeName.compare("start")!=0)
        UnloadImages();
#if defined(__unix__)
    string home = (string)getenv("HOME")+(string)"/.gamesaves/blockattack";
#elif defined(_WIN32)
    string home = (string)getMyDocumentsPath()+(string)"/My Games/blockattack";
#endif
    //Remove old theme
    PHYSFS_removeFromSearchPath(oldThemePath.c_str());
    //Look in blockattack.data
    PHYSFS_addToSearchPath(((string)SHAREDIR+"/blockattack.data").c_str(), 1);
    //Look in folder
    PHYSFS_addToSearchPath(SHAREDIR, 1);
    //Look in home folder
    #if defined(__unix__) || defined(_WIN32)
    PHYSFS_addToSearchPath(home.c_str(), 1);
    #endif
    if(themeName.compare("default")==0 || (themeName.compare("start")==0))
    {
        InitImages();
        return; //Nothing more to do
    }
    oldThemePath = "themes/"+themeName;
    PHYSFS_addToSearchPath(oldThemePath.c_str(),0);
    #if defined(__unix__) || defined(_WIN32)
    PHYSFS_addToSearchPath((home+(string)"/"+oldThemePath).c_str(), 0);
    #endif
    InitImages();
}
#endif

/*TTF_Font * TTF_OpenFont2(char* path, int ptsize) {

    char * tmp;
    TTF_Font * ret=NULL;
    tmp = (char*)malloc (sizeof(char)*(strlen(path)+strlen(sharedir)+2));
    strcpy(tmp, sharedir);
    strcat(tmp, "/");
    strcat(tmp, path);
#if DEBUG
    printf("loading %s\n",tmp);
#endif
    if(!(TTF_WasInit()))
       TTF_Init();
    if (!(ret = TTF_OpenFont(tmp, ptsize)))
        ret = TTF_OpenFont(path, ptsize);
    if(!ret)
        cout << "failed to load font: " << TTF_GetError() << endl;
    free(tmp);
    return ret;
}*/

#if USE_ABSTRACT_FS
Mix_Music * Mix_LoadMUS2(char* path)
{
    if (!PHYSFS_exists(path))
    {
        cout << "File not in blockattack.data: " << path << endl;
        return NULL; //file doesn't exist
    }

    PHYSFS_file* myfile = PHYSFS_openRead(path);

    // Get the lenght of the file
    unsigned int m_size = PHYSFS_fileLength(myfile);

    // Get the file data.
    char *m_data = new char[m_size];

    int length_read = PHYSFS_read (myfile, m_data, 1, m_size);

    if (length_read != (int)m_size)
    {
            delete [] m_data;
            m_data = 0;
            PHYSFS_close(myfile);
            cout << "Error. Curropt data file!" << endl;
            return NULL;
    }

    PHYSFS_close(myfile);

// And this is how you load from a memory buffer with SDL
    SDL_RWops *rw = SDL_RWFromMem (m_data, m_size);

    //The above might fail an return null.
    if(!rw)
    {
        delete [] m_data;
        m_data = 0;
        PHYSFS_close(myfile);
        cout << "Error. Curropt data file!" << endl;
        return NULL;
    }

    Mix_Music* ret = Mix_LoadMUS_RW(rw);

    return ret;
}
#else
Mix_Music * Mix_LoadMUS2(char* path)
{
    Mix_Music * ret=NULL;
    string tmp = (string)""+sharedir+(string)"/"+path;
#if DEBUG
    cout << "loading " << tmp << endl;
#endif
    if (!(ret = Mix_LoadMUS(tmp.c_str())))
        ret = Mix_LoadMUS(path);

    return ret;
}
#endif

#if USE_ABSTRACT_FS
Mix_Chunk * Mix_LoadWAV2(char* path)
{
    if (!PHYSFS_exists(path))
    {
        cout << "File not in blockattack.data: " << path << endl;
        return NULL; //file doesn't exist
    }

    PHYSFS_file* myfile = PHYSFS_openRead(path);

    // Get the lenght of the file
    unsigned int m_size = PHYSFS_fileLength(myfile);

    // Get the file data.
    char *m_data = new char[m_size];

    int length_read = PHYSFS_read (myfile, m_data, 1, m_size);

    if (length_read != (int)m_size)
    {
            delete [] m_data;
            m_data = 0;
            PHYSFS_close(myfile);
            cout << "Error. Curropt data file!" << endl;
            return NULL;
    }

    PHYSFS_close(myfile);

// And this is how you load from a memory buffer with SDL
    SDL_RWops *rw = SDL_RWFromMem (m_data, m_size);

    //The above might fail an return null.
    if(!rw)
    {
        delete [] m_data;
        m_data = 0;
        PHYSFS_close(myfile);
        cout << "Error. Curropt data file!" << endl;
        return NULL;
    }

    Mix_Chunk* ret = Mix_LoadWAV_RW(rw,true); //the second argument tells the function to three RWops

    return ret;
}
#else
Mix_Chunk * Mix_LoadWAV2(char* path)
{
    Mix_Chunk * ret=NULL;
    string tmp = (string)""+sharedir+(string)"/"+path;
#if DEBUG
    cout << "loading " << tmp << endl;
#endif
    if (!(ret = Mix_LoadWAV(tmp.c_str())))
        ret = Mix_LoadWAV(path);

    return ret;
}
#endif

//Load all image files to memory
int InitImages()
{
    if (!((backgroundImage = IMG_Load2((char*)"gfx/background.png"))
            && (background = IMG_Load2((char*)"gfx/blackBackGround.png"))
            && (bNewGame = IMG_Load2((char*)"gfx/bNewGame.png"))
            && (b1player = IMG_Load2((char*)"gfx/bOnePlayer.png"))
            && (b2players = IMG_Load2((char*)"gfx/bTwoPlayers.png"))
            && (bVsMode = IMG_Load2((char*)"gfx/bVsGame.png"))
            && (bVsModeConfig = IMG_Load2((char*)"gfx/bVsGameConfig.png"))
            && (bPuzzle = IMG_Load2((char*)"gfx/bPuzzle.png"))
            && (bStageClear = IMG_Load2((char*)"gfx/bStageClear.png"))
            && (bTimeTrial = IMG_Load2((char*)"gfx/bTimeTrial.png"))
            && (bEndless = IMG_Load2((char*)"gfx/bEndless.png"))
            && (bOptions = IMG_Load2((char*)"gfx/bOptions.png"))
            && (bConfigure = IMG_Load2((char*)"gfx/bConfigure.png"))
            && (bSelectPuzzle = IMG_Load2((char*)"gfx/bSelectPuzzle.png"))
            && (bHighScore = IMG_Load2((char*)"gfx/bHighScore.png"))
            && (bExit = IMG_Load2((char*)"gfx/bExit.png"))
            && (bBack = IMG_Load2((char*)"gfx/bBack.png"))
            && (bForward = IMG_Load2((char*)"gfx/bForward.png"))
            && (bReplay = IMG_Load2((char*)"gfx/bReplays.png"))
            && (bSave = IMG_Load2((char*)"gfx/bSave.png"))
            && (bLoad = IMG_Load2((char*)"gfx/bLoad.png"))
#if NETWORK
            && (bNetwork = IMG_Load2((char*)"gfx/bNetwork.png"))
            && (bHost = IMG_Load2((char*)"gfx/bHost.png"))
            && (bConnect = IMG_Load2((char*)"gfx/bConnect.png"))
#endif
            && (blackLine = IMG_Load2((char*)"gfx/blackLine.png"))
            && (stageBobble = IMG_Load2((char*)"gfx/iStageClearLimit.png"))
            && (bricks[0] = IMG_Load2((char*)"gfx/bricks/blue.png"))
            && (bricks[1] = IMG_Load2((char*)"gfx/bricks/green.png"))
            && (bricks[2] = IMG_Load2((char*)"gfx/bricks/purple.png"))
            && (bricks[3] = IMG_Load2((char*)"gfx/bricks/red.png"))
            && (bricks[4] = IMG_Load2((char*)"gfx/bricks/turkish.png"))
            && (bricks[5] = IMG_Load2((char*)"gfx/bricks/yellow.png"))
            && (bricks[6] = IMG_Load2((char*)"gfx/bricks/grey.png"))
            && (crossover = IMG_Load2((char*)"gfx/crossover.png"))
            && (balls[0] = IMG_Load2((char*)"gfx/balls/ballBlue.png"))
            && (balls[1] = IMG_Load2((char*)"gfx/balls/ballGreen.png"))
            && (balls[2] = IMG_Load2((char*)"gfx/balls/ballPurple.png"))
            && (balls[3] = IMG_Load2((char*)"gfx/balls/ballRed.png"))
            && (balls[4] = IMG_Load2((char*)"gfx/balls/ballTurkish.png"))
            && (balls[5] = IMG_Load2((char*)"gfx/balls/ballYellow.png"))
            && (balls[6] = IMG_Load2((char*)"gfx/balls/ballGray.png"))
            && (cursor[0] = IMG_Load2((char*)"gfx/animations/cursor/1.png"))
            && (cursor[1] = IMG_Load2((char*)"gfx/animations/cursor/2.png"))
            && (bomb[0] = IMG_Load2((char*)"gfx/animations/bomb/bomb_1.png"))
            && (bomb[1] = IMG_Load2((char*)"gfx/animations/bomb/bomb_2.png"))
            && (ready[0] = IMG_Load2((char*)"gfx/animations/ready/ready_1.png"))
            && (ready[1] = IMG_Load2((char*)"gfx/animations/ready/ready_2.png"))
            && (explosion[0] = IMG_Load2((char*)"gfx/animations/explosion/0.png"))
            && (explosion[1] = IMG_Load2((char*)"gfx/animations/explosion/1.png"))
            && (explosion[2] = IMG_Load2((char*)"gfx/animations/explosion/2.png"))
            && (explosion[3] = IMG_Load2((char*)"gfx/animations/explosion/3.png"))
            && (counter[0] = IMG_Load2((char*)"gfx/counter/1.png"))
            && (counter[1] = IMG_Load2((char*)"gfx/counter/2.png"))
            && (counter[2] = IMG_Load2((char*)"gfx/counter/3.png"))
            && (backBoard = IMG_Load2((char*)"gfx/BackBoard.png")) //not used, we just test if it exists :)
            && (iGameOver = IMG_Load2((char*)"gfx/iGameOver.png"))
            && (iWinner = IMG_Load2((char*)"gfx/iWinner.png"))
            && (iDraw = IMG_Load2((char*)"gfx/iDraw.png"))
            && (iLoser = IMG_Load2((char*)"gfx/iLoser.png"))
            && (iChainBack = IMG_Load2((char*)"gfx/chainFrame.png"))
            && (iBlueFont = IMG_Load2((char*)"gfx/24P_Arial_Blue.png"))
            && (iSmallFont = IMG_Load2((char*)"gfx/14P_Arial_Angle_Red.png"))
            && (optionsBack = IMG_Load2((char*)"gfx/options.png"))
            && (bOn = IMG_Load2((char*)"gfx/bOn.png"))
            && (bOff = IMG_Load2((char*)"gfx/bOff.png"))
            && (bChange = IMG_Load2((char*)"gfx/bChange.png"))
            && (b1024 = IMG_Load2((char*)"gfx/b1024.png"))
            && (dialogBox = IMG_Load2((char*)"gfx/dialogbox.png"))
//	&& (fileDialogBox = IMG_Load2("gfx/fileDialogbox.png"))
            && (iLevelCheck = IMG_Load2((char*)"gfx/iLevelCheck.png"))
            && (iLevelCheckBox = IMG_Load2((char*)"gfx/iLevelCheckBox.png"))
            && (iCheckBoxArea = IMG_Load2((char*)"gfx/iCheckBoxArea.png"))
            && (boardBackBack = IMG_Load2((char*)"gfx/boardBackBack.png"))
            && (changeButtonsBack = IMG_Load2((char*)"gfx/changeButtonsBack.png"))
            && (garbageTL = IMG_Load2((char*)"gfx/garbage/garbageTL.png"))
            && (garbageT = IMG_Load2((char*)"gfx/garbage/garbageT.png"))
            && (garbageTR = IMG_Load2((char*)"gfx/garbage/garbageTR.png"))
            && (garbageR = IMG_Load2((char*)"gfx/garbage/garbageR.png"))
            && (garbageBR = IMG_Load2((char*)"gfx/garbage/garbageBR.png"))
            && (garbageB = IMG_Load2((char*)"gfx/garbage/garbageB.png"))
            && (garbageBL = IMG_Load2((char*)"gfx/garbage/garbageBL.png"))
            && (garbageL = IMG_Load2((char*)"gfx/garbage/garbageL.png"))
            && (garbageFill = IMG_Load2((char*)"gfx/garbage/garbageFill.png"))
            && (garbageML = IMG_Load2((char*)"gfx/garbage/garbageML.png"))
            && (garbageM = IMG_Load2((char*)"gfx/garbage/garbageM.png"))
            && (garbageMR = IMG_Load2((char*)"gfx/garbage/garbageMR.png"))
            && (garbageGM = IMG_Load2((char*)"gfx/garbage/garbageGM.png"))
            && (garbageGML = IMG_Load2((char*)"gfx/garbage/garbageGML.png"))
            && (garbageGMR = IMG_Load2((char*)"gfx/garbage/garbageGMR.png"))
            && (smiley[0] = IMG_Load2((char*)"gfx/smileys/0.png"))
            && (smiley[1] = IMG_Load2((char*)"gfx/smileys/1.png"))
            && (smiley[2] = IMG_Load2((char*)"gfx/smileys/2.png"))
            && (smiley[3] = IMG_Load2((char*)"gfx/smileys/3.png"))
            //new in 1.3.2
            && (transCover = IMG_Load2((char*)"gfx/transCover.png"))
            #if LEVELEDITOR
            && (bCreateFile = IMG_Load2((char*)"gfx/editor/bCreateFile.png"))
            && (bDeletePuzzle = IMG_Load2((char*)"gfx/editor/bDeletePuzzle.png"))
            && (bLoadFile = IMG_Load2((char*)"gfx/editor/bLoadFile.png"))
            && (bMoveBack = IMG_Load2((char*)"gfx/editor/bMoveBack.png"))
            && (bMoveDown = IMG_Load2((char*)"gfx/editor/bMoveDown.png"))
            && (bMoveForward = IMG_Load2((char*)"gfx/editor/bMoveForward.png"))
            && (bMoveLeft = IMG_Load2((char*)"gfx/editor/bMoveLeft.png"))
            && (bMoveRight = IMG_Load2((char*)"gfx/editor/bMoveRight.png"))
            && (bMoveUp = IMG_Load2((char*)"gfx/editor/bMoveUp.png"))
            && (bNewPuzzle = IMG_Load2((char*)"gfx/editor/bNewPuzzle.png"))
            && (bSaveFileAs = IMG_Load2((char*)"gfx/editor/bSaveFileAs.png"))
            && (bSavePuzzle = IMG_Load2((char*)"gfx/editor/bSavePuzzle.png"))
            && (bSaveToFile = IMG_Load2((char*)"gfx/editor/bSaveToFile.png"))
            && (bTestPuzzle = IMG_Load2((char*)"gfx/editor/bTestPuzzle.png"))
            #endif
            //end new in 1.3.2
            //new in 1.4.0
            && (bTheme = IMG_Load2((char*)"gfx/bTheme.png"))
            && (bSkip = IMG_Load2((char*)"gfx/bSkip.png"))
            && (bNext = IMG_Load2((char*)"gfx/bNext.png"))
            && (bRetry = IMG_Load2((char*)"gfx/bRetry.png"))
            //&& (menuMarked = IMG_Load2((char*)"gfx/menu/marked.png"))
            //&& (menuUnmarked = IMG_Load2((char*)"gfx/menu/unmarked.png"))
            //end new in 1.4.0
            && (mouse = IMG_Load2((char*)"gfx/mouse.png"))
         ))
        //if there was a problem ie. "File not found"
    {
        cout << "Error loading image file: " << SDL_GetError() << endl;
        exit(1);
    }


    //Prepare for fast blittering!
    CONVERT(background);
    CONVERT(bNewGame);
    CONVERT(backgroundImage);
    CONVERT(b1player);
    CONVERT(b2players);
    CONVERT(bVsMode);
    CONVERT(bVsModeConfig);
    CONVERT(bPuzzle);
    CONVERT(bStageClear);
    CONVERT(bTimeTrial);
    CONVERT(bEndless);
    CONVERT(bOptions);
    CONVERTA(bConfigure);
    CONVERTA(bSelectPuzzle);
    CONVERTA(bReplay);
    CONVERTA(bSave);
    CONVERTA(bLoad);
    CONVERTA(bTheme);
    CONVERTA(bSkip);
    CONVERTA(bRetry);
    CONVERTA(bNext);
    //CONVERTA(menuMarked);
    //CONVERTA(menuUnmarked);
#if NETWORK
    CONVERTA(bNetwork);
    CONVERTA(bHost);
    CONVERTA(bConnect);
#endif
    CONVERT(bHighScore);
    CONVERTA(boardBackBack);
    CONVERT(backBoard);
    CONVERT(blackLine);
    CONVERTA(changeButtonsBack);
    CONVERTA(cursor[0]);
    CONVERTA(cursor[1]);
    CONVERTA(counter[0]);
    CONVERTA(counter[1]);
    CONVERTA(counter[2]);
    CONVERTA(optionsBack);
    CONVERT(bExit);
    CONVERT(bOn);
    CONVERT(bOff);
    CONVERT(bChange);
    CONVERT(b1024);
    CONVERTA(dialogBox);
//	CONVERTA(fileDialogBox);
    CONVERTA(iLevelCheck);
    CONVERT(iLevelCheckBox);
    CONVERTA(iCheckBoxArea);
    for (int i = 0;i<4;i++)
    {
        CONVERTA(explosion[i]);
    }
    for (int i = 0; i<7; i++)
    {
        CONVERTA(bricks[i]);
        CONVERTA(balls[i]);
    }
    CONVERTA(crossover);
    CONVERTA(garbageTL);
    CONVERTA(garbageT);
    CONVERTA(garbageTR);
    CONVERTA(garbageR);
    CONVERTA(garbageBR);
    CONVERTA(garbageB);
    CONVERTA(garbageBL);
    CONVERTA(garbageL);
    CONVERTA(garbageFill);
    CONVERTA(garbageML);
    CONVERTA(garbageMR);
    CONVERTA(garbageM);
    CONVERTA(garbageGML);
    CONVERTA(garbageGMR);
    CONVERTA(garbageGM);
    CONVERTA(smiley[0]);
    CONVERTA(smiley[1]);
    CONVERTA(smiley[2]);
    CONVERTA(smiley[3]);
    CONVERTA(iWinner);
    CONVERTA(iDraw);
    CONVERTA(iLoser);
    CONVERTA(iChainBack);
    CONVERTA(iBlueFont);
    CONVERTA(iSmallFont);
    CONVERTA(iGameOver);
    CONVERTA(mouse);
    CONVERTA(stageBobble);
    CONVERTA(transCover);
    //Editor:
    #if LEVELEDITOR
    CONVERTA(bCreateFile);
    CONVERTA(bDeletePuzzle);
    CONVERTA(bLoadFile);
    CONVERTA(bMoveBack);
    CONVERTA(bMoveDown);
    CONVERTA(bMoveForward);
    CONVERTA(bMoveLeft);
    CONVERTA(bMoveRight);
    CONVERTA(bMoveUp);
    CONVERTA(bNewPuzzle);
    CONVERTA(bSaveFileAs);
    CONVERTA(bSavePuzzle);
    CONVERTA(bSaveToFile);
    CONVERTA(bTestPuzzle);
    #endif
    
    //Here comes the fonts:
    fBlueFont = SFont_InitFont(iBlueFont);
    fSmallFont = SFont_InitFont(iSmallFont);
    
    //And the ttf font:
    /*TTF_Font *ttFont1 = TTF_OpenFont2((char*)"fonts/FreeSerif.ttf", 24);
    TTF_SetFontStyle(ttFont1,TTF_STYLE_BOLD);
    ttfont = TTFont(ttFont1);*/

//Loads the sound if sound present
    if (!NoSound)
    {
        //And here the music:
        bgMusic = Mix_LoadMUS2((char*)"music/bgMusic.ogg");
        highbeatMusic = Mix_LoadMUS2((char*)"music/highbeat.ogg");
        //the music... we just hope it exists, else the user won't hear anything
        //Same goes for the sounds
        boing = Mix_LoadWAV2((char*)"sound/pop.ogg");
        applause = Mix_LoadWAV2((char*)"sound/applause.ogg");
        photoClick = Mix_LoadWAV2((char*)"sound/cameraclick.ogg");
        typingChunk = Mix_LoadWAV2((char*)"sound/typing.ogg");
        counterChunk = Mix_LoadWAV2((char*)"sound/counter.ogg");
        counterFinalChunk = Mix_LoadWAV2((char*)"sound/counterFinal.ogg");
    } //All sound has been loaded or not
    return 0;
} //InitImages()


//Unload images and fonts and sounds
void UnloadImages()
{
    cout << "Unloading data..." << endl;
    //Fonts and Sounds needs to be freed
    SFont_FreeFont(fBlueFont);
    SFont_FreeFont(fSmallFont);
    if (!NoSound) //Only unload then it has been loaded!
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bgMusic);
        Mix_FreeMusic(highbeatMusic);
        Mix_FreeChunk(boing);
        Mix_FreeChunk(applause);
        Mix_FreeChunk(photoClick);
        Mix_FreeChunk(counterChunk);
        Mix_FreeChunk(counterFinalChunk);
        Mix_FreeChunk(typingChunk);
    }
    //Free surfaces:
    //I think this will crash, at least it happend to me...
    //Chrashes no more. Caused by an undocumented double free
    SDL_FreeSurface(backgroundImage);
    SDL_FreeSurface(background);
    SDL_FreeSurface(bNewGame);
    SDL_FreeSurface(b1player);
    SDL_FreeSurface(b2players);
    SDL_FreeSurface(bVsMode);
    SDL_FreeSurface(bVsModeConfig);
    SDL_FreeSurface(bPuzzle);
    SDL_FreeSurface(bStageClear);
    SDL_FreeSurface(bTimeTrial);
    SDL_FreeSurface(bEndless);
    SDL_FreeSurface(bOptions);
    SDL_FreeSurface(bConfigure);
    SDL_FreeSurface(bSelectPuzzle);
    SDL_FreeSurface(bHighScore);
    SDL_FreeSurface(bReplay);
    SDL_FreeSurface(bSave);
    SDL_FreeSurface(bLoad);
    #if NETWORK
    SDL_FreeSurface(bNetwork);
    SDL_FreeSurface(bHost);
    SDL_FreeSurface(bConnect);
    #endif
    SDL_FreeSurface(bExit);
    SDL_FreeSurface(blackLine);
    SDL_FreeSurface(stageBobble);
    SDL_FreeSurface(bricks[0]);
    SDL_FreeSurface(bricks[1]);
    SDL_FreeSurface(bricks[2]);
    SDL_FreeSurface(bricks[3]);
    SDL_FreeSurface(bricks[4]);
    SDL_FreeSurface(bricks[5]);
    SDL_FreeSurface(bricks[6]);
    SDL_FreeSurface(crossover);
    SDL_FreeSurface(balls[0]);
    SDL_FreeSurface(balls[1]);
    SDL_FreeSurface(balls[2]);
    SDL_FreeSurface(balls[3]);
    SDL_FreeSurface(balls[4]);
    SDL_FreeSurface(balls[5]);
    SDL_FreeSurface(balls[6]);
    SDL_FreeSurface(cursor[0]);
    SDL_FreeSurface(cursor[1]);
    SDL_FreeSurface(backBoard); //not used, we just test if it exists :)
    SDL_FreeSurface(iGameOver);
    SDL_FreeSurface(iWinner);
    SDL_FreeSurface(iDraw);
    SDL_FreeSurface(iLoser);
    SDL_FreeSurface(iChainBack);
    //SDL_FreeSurface(iBlueFont); //Segfault
    //SDL_FreeSurface(iSmallFont); //Segfault
    SDL_FreeSurface(optionsBack);
    SDL_FreeSurface(bOn);
    SDL_FreeSurface(bOff);
    SDL_FreeSurface(bChange);
    SDL_FreeSurface(b1024);
    SDL_FreeSurface(dialogBox);
    //SDL_FreeSurface(fileDialogBox);
    SDL_FreeSurface(iLevelCheck);
    SDL_FreeSurface(iLevelCheckBox);
    SDL_FreeSurface(iCheckBoxArea);
    SDL_FreeSurface(boardBackBack);
    SDL_FreeSurface(changeButtonsBack);
    SDL_FreeSurface(garbageTL);
    SDL_FreeSurface(garbageT);
    SDL_FreeSurface(garbageTR);
    SDL_FreeSurface(garbageR);
    SDL_FreeSurface(garbageBR);
    SDL_FreeSurface(garbageB);
    SDL_FreeSurface(garbageBL);
    SDL_FreeSurface(garbageL);
    SDL_FreeSurface(garbageFill);
    SDL_FreeSurface(garbageML);
    SDL_FreeSurface(garbageM);
    SDL_FreeSurface(garbageMR);
    SDL_FreeSurface(garbageGML);
    SDL_FreeSurface(garbageGM);
    SDL_FreeSurface(garbageGMR);
    SDL_FreeSurface(smiley[0]);
    SDL_FreeSurface(smiley[1]);
    SDL_FreeSurface(smiley[2]);
    SDL_FreeSurface(smiley[3]);
    SDL_FreeSurface(transCover);
    SDL_FreeSurface(mouse);
    
}

//Function to convert numbers to string
/*string itoa(int num)
{
    stringstream converter;
    converter << num;
    return converter.str();
}*/

//Function to convert numbers to string (2 diget)
string itoa2(int num)
{
    stringstream converter;
    if(num<10)
        converter << "0";
    converter << num;
    return converter.str();
}

/*Loads all the puzzle levels*/
int LoadPuzzleStages()
{
    //if(puzzleLoaded)
    //    return 1;
#if USE_ABSTRACT_FS
    if (!PHYSFS_exists(("puzzles/"+puzzleName).c_str()))
    {
        cout << "File not in blockattack.data: " << ("puzzles/"+puzzleName) << endl;
        return -1; //file doesn't exist
    }
    PhysFS::ifstream inFile(("puzzles/"+puzzleName).c_str());
#else
#ifdef __unix__
    string filename0 = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/puzzles/";
    filename0 = filename0+puzzleName;
    if (singlePuzzle)
        filename0 = singlePuzzleFile;
#endif //__unix__
    string filename = (string)SHAREDIR+(string)"/puzzles/";
    filename = filename+puzzleName;
#ifdef __unix__
    ifstream inFile(filename0.c_str());
    if (!inFile)
        inFile.open(filename.c_str());
#else
    ifstream inFile(filename.c_str());
#endif
#endif

    inFile >> nrOfPuzzles;
    if (nrOfPuzzles>maxNrOfPuzzleStages)
        nrOfPuzzles=maxNrOfPuzzleStages;
    for (int k=0; (k<nrOfPuzzles) /*&&(!inFile.eof())*/ ; k++)
    {
        inFile >> nrOfMovesAllowed[k];
        for (int i=11;i>=0;i--)
            for (int j=0;j<6;j++)
            {
                inFile >> puzzleLevels[k][j][i];
            }
    }
    puzzleLoaded = true;
    return 0;
}

/*Draws a image from on a given Surface. Takes source image, destination surface and coordinates*/
inline void DrawIMG(SDL_Surface *img, SDL_Surface *target, int x, int y)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_BlitSurface(img, NULL, target, &dest);
}

/*Draws a part of an image on a surface of choice*/
void DrawIMG(SDL_Surface *img, SDL_Surface * target, int x, int y, int w, int h, int x2, int y2)
{
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_Rect dest2;
    dest2.x = x2;
    dest2.y = y2;
    dest2.w = w;
    dest2.h = h;
    SDL_BlitSurface(img, &dest2, target, &dest);
}

//Menu
/*void PrintHi()
{
    cout << "Hi" <<endl;
}

void InitMenues()
{
    ButtonGfx::setSurfaces(&menuMarked,&menuUnmarked);
    ButtonGfx::ttf = &ttfont;
}

void MainMenu()
{
    Menu m(&screen,true);
    Button bHi;
    bHi.setLabel("Write hi");
    bHi.setAction(PrintHi);
    m.addButton(bHi);
    m.run();
}*/

//The small things that are faaling when you clear something
class aBall
{
private:
    double x;
    double y;
    double velocityY;
    double velocityX;
    int color;
    unsigned long int lastTime;
public:

    aBall()
    {}

    //constructor:
    aBall(int X, int Y, bool right, int coulor)
    {
        double tal = 1.0;
        velocityY = -tal*startVelocityY;
        lastTime = currentTime;
        x = (double)X;
        y = (double)Y;
        color = coulor;
        if (right)
            velocityX = tal*VelocityX;
        else
            velocityX = -tal*VelocityX;
    }  //constructor

    //Deconstructor
    ~aBall()
    {
    }   //Deconstructor

    void update()
    {
        double timePassed = (((double)(currentTime-lastTime))/1000.0);  //time passed in seconds
        x = x+timePassed*velocityX;
        y = y+timePassed*velocityY;
        velocityY = velocityY + gravity*timePassed;
        if (y<1.0)
            velocityY=10.0;
        if ((velocityY>minVelocity) && (y>(double)(768-ballSize)) && (y<768.0))
        {
            velocityY = -0.70*velocityY;
            y = 768.0-ballSize;
        }
        lastTime = currentTime;
    }

    int getX()
    {
        return (int)x;
    }

    int getY()
    {
        return (int)y;
    }

    int getColor()
    {
        return color;
    }
};  //aBall

const int maxNumberOfBalls = 6*12*2*2;

class ballManeger
{
public:
    aBall ballArray[maxNumberOfBalls];
    bool ballUsed[maxNumberOfBalls];
    //The old ball information is also saved so balls can be deleted!
    aBall oldBallArray[maxNumberOfBalls];
    bool oldBallUsed[maxNumberOfBalls];

    ballManeger()
    {
        for (int i=0; i<maxNumberOfBalls; i++)
        {
            ballUsed[i] = false;
            oldBallUsed[i] = false;
        }
    }

    //Adds a ball to the screen at given coordiantes, traveling right or not with color
    int addBall(int x, int y,bool right,int color)
    {
        int ballNumber = 0;
        //Find a free ball
        while ((ballUsed[ballNumber])&&(ballNumber<maxNumberOfBalls))
            ballNumber++;
        //Could not find a free ball, return -1
        if (ballNumber==maxNumberOfBalls)
            return -1;
        currentTime = SDL_GetTicks();
        ballArray[ballNumber] = aBall(x,y,right,color);
        ballUsed[ballNumber] = true;
        return 1;
    }  //addBall

    void update()
    {
        currentTime = SDL_GetTicks();
        for (int i = 0; i<maxNumberOfBalls; i++)
        {

            if (ballUsed[i])
            {
                oldBallUsed[i] = true;
                oldBallArray[i] = ballArray[i];
                ballArray[i].update();
                if (ballArray[i].getY()>800 || ballArray[i].getX()>xsize || ballArray[i].getX()<-ballSize)
                {
                    ballArray[i].~aBall();
                    ballUsed[i] = false;
                    //cout << "Ball removed" << endl;
                }
            }
            else
            {
                oldBallUsed[i] = false;
            }
        }
    } //update


}; //theBallManeger

ballManeger theBallManeger;

//a explosions, non moving
class anExplosion
{
private:
    int x;
    int y;
    Uint8 frameNumber;
#define frameLength 80
    //How long an image in an animation should be showed
#define maxFrame 4
    //How many images there are in the animation
    unsigned long int placeTime; //Then the explosion occored
public:

    anExplosion()
    {}

    //constructor:
    anExplosion(int X, int Y)
    {
        placeTime = currentTime;
        x = X;
        y = Y;
        frameNumber=0;
    }  //constructor

    //Deconstructor
    ~anExplosion()
    {
    }   //Deconstructor

    //true if animation has played and object should be removed from the screen
    bool removeMe()
    {
        frameNumber = (currentTime-placeTime)/frameLength;
        return (!(frameNumber<maxFrame));
    }

    int getX()
    {
        return (int)x;
    }

    int getY()
    {
        return (int)y;
    }

    int getFrame()
    {
        return frameNumber;
    }
};  //nExplosion

class explosionManeger
{
public:
    anExplosion explosionArray[maxNumberOfBalls];
    bool explosionUsed[maxNumberOfBalls];
    //The old explosion information is also saved so explosions can be deleted!
    anExplosion oldExplosionArray[maxNumberOfBalls];
    bool oldExplosionUsed[maxNumberOfBalls];

    explosionManeger()
    {
        for (int i=0; i<maxNumberOfBalls; i++)
        {
            explosionUsed[i] = false;
            oldExplosionUsed[i] = false;
        }
    }

    int addExplosion(int x, int y)
    {
        //cout << "Tries to add an explosion" << endl;
        int explosionNumber = 0;
        while ((explosionUsed[explosionNumber])&&(explosionNumber<maxNumberOfBalls))
            explosionNumber++;
        if (explosionNumber==maxNumberOfBalls)
            return -1;
        currentTime = SDL_GetTicks();
        explosionArray[explosionNumber] = anExplosion(x,y);
        explosionUsed[explosionNumber] = true;
        //cout << "Explosion added" << endl;
        return 1;
    }  //addBall

    void update()
    {
        currentTime = SDL_GetTicks();
        for (int i = 0; i<maxNumberOfBalls; i++)
        {

            if (explosionUsed[i])
            {
                oldExplosionUsed[i] = true;
                oldExplosionArray[i] = explosionArray[i];
                if (explosionArray[i].removeMe())
                {
                    explosionArray[i].~anExplosion();
                    explosionUsed[i] = false;
                    //cout << "Explosion removed" << endl;
                }
            }
            else
            {
                oldExplosionUsed[i] = false;
            }
        }
    } //update


}; //explosionManeger

explosionManeger theExplosionManeger;

//text pop-up
class textMessage
{
private:
    int x;
    int y;
    char textt[10];
    unsigned long int time;
    unsigned long int placeTime; //Then the text was placed
public:

    textMessage()
    {}

    //constructor:
    textMessage(int X, int Y,const char* Text,unsigned int Time)
    {
        placeTime = currentTime;
        x = X;
        y = Y;
        strncpy(textt,Text,10);
        textt[9]=0;
        time = Time;
    }  //constructor

    //true if the text has expired
    bool removeMe()
    {
        return currentTime-placeTime>time;
    }

    int getX()
    {
        return x;
    }

    int getY()
    {
        return y;
    }

    char* getText()
    {
        return textt;
    }
};  //text popup

class textManeger
{
public:
    textMessage textArray[maxNumberOfBalls];
    bool textUsed[maxNumberOfBalls];
    //The old text information is also saved so text can be deleted!
    textMessage oldTextArray[maxNumberOfBalls];
    bool oldTextUsed[maxNumberOfBalls];

    textManeger()
    {
        for (int i=0; i<maxNumberOfBalls; i++)
        {
            textUsed[i] = false;
            oldTextUsed[i] = false;
        }
    }

    int addText(int x, int y,string Text,unsigned int Time)
    {
        //cout << "Tries to add text" << endl;
        int textNumber = 0;
        while ((textNumber<maxNumberOfBalls)&&((textUsed[textNumber])||(oldTextUsed[textNumber])))
            textNumber++;
        if (textNumber==maxNumberOfBalls)
            return -1;
        //cout << "adding to: " << textNumber << ":" << textUsed[textNumber] << ":" << &textArray[textNumber] << endl;
        currentTime = SDL_GetTicks();
        textArray[textNumber] = textMessage(x,y,Text.c_str(),Time);
        textUsed[textNumber] = true;
        return 1;
    }  //addText

    void update()
    {
        //cout << "Running update" << endl;
        currentTime = SDL_GetTicks();
        for (int i = 0; i<maxNumberOfBalls; i++)
        {

            if (textUsed[i])
            {
                if (!oldTextUsed[i])
                {
                    oldTextUsed[i] = true;
                    oldTextArray[i] = textMessage(textArray[i]);
                }
                if (textArray[i].removeMe())
                {
                    textArray[i].~textMessage();
                    textUsed[i] = false;
                }
            }
            else
                if (oldTextUsed[i])
                {
                    oldTextUsed[i] = false;
                    oldTextArray[i].~textMessage();
                }
        }
    } //update


}; //textManeger

textManeger theTextManeger;

//Here comes the Block Game object
#include "BlockGame.hpp"
#include "SFont.h"

//writeScreenShot saves the screen as a bmp file, it uses the time to get a unique filename
void writeScreenShot()
{
    cout << "Saving screenshot" << endl;
    int rightNow = (int)time(NULL);
/*#if defined(__unix__)
    char buf[514];
    sprintf( buf, "%s/.gamesaves/blockattack/screenshots/screenshot%i.bmp", getenv("HOME"), rightNow );
#elif defined(__win32__)
    char buf[MAX_PATH];
    sprintf( buf, "%s\\My Games\\blockattack\\screenshots\\screenshot%i.bmp", (getMyDocumentsPath()).c_str(), rightNow );
#else
    char buf[MAX_PATH];
    sprintf( buf, "screenshot%i.bmp", rightNow );
#endif*/
#if defined(__unix__)
    string buf = (string)getenv("HOME")+"/.gamesaves/blockattack/screenshots/screenshot"+itoa(rightNow)+".bmp";
#elif defined(__win32__)
    string buf = getMyDocumentsPath()+"\\My Games\\blockattack\\screenshots\\screenshot"+itoa(rightNow)+".bmp";
#else
    string buf = "screenshot"+itoa(rightNow)+".bmp";
#endif
    SDL_SaveBMP( screen, buf.c_str() );
    if (!NoSound)
        if (SoundEnabled)Mix_PlayChannel(1,photoClick,0);
}

//Function to return the name of a key, to be displayed...
string getKeyName(SDLKey key)
{
    string keyname;
    char charToPut = '\0';
    switch (key)
    {
    case SDLK_a:
        charToPut = 'A';
        break;
    case SDLK_b:
        charToPut = 'B';
        break;
    case SDLK_c:
        charToPut = 'C';
        break;
    case SDLK_d:
        charToPut = 'D';
        break;
    case SDLK_e:
        charToPut = 'E';
        break;
    case SDLK_f:
        charToPut = 'F';
        break;
    case SDLK_g:
        charToPut = 'G';
        break;
    case SDLK_h:
        charToPut = 'H';
        break;
    case SDLK_i:
        charToPut = 'I';
        break;
    case SDLK_j:
        charToPut = 'J';
        break;
    case SDLK_k:
        charToPut = 'K';
        break;
    case SDLK_l:
        charToPut = 'L';
        break;
    case SDLK_m:
        charToPut = 'M';
        break;
    case SDLK_n:
        charToPut = 'N';
        break;
    case SDLK_o:
        charToPut = 'O';
        break;
    case SDLK_p:
        charToPut = 'P';
        break;
    case SDLK_q:
        charToPut = 'Q';
        break;
    case SDLK_r:
        charToPut = 'R';
        break;
    case SDLK_s:
        charToPut = 'S';
        break;
    case SDLK_t:
        charToPut = 'T';
        break;
    case SDLK_u:
        charToPut = 'U';
        break;
    case SDLK_v:
        charToPut = 'V';
        break;
    case SDLK_w:
        charToPut = 'W';
        break;
    case SDLK_x:
        charToPut = 'X';
        break;
    case SDLK_y:
        charToPut = 'Y';
        break;
    case SDLK_z:
        charToPut = 'Z';
        break;
    case SDLK_0:
        charToPut = '0';
        break;
    case SDLK_1:
        charToPut = '1';
        break;
    case SDLK_2:
        charToPut = '2';
        break;
    case SDLK_3:
        charToPut = '3';
        break;
    case SDLK_4:
        charToPut = '4';
        break;
    case SDLK_5:
        charToPut = '5';
        break;
    case SDLK_6:
        charToPut = '6';
        break;
    case SDLK_7:
        charToPut = '7';
        break;
    case SDLK_8:
        charToPut = '8';
        break;
    case SDLK_9:
        charToPut = '9';
        break;
    case SDLK_KP0:
        keyname = "NP_0";
        break;
    case SDLK_KP1:
        keyname = "NP_1";
        break;
    case SDLK_KP2:
        keyname = "NP_2";
        break;
    case SDLK_KP3:
        keyname = "NP_3";
        break;
    case SDLK_KP4:
        keyname = "NP_4";
        break;
    case SDLK_KP5:
        keyname = "NP_5";
        break;
    case SDLK_KP6:
        keyname = "NP_6";
        break;
    case SDLK_KP7:
        keyname = "NP_7";
        break;
    case SDLK_KP8:
        keyname = "NP_8";
        break;
    case SDLK_KP9:
        keyname = "NP_9";
        break;
    case SDLK_BACKSPACE:
        keyname = "Backspace";
        break;
    case SDLK_TAB:
        keyname = "Tab";
        break;
    case SDLK_CLEAR:
        keyname = "Clear";
        break;
    case SDLK_RETURN:
        keyname = "Return";
        break;
    case SDLK_PAUSE:
        keyname = "Pause";
        break;
    case SDLK_SPACE:
        keyname = "Space";
        break;
    case SDLK_EXCLAIM:
        charToPut = '!';
        break;
    case SDLK_QUOTEDBL:
        keyname = "QuoteDBL";
        break;
    case SDLK_HASH:
        charToPut = '#';
        break;
    case SDLK_DOLLAR:
        keyname = "$";
        break;
    case SDLK_ASTERISK:
        keyname = "Asterisk";
        break;
    case SDLK_PLUS:
        keyname = "Plus";
        break;
    case SDLK_COMMA:
        keyname = "Comma";
        break;
    case SDLK_MINUS:
        keyname = "Minus";
        break;
    case SDLK_PERIOD:
        keyname = "Period";
        break;
    case SDLK_SLASH:
        charToPut ='/';
        break;
    case SDLK_COLON:
        keyname = "Colon";
        break;
    case SDLK_SEMICOLON:
        keyname = "SemiColon";
        break;
    case SDLK_LESS:
        charToPut = '<';
        break;
    case SDLK_EQUALS:
        keyname = "Equals";
        break;
    case SDLK_DELETE:
        keyname = "Delete";
        break;
    case SDLK_KP_PERIOD:
        keyname = "NPperiod";
        break;
    case SDLK_KP_DIVIDE:
        keyname = "NPdivide";
        break;
    case SDLK_KP_MULTIPLY:
        keyname = "NPmultiply";
        break;
    case SDLK_KP_MINUS:
        keyname = "NPminus";
        break;
    case SDLK_KP_PLUS:
        keyname = "NPplus";
        break;
    case SDLK_KP_ENTER:
        keyname = "NP_Enter";
        break;
    case SDLK_KP_EQUALS:
        keyname = "NP=";
        break;
    case SDLK_UP:
        keyname = "UP";
        break;
    case SDLK_DOWN:
        keyname = "DOWN";
        break;
    case SDLK_RIGHT:
        keyname = "RIGHT";
        break;
    case SDLK_LEFT:
        keyname = "LEFT";
        break;
    case SDLK_INSERT:
        keyname = "Insert";
        break;
    case SDLK_HOME:
        keyname = "Home";
        break;
    case SDLK_END:
        keyname = "End";
        break;
    case SDLK_PAGEUP:
        keyname = "PageUp";
        break;
    case SDLK_PAGEDOWN:
        keyname = "PageDown";
        break;
    case SDLK_NUMLOCK:
        keyname = "NumLock";
        break;
    case SDLK_CAPSLOCK:
        keyname = "CapsLock";
        break;
    case SDLK_SCROLLOCK:
        keyname = "ScrolLock";
        break;
    case SDLK_RSHIFT:
        keyname = "Rshift";
        break;
    case SDLK_LSHIFT:
        keyname = "Lshift";
        break;
    case SDLK_RCTRL:
        keyname = "Rctrl";
        break;
    case SDLK_LCTRL:
        keyname = "Lctrl";
        break;
    case SDLK_RALT:
        keyname = "Ralt";
        break;
    case SDLK_LALT:
        keyname = "Lalt";
        break;
    case SDLK_RMETA:
        keyname = "Rmeta";
        break;
    case SDLK_LMETA:
        keyname = "Lmeta";
        break;
    case SDLK_LSUPER:
        keyname = "Lwin";
        break;
    case SDLK_RSUPER:
        keyname = "Rwin";
        break;
    case SDLK_MODE:
        keyname = "Mode";
        break;
    case SDLK_HELP:
        keyname = "Help";
        break;
    default:
        keyname = "Unknown";
        break;
    }
    if (charToPut != '\0')
    {
        keyname.clear();
        keyname.append(1,charToPut);
    }
    return keyname;
}

void MakeBackground(int xsize,int ysize,BlockGame &theGame, BlockGame &theGame2);

int OpenControlsBox(int x, int y, int player)
{
    int mousex, mousey;
    Uint8 *keys;
    bool done =false;
    string keyname;
    MakeBackground(xsize,ysize);
    while (!done)
    {
        SDL_Delay(10);
        DrawIMG(background, screen, 0, 0);
        DrawIMG(changeButtonsBack,screen,x,y);
        if (player == 0)
            SFont_Write(screen,fBlueFont,x+40,y+2,"Player 1 keys");
        else
            SFont_Write(screen,fBlueFont,x+40,y+2,"Player 2 keys");
        SFont_Write(screen,fBlueFont,x+6,y+50,"Up");
        keyname = getKeyName(keySettings[player].up);
        SFont_Write(screen,fBlueFont,x+200,y+50,keyname.c_str());
        SFont_Write(screen,fBlueFont,x+6,y+100,"Down");
        keyname = getKeyName(keySettings[player].down);
        SFont_Write(screen,fBlueFont,x+200,y+100,keyname.c_str());
        SFont_Write(screen,fBlueFont,x+6,y+150,"Left");
        keyname = getKeyName(keySettings[player].left);
        SFont_Write(screen,fBlueFont,x+200,y+150,keyname.c_str());
        SFont_Write(screen,fBlueFont,x+6,y+200,"Right");
        keyname = getKeyName(keySettings[player].right);
        SFont_Write(screen,fBlueFont,x+200,y+200,keyname.c_str());
        SFont_Write(screen,fBlueFont,x+6,y+250,"Push");
        keyname = getKeyName(keySettings[player].push);
        SFont_Write(screen,fBlueFont,x+200,y+250,keyname.c_str());
        SFont_Write(screen,fBlueFont,x+6,y+300,"Change");
        keyname = getKeyName(keySettings[player].change);
        SFont_Write(screen,fBlueFont,x+200,y+300,keyname.c_str());
        //Ask for mouse play
        SFont_Write(screen,fBlueFont,x+6,y+350,"Mouse play?");
        DrawIMG(iLevelCheckBox,screen,x+220,y+350);
        if (((player==0)&&(mouseplay1))||((player==2)&&(mouseplay2)))
            DrawIMG(iLevelCheck,screen,x+220,y+350); //iLevelCheck witdh is 42
        //Ask for joypad play
        SFont_Write(screen,fBlueFont,x+300,y+350,"Joypad?");
        DrawIMG(iLevelCheckBox,screen,x+460,y+350);
        if (((player==0)&&(joyplay1))||((player==2)&&(joyplay2)))
            DrawIMG(iLevelCheck,screen,x+460,y+350); //iLevelCheck witdh is 42
        for (int i=1; i<7; i++)
            DrawIMG(bChange,screen,x+420,y+50*i);
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if ( event.type == SDL_QUIT )  {
                done = true;
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
            }
        }	//PollEvent

        keys = SDL_GetKeyState(NULL);

        SDL_GetMouseState(&mousex,&mousey);

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            if ((mousex>(420+x)) && (mousex<(540+x)) && (mousey>(50+y)) && (mousey<(90+y)))
            {
                //up
                bool finnish = false;
                while (!finnish)
                    while ( SDL_PollEvent(&event) )
                    {
                        if (event.type == SDL_KEYDOWN)
                        {
                            if (event.key.keysym.sym != SDLK_ESCAPE)
                                keySettings[player].up = event.key.keysym.sym;
                            finnish = true;
                        }
                    }
            } //up
            if ((mousex>(420+x)) && (mousex<(540+x)) && (mousey>(100+y)) && (mousey<(140+y)))
            {
                //down
                bool finnish = false;
                while (!finnish)
                    while ( SDL_PollEvent(&event) )
                    {
                        if (event.type == SDL_KEYDOWN)
                        {
                            if (event.key.keysym.sym != SDLK_ESCAPE)
                                keySettings[player].down = event.key.keysym.sym;
                            finnish = true;
                        }
                    }
            } //down
            if ((mousex>(420+x)) && (mousex<(540+x)) && (mousey>(150+y)) && (mousey<(190+y)))
            {
                //left
                bool finnish = false;
                while (!finnish)
                    while ( SDL_PollEvent(&event) )
                    {
                        if (event.type == SDL_KEYDOWN)
                        {
                            if (event.key.keysym.sym != SDLK_ESCAPE)
                                keySettings[player].left = event.key.keysym.sym;
                            finnish = true;
                        }
                    }
            } //left
            if ((mousex>(420+x)) && (mousex<(540+x)) && (mousey>(200+y)) && (mousey<(240+y)))
            {
                //right
                bool finnish = false;
                while (!finnish)
                    while ( SDL_PollEvent(&event) )
                    {
                        if (event.type == SDL_KEYDOWN)
                        {
                            if (event.key.keysym.sym != SDLK_ESCAPE)
                                keySettings[player].right = event.key.keysym.sym;
                            finnish = true;
                        }
                    }
            } //right
            if ((mousex>(420+x)) && (mousex<(540+x)) && (mousey>(250+y)) && (mousey<(290+y)))
            {
                //push
                bool finnish = false;
                while (!finnish)
                    while ( SDL_PollEvent(&event) )
                    {
                        if (event.type == SDL_KEYDOWN)
                        {
                            if (event.key.keysym.sym != SDLK_ESCAPE)
                                keySettings[player].push = event.key.keysym.sym;
                            finnish = true;
                        }
                    }
            } //push
            if ((mousex>(420+x)) && (mousex<(540+x)) && (mousey>(300+y)) && (mousey<(340+y)))
            {
                //change
                bool finnish = false;
                while (!finnish)
                    while ( SDL_PollEvent(&event) )
                    {
                        if (event.type == SDL_KEYDOWN)
                        {
                            if (event.key.keysym.sym != SDLK_ESCAPE)
                                keySettings[player].change = event.key.keysym.sym;
                            finnish = true;
                        }
                    }
            } //change
            //mouseplay:
            if ((mousex>(220+x)) && (mousex<(262+x)) && (mousey>(350+y)) && (mousey<(392+y)))
            {
                if (player==0)
                {
                    mouseplay1 = !mouseplay1;
                }
                else
                {
                    mouseplay2 = !mouseplay2;
                }
            }
            //Joyplay:
            if ((mousex>(460+x)) && (mousex<(502+x)) && (mousey>(350+y)) && (mousey<(392+y)))
            {
                if (player==0)
                {
                    joyplay1 = !joyplay1;
                }
                else
                {
                    joyplay2 = !joyplay2;
                }
            }
        }	//get mouse state

        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen);
    }	//while !done
    DrawIMG(background, screen, 0, 0);
    return 0;
}


//Dialogbox
bool OpenDialogbox(int x, int y, char *name)
{
    bool done = false;     //We are done!
    bool accept = false;   //New name is accepted! (not Cancelled)
    bool repeating = false; //The key is being held (BACKSPACE)
    const int repeatDelay = 200;    //Repeating
    unsigned long time = 0;
    ReadKeyboard rk = ReadKeyboard(name);
    Uint8* keys;
    string strHolder;
    MakeBackground(xsize,ysize);
    DrawIMG(background,screen,0,0);
    while (!done)
    {
        DrawIMG(dialogBox,screen,x,y);
        SFont_Write(screen,fBlueFont,x+40,y+72,rk.GetString());
        strHolder = rk.GetString();
        strHolder.erase((int)rk.CharsBeforeCursor());

        if (((SDL_GetTicks()/600)%2)==1)
            SFont_Write(screen,fBlueFont,x+40+SFont_TextWidth(fBlueFont,strHolder.c_str()),y+69,"|");

        SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            if ( event.type == SDL_QUIT )  {
                done = true;
                accept = false;
            }

            if ( event.type == SDL_KEYDOWN )
            {
                if ( (event.key.keysym.sym == SDLK_RETURN)||(event.key.keysym.sym == SDLK_KP_ENTER) ) {
                    done = true;
                    accept = true;
                }
                else
                    if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
                        done = true;
                        accept = false;
                    }
                    else if (!(event.key.keysym.sym == SDLK_BACKSPACE)){
                        if ((rk.ReadKey(event.key.keysym.sym))&&(SoundEnabled)&&(!NoSound))Mix_PlayChannel(1,typingChunk,0);
                    }
                    else if ((event.key.keysym.sym == SDLK_BACKSPACE)&&(!repeating)){
                        if ((rk.ReadKey(event.key.keysym.sym))&&(SoundEnabled)&&(!NoSound))Mix_PlayChannel(1,typingChunk,0);
                        repeating = true;
                        time=SDL_GetTicks();
                    }
            }

        }	//while(event)

        if (SDL_GetTicks()>(time+repeatDelay))
        {
            time = SDL_GetTicks();
            keys = SDL_GetKeyState(NULL);
            if ( (keys[SDLK_BACKSPACE])&&(repeating) )
            {
                if ((rk.ReadKey(SDLK_BACKSPACE))&&(SoundEnabled)&&(!NoSound))Mix_PlayChannel(1,typingChunk,0);
            }
            else
                repeating = false;
        }

        SDL_Flip(screen); //Update screen
    }	//while(!done)
    strcpy(name,rk.GetString());
    bScreenLocked = false;
    showDialog = false;
    return accept;
}

//Draws the highscores
void DrawHighscores(int x, int y, bool endless)
{
    MakeBackground(xsize,ysize);
    DrawIMG(background,screen,0,0);
    if (endless) SFont_Write(screen,fBlueFont,x+100,y+100,"Endless:");
    else SFont_Write(screen,fBlueFont,x+100,y+100,"Time Trial:");
    for (int i =0;i<10;i++)
    {
        char playerScore[32];
        char playerName[32];
        if (endless)
        {
            sprintf(playerScore, "%i", theTopScoresEndless.getScoreNumber(i));
        }
        else
        {
            sprintf(playerScore, "%i", theTopScoresTimeTrial.getScoreNumber(i));
        }
        if (endless)
        {
            strcpy(playerName,theTopScoresEndless.getScoreName(i));
        }
        else
        {
            strcpy(playerName,theTopScoresTimeTrial.getScoreName(i));
        }
        SFont_Write(screen,fBlueFont,x+420,y+150+i*35,playerScore);
        SFont_Write(screen,fBlueFont,x+60,y+150+i*35,playerName);
    }
}

void DrawStats()
{
    MakeBackground(xsize,ysize);
    DrawIMG(background,screen,0,0);
    int y = 5;
    const int y_spacing = 30;
    SFont_Write(screen,fBlueFont,10,y,"Stats");
    y+=y_spacing*2;
    SFont_Write(screen,fBlueFont,10,y,"Chains");
    for(int i=2;i<13;i++)
    {
        y+=y_spacing;
        SFont_Write(screen,fBlueFont,10,y,(itoa(i)+"X").c_str());
        string numberAsString = itoa(Stats::getInstance()->getNumberOf("chainX"+itoa(i)));
        SFont_Write(screen,fBlueFont,300,y,numberAsString.c_str());
    }
    y+=y_spacing*2;
    SFont_Write(screen,fBlueFont,10,y,"Lines Pushed: ");
    string numberAsString = itoa(Stats::getInstance()->getNumberOf("linesPushed"));
    SFont_Write(screen,fBlueFont,300,y,numberAsString.c_str());

    y+=y_spacing;
    SFont_Write(screen,fBlueFont,10,y,"Puzzles solved: ");
    numberAsString = itoa(Stats::getInstance()->getNumberOf("puzzlesSolved"));
    SFont_Write(screen,fBlueFont,300,y,numberAsString.c_str());

    y+=y_spacing*2;
    SFont_Write(screen,fBlueFont,10,y,"Run time: ");
    commonTime ct = TimeHandler::peekTime("totalTime",TimeHandler::ms2ct(SDL_GetTicks()));
    y+=y_spacing;
    SFont_Write(screen,fBlueFont,10,y,((string)("Days: "+itoa(ct.days))).c_str());
    y+=y_spacing;
    SFont_Write(screen,fBlueFont,10,y,((string)("Hours: "+itoa(ct.hours))).c_str());
    y+=y_spacing;
    SFont_Write(screen,fBlueFont,10,y,((string)("Minutes: "+itoa(ct.minutes))).c_str());
    y+=y_spacing;
    SFont_Write(screen,fBlueFont,10,y,((string)("Seconds: "+itoa(ct.seconds))).c_str());

    y-=y_spacing*4; //Four rows back
    const int x_offset3 = xsize/3+10; //Ofset for three rows
    SFont_Write(screen,fBlueFont,x_offset3,y,"Play time: ");
    ct = TimeHandler::getTime("playTime");
    y+=y_spacing;
    SFont_Write(screen,fBlueFont,x_offset3,y,((string)("Days: "+itoa(ct.days))).c_str());
    y+=y_spacing;
    SFont_Write(screen,fBlueFont,x_offset3,y,((string)("Hours: "+itoa(ct.hours))).c_str());
    y+=y_spacing;
    SFont_Write(screen,fBlueFont,x_offset3,y,((string)("Minutes: "+itoa(ct.minutes))).c_str());
    y+=y_spacing;
    SFont_Write(screen,fBlueFont,x_offset3,y,((string)("Seconds: "+itoa(ct.seconds))).c_str());

    const int x_offset = xsize/2+10;
    y = 5+y_spacing*2;
    SFont_Write(screen,fBlueFont,x_offset,y,"VS CPU (win/loss)");
    for(int i=0;i<7;i++)
    {
        y += y_spacing;
        SFont_Write(screen,fBlueFont,x_offset,y,("AI "+itoa(i+1)).c_str());
        numberAsString = itoa(Stats::getInstance()->getNumberOf("defeatedAI"+itoa(i)));
        string numberAsString2 = itoa(Stats::getInstance()->getNumberOf("defeatedByAI"+itoa(i)));
        string toPrint = numberAsString + "/" + numberAsString2;
        SFont_Write(screen,fBlueFont,x_offset+230,y,toPrint.c_str());
    }
}

void OpenScoresDisplay()
{
    int mousex,mousey;
    bool done = false;     //We are done!
    int page = 0;
    const int numberOfPages = 3;
    //button coodinates:
    const int scoreX = buttonXsize*2;
    const int scoreY = 0;
    const int backX = 20;
    const int backY = ysize-buttonYsize-20;
    const int nextX = xsize-buttonXsize-20;
    const int nextY = backY;
    while (!done)
    {
        switch(page)
        {
            case 0:
                //Highscores, endless
                DrawHighscores(100,100,true);
                break;
            case 1:
                //Highscores, Time Trial
                DrawHighscores(100,100,false);
                break;
            case 2:
            default:
                DrawStats();
        };

        //Draw buttons:
        DrawIMG(bHighScore,screen,scoreX,scoreY);
        DrawIMG(bBack,screen,backX,backY);
        DrawIMG(bNext,screen,nextX,nextY);

        //Draw page number
        string pageXofY = ((string)"Page ")+itoa(page+1)+((string)" of ")+itoa(numberOfPages);
        SFont_Write(screen,fBlueFont,xsize/2-SFont_TextWidth(fBlueFont,pageXofY.c_str())/2,ysize-60,pageXofY.c_str());
        
        SDL_Delay(10);
        SDL_Event event;

        SDL_GetMouseState(&mousex,&mousey);
        
        while ( SDL_PollEvent(&event) )
        {


            if ( event.type == SDL_QUIT )  {
                done = true;
            }

            if ( event.type == SDL_KEYDOWN )
            {
                if( (event.key.keysym.sym == SDLK_RIGHT))
                {
                    page++;
                    if(page>=numberOfPages)
                        page = 0;
                }
                else
                if( (event.key.keysym.sym == SDLK_LEFT))
                {
                    page--;
                    if(page<0)
                        page = numberOfPages-1;
                }
                else
                    done = true;
                
                if ( event.key.keysym.sym == SDLK_F9 ) {
                    writeScreenShot();
                }

                if ( (event.key.keysym.sym == SDLK_RETURN)||(event.key.keysym.sym == SDLK_KP_ENTER) ) {
                    done = true;
                }
                else
                    if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
                        done = true;
                    }
            }

        }	//while(event)

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            //The Score button:
            if((mousex>scoreX) && (mousex<scoreX+buttonXsize) && (mousey>scoreY) && (mousey<scoreY+buttonYsize))
                done =true;

            //The back button:
            if((mousex>backX) && (mousex<backX+buttonXsize) && (mousey>backY) && (mousey<backY+buttonYsize))
            {
                page--;
                if(page<0)
                    page = numberOfPages-1;
            }

            //The next button:
            if((mousex>nextX) && (mousex<nextX+buttonXsize) && (mousey>nextY) && (mousey<nextY+buttonYsize))
            {
                page++;
                if(page>=numberOfPages)
                    page = 0;
            }
        }

        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen); //Update screen
    }
}


//Open a puzzle file
bool OpenFileDialogbox(int x, int y, char *name)
{
    bool done = false;	//We are done!
    int mousex, mousey;
    ListFiles lf = ListFiles();
    string folder = (string)SHAREDIR+(string)"/puzzles";
    cout << "Looking in " << folder << endl;
    lf.setDirectory(folder.c_str());
#ifdef __unix__
    string homeFolder = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/puzzles";
    lf.setDirectory2(homeFolder.c_str());
#endif
    Uint8* keys;
    string strHolder;
    MakeBackground(xsize,ysize);
    DrawIMG(background,screen,0,0);
    DrawIMG(bForward,background,x+460,y+420);
    DrawIMG(bBack,background,x+20,y+420);
    while (!done)
    {
        DrawIMG(background,screen,0,0);
        const int nrOfFiles = 10;
        DrawIMG(changeButtonsBack,screen,x,y);
        for (int i=0;i<nrOfFiles;i++)
        {
            SFont_Write(screen,fBlueFont,x+10,y+10+36*i,lf.getFileName(i).c_str());
        }

        SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            if ( event.type == SDL_QUIT )  {
                done = true;
            }

            if ( event.type == SDL_KEYDOWN )
            {
                if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
                    done = true;
                }

                if ( (event.key.keysym.sym == SDLK_RIGHT) ) {
                    lf.forward();
                }

                if ( (event.key.keysym.sym == SDLK_LEFT) ) {
                    lf.back();
                }
            }

        } //while(event)

        SDL_GetMouseState(&mousex,&mousey);

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            //The Forward Button:
            if ( (mousex>x+460) && (mousex<x+460+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
            {
                lf.forward();
            }

            //The back button:
            if ( (mousex>x+20) && (mousex<x+20+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
            {
                lf.back();
            }

            for (int i=0;i<10;i++)
            {
                if ( (mousex>x+10) && (mousex<x+480) && (mousey>y+10+i*36) && (mousey<y+10+i*36+32) )
                {
                    if (lf.fileExists(i))
                    {
                        strncpy(name,lf.getFileName(i).c_str(),28); //Problems occurs then larger than 28 (maybe 29)
                        done=true; //The user have, clicked the purpose of this function is now complete
                    }
                }
            }
        }

        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen); //Update screen
    }
}

//Slelect a theme
bool SelectThemeDialogbox(int x, int y, char *name)
{
    bool done = false;	//We are done!
    int mousex, mousey;
    ListFiles lf = ListFiles();
    string folder = (string)SHAREDIR+(string)"/themes";
    cout << "Looking in " << folder << endl;
    lf.setDirectory(folder.c_str());
#ifdef __unix__
    string homeFolder = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/themes";
    lf.setDirectory2(homeFolder.c_str());
#endif
    Uint8* keys;
    string strHolder;
    MakeBackground(xsize,ysize);
    DrawIMG(background,screen,0,0);
    DrawIMG(bForward,background,x+460,y+420);
    DrawIMG(bBack,background,x+20,y+420);
    while (!done)
    {
        DrawIMG(background,screen,0,0);
        const int nrOfFiles = 10;
        DrawIMG(changeButtonsBack,screen,x,y);
        for (int i=0;i<nrOfFiles;i++)
        {
            SFont_Write(screen,fBlueFont,x+10,y+10+36*i,lf.getFileName(i).c_str());
        }

        SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            if ( event.type == SDL_QUIT )  {
                done = true;
            }

            if ( event.type == SDL_KEYDOWN )
            {
                if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
                    done = true;
                }

                if ( (event.key.keysym.sym == SDLK_RIGHT) ) {
                    lf.forward();
                }

                if ( (event.key.keysym.sym == SDLK_LEFT) ) {
                    lf.back();
                }
            }

        } //while(event)

        SDL_GetMouseState(&mousex,&mousey);

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            //The Forward Button:
            if ( (mousex>x+460) && (mousex<x+460+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
            {
                lf.forward();
            }

            //The back button:
            if ( (mousex>x+20) && (mousex<x+20+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
            {
                lf.back();
            }

            for (int i=0;i<10;i++)
            {
                if ( (mousex>x+10) && (mousex<x+480) && (mousey>y+10+i*36) && (mousey<y+10+i*36+32) )
                {
                    if (lf.fileExists(i))
                    {
                        strncpy(name,lf.getFileName(i).c_str(),28); //Problems occurs then larger than 28 (maybe 29)
                        loadTheme(lf.getFileName(i));
                        done=true; //The user have, clicked the purpose of this function is now complete
                    }
                }
            }
        }

        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen); //Update screen
    }
}

//Open a saved replay
bool OpenReplayDialogbox(int x, int y, char *name)
{
    bool done = false;	//We are done!
    int mousex, mousey;
    ListFiles lf = ListFiles();
    cout << "Ready to set directory!" << endl;
#ifdef __unix__
    string directory = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/replays";
#elif WIN32
    string directory = getMyDocumentsPath()+(string)"/My Games/blockattack/replays";
#else
    string directory = "./replays";
#endif
    lf.setDirectory(directory);
    cout << "Directory sat" << endl;
    Uint8* keys;
    string strHolder;
    MakeBackground(xsize,ysize);
    DrawIMG(background,screen,0,0);
    DrawIMG(bForward,background,x+460,y+420);
    DrawIMG(bBack,background,x+20,y+420);
    while (!done)
    {
        DrawIMG(background,screen,0,0);
        const int nrOfFiles = 10;
        DrawIMG(changeButtonsBack,screen,x,y);
        for (int i=0;i<nrOfFiles;i++)
        {
            SFont_Write(screen,fBlueFont,x+10,y+10+36*i,lf.getFileName(i).c_str());
        }

        SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            if ( event.type == SDL_QUIT )  {
                done = true;
                return false;
            }

            if ( event.type == SDL_KEYDOWN )
            {
                if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
                    done = true;
                    return false;
                }

                if ( (event.key.keysym.sym == SDLK_RIGHT) ) {
                    lf.forward();
                }

                if ( (event.key.keysym.sym == SDLK_LEFT) ) {
                    lf.back();
                }
            }

        } //while(event)

        SDL_GetMouseState(&mousex,&mousey);

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            //The Forward Button:
            if ( (mousex>x+460) && (mousex<x+460+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
            {
                lf.forward();
            }

            //The back button:
            if ( (mousex>x+20) && (mousex<x+20+buttonXsize) && (mousey>y+420) && (mousey<y+420+40) )
            {
                lf.back();
            }

            for (int i=0;i<10;i++)
            {
                if ( (mousex>x+10) && (mousex<x+480) && (mousey>y+10+i*36) && (mousey<y+10+i*36+32) )
                {
                    if (lf.fileExists(i))
                    {
                        strncpy(name,lf.getFileName(i).c_str(),28); //Problems occurs then larger than 28 (maybe 29)
                        done=true; //The user have, clicked the purpose of this function is now complete
                        return true;
                    }
                }
            }
        }

        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen); //Update screen
    }
}


//draws options:
inline void DrawOptions(int x, int y)
{
    if (MusicEnabled) DrawIMG(bOn,optionsBack,400,buttonXsize);
    else DrawIMG(bOff,optionsBack,400,buttonXsize);
    if (SoundEnabled) DrawIMG(bOn,optionsBack,400,170);
    else DrawIMG(bOff,optionsBack,400,170);
    if (bFullscreen) DrawIMG(bOn,optionsBack,400,220);
    else DrawIMG(bOff,optionsBack,400,220);
    DrawIMG(bChange,optionsBack,230,435);
    DrawIMG(bChange,optionsBack,410,435);
    DrawIMG(bChange,optionsBack,230,500);
    DrawIMG(bChange,optionsBack,410,500);
    DrawIMG(optionsBack,screen,x,y);
}  //drawOptions

//Draws the balls and explosions
void DrawBalls()
{
    for (int i = 0; i< maxNumberOfBalls; i++)
    {
        if (theBallManeger.ballUsed[i])
        {
            DrawIMG(balls[theBallManeger.ballArray[i].getColor()],screen,theBallManeger.ballArray[i].getX(),theBallManeger.ballArray[i].getY());
        } //if used
        if (theExplosionManeger.explosionUsed[i])
        {
            DrawIMG(explosion[theExplosionManeger.explosionArray[i].getFrame()],screen,theExplosionManeger.explosionArray[i].getX(),theExplosionManeger.explosionArray[i].getY());
        }
        if (theTextManeger.textUsed[i])
        {
            //cout << "Printing text: " << theTextManeger.textArray[i].getText() << endl;
            int x = theTextManeger.textArray[i].getX()-SFont_TextWidth(fSmallFont,theTextManeger.textArray[i].getText())/2;
            int y = theTextManeger.textArray[i].getY()-SFont_TextHeight(fSmallFont)/2;
            DrawIMG(iChainBack,screen,x,y);
            SFont_Write(screen,fSmallFont,x+(25-SFont_TextWidth(fSmallFont,theTextManeger.textArray[i].getText()))/2,y+(25-SFont_TextHeight(fSmallFont))/2,theTextManeger.textArray[i].getText());
        }
    } //for
}    //DrawBalls

//Removes the old balls
void UndrawBalls()
{
    for (int i = 0; i< maxNumberOfBalls; i++)
    {
        if (theBallManeger.oldBallUsed[i])
        {
            DrawIMG(background,screen,theBallManeger.oldBallArray[i].getX(),theBallManeger.oldBallArray[i].getY(),ballSize,ballSize,theBallManeger.oldBallArray[i].getX(),theBallManeger.oldBallArray[i].getY());
        } //if used
        if (theExplosionManeger.oldExplosionUsed[i])
        {
            DrawIMG(background,screen,theExplosionManeger.oldExplosionArray[i].getX(),theExplosionManeger.oldExplosionArray[i].getY(),70,120,theExplosionManeger.oldExplosionArray[i].getX(),theExplosionManeger.oldExplosionArray[i].getY());
        }
        if (theTextManeger.oldTextUsed[i])
        {
            int x = theTextManeger.oldTextArray[i].getX()-SFont_TextWidth(fSmallFont,theTextManeger.oldTextArray[i].getText())/2;
            int y = theTextManeger.oldTextArray[i].getY()-SFont_TextHeight(fSmallFont)/2;
            DrawIMG(background,screen,x,y,25,25,x,y);
        }
    } //for
}   //UndrawBalls

//draws everything
void DrawEverything(int xsize, int ysize,BlockGame &theGame, BlockGame &theGame2)
{
    SDL_ShowCursor(SDL_DISABLE);
    //draw background:
    if (forceredraw != 1)
    {

        UndrawBalls();
        DrawIMG(background,screen,oldMousex,oldMousey,32,32,oldMousex,oldMousey);
        DrawIMG(background,screen,oldBubleX,oldBubleY,140,50,oldBubleX,oldBubleY);


        DrawIMG(background,screen,350,200,120,200,350,200);
        DrawIMG(background,screen,830,200,120,200,830,200);
        DrawIMG(background,screen,800,0,140,50,800,0);

        DrawIMG(background,screen,50,60,300,50,50,60);
        DrawIMG(background,screen,510,60,300,50,510,60);
    }
    else
        DrawIMG(background,screen,0,0);
    //draw bottons (should be moves and drawn directly to background once)
    if (!editorMode)
        #if NETWORK
        if (!networkActive) //We don't show the menu while running server or connected to a server
        #else
            if(true)
        #endif
        {
            //Here we draw the menu
            DrawIMG(bNewGame, screen, 0, 0);
            DrawIMG(bOptions, screen, buttonXsize,0);
            DrawIMG(bHighScore, screen, 2*buttonXsize,0);
            DrawIMG(bReplay,screen,3*buttonXsize,0);
        }
        else
        { //If network is active
            DrawIMG(bBack, screen, 0, 0); //Display a disconnect button
        }
    if (!editorMode)
        DrawIMG(bExit, screen, xsize-120,ysize-120);
    //DrawIMG(boardBackBack,screen,theGame.topx-60,theGame.topy-68);
    DrawIMG(theGame.sBoard,screen,theGame.topx,theGame.topy);
    string strHolder;
    strHolder = itoa(theGame.score+theGame.handicap);
    SFont_Write(screen,fBlueFont,theGame.topx+310,theGame.topy+100,strHolder.c_str());
    if (theGame.AI_Enabled)
        SFont_Write(screen,fBlueFont,theGame.topx+10,theGame.topy-40,"CPU");
    else
        if (editorMode)
            SFont_Write(screen,fBlueFont,theGame.topx+10,theGame.topy-40,"Playing field");
        else
            if (!singlePuzzle)
                SFont_Write(screen,fBlueFont,theGame.topx+10,theGame.topy-40,player1name);
    if (theGame.timetrial)
    {
        int tid = (int)SDL_GetTicks()-theGame.gameStartedAt;
        int minutes;
        int seconds;
        if (tid>=0)
        {
            minutes = (2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame.gameStartedAt)))/60/1000;
            seconds = ((2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame.gameStartedAt)))%(60*1000))/1000;
        }
        else
        {
            minutes = ((abs((int)SDL_GetTicks()-(int)theGame.gameStartedAt)))/60/1000;
            seconds = (((abs((int)SDL_GetTicks()-(int)theGame.gameStartedAt)))%(60*1000))/1000;
        }
        if (theGame.bGameOver) minutes=0;
        if (theGame.bGameOver) seconds=0;
        if (seconds>9)
            strHolder = itoa(minutes)+":"+itoa(seconds);
        else strHolder = itoa(minutes)+":0"+itoa(seconds);
        //if ((SoundEnabled)&&(!NoSound)&&(tid>0)&&(seconds<5)&&(minutes == 0)&&(seconds>1)&&(!(Mix_Playing(6)))) Mix_PlayChannel(6,heartBeat,0);
        SFont_Write(screen,fBlueFont,theGame.topx+310,theGame.topy+150,strHolder.c_str());
    }
    else
    {
        int minutes = ((abs((int)SDL_GetTicks()-(int)theGame.gameStartedAt)))/60/1000;
        int seconds = (((abs((int)SDL_GetTicks()-(int)theGame.gameStartedAt)))%(60*1000))/1000;
        if (theGame.bGameOver) minutes=(theGame.gameEndedAfter/1000/60)%100;
        if (theGame.bGameOver) seconds=(theGame.gameEndedAfter/1000)%60;
        if (seconds>9)
            strHolder = itoa(minutes)+":"+itoa(seconds);
        else
            strHolder = itoa(minutes)+":0"+itoa(seconds);
        SFont_Write(screen,fBlueFont,theGame.topx+310,theGame.topy+150,strHolder.c_str());
    }
    strHolder = itoa(theGame.chain);
    SFont_Write(screen,fBlueFont,theGame.topx+310,theGame.topy+200,strHolder.c_str());
    //drawspeedLevel:
    strHolder = itoa(theGame.speedLevel);
    SFont_Write(screen,fBlueFont,theGame.topx+310,theGame.topy+250,strHolder.c_str());
    if ((theGame.stageClear) &&(theGame.topy+700+50*(theGame.stageClearLimit-theGame.linesCleared)-theGame.pixels-1<600+theGame.topy))
    {
        oldBubleX = theGame.topx+280;
        oldBubleY = theGame.topy+650+50*(theGame.stageClearLimit-theGame.linesCleared)-theGame.pixels-1;
        DrawIMG(stageBobble,screen,theGame.topx+280,theGame.topy+650+50*(theGame.stageClearLimit-theGame.linesCleared)-theGame.pixels-1);
    }
    //player1 finnish, player2 start
    //DrawIMG(boardBackBack,screen,theGame2.topx-60,theGame2.topy-68);
    if (!editorMode)
    {
        DrawIMG(theGame2.sBoard,screen,theGame2.topx,theGame2.topy);
        strHolder = itoa(theGame2.score+theGame2.handicap);
        SFont_Write(screen,fBlueFont,theGame2.topx+310,theGame2.topy+100,strHolder.c_str());
        if (theGame2.AI_Enabled)
            SFont_Write(screen,fBlueFont,theGame2.topx+10,theGame2.topy-40,"CPU");
        else
            SFont_Write(screen,fBlueFont,theGame2.topx+10,theGame2.topy-40,theGame2.name);
        if (theGame2.timetrial)
        {
            int tid = (int)SDL_GetTicks()-theGame2.gameStartedAt;
            int minutes;
            int seconds;
            if (tid>=0)
            {
                minutes = (2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame2.gameStartedAt)))/60/1000;
                seconds = ((2*60*1000-(abs((int)SDL_GetTicks()-(int)theGame2.gameStartedAt)))%(60*1000))/1000;
            }
            else
            {
                minutes = ((abs((int)SDL_GetTicks()-(int)theGame2.gameStartedAt)))/60/1000;
                seconds = (((abs((int)SDL_GetTicks()-(int)theGame2.gameStartedAt)))%(60*1000))/1000;
            }
            if (theGame2.bGameOver) minutes=0;
            if (theGame2.bGameOver) seconds=0;
            if (seconds>9)
                strHolder = itoa(minutes)+":"+itoa(seconds);
            else
                strHolder = itoa(minutes)+":0"+itoa(seconds);
            //if ((SoundEnabled)&&(!NoSound)&&(tid>0)&&(seconds<5)&&(minutes == 0)&&(seconds>1)&&(!(Mix_Playing(6)))) Mix_PlayChannel(6,heartBeat,0);
            SFont_Write(screen,fBlueFont,theGame2.topx+310,theGame2.topy+150,strHolder.c_str());
        }
        else
        {
            int minutes = (abs((int)SDL_GetTicks()-(int)theGame2.gameStartedAt))/60/1000;
            int seconds = (abs((int)SDL_GetTicks()-(int)theGame2.gameStartedAt)%(60*1000))/1000;
            if (theGame2.bGameOver) minutes=(theGame2.gameEndedAfter/1000/60)%100;
            if (theGame2.bGameOver) seconds=(theGame2.gameEndedAfter/1000)%60;
            if (seconds>9)
                strHolder = itoa(minutes)+":"+itoa(seconds);
            else
                strHolder = itoa(minutes)+":0"+itoa(seconds);
            SFont_Write(screen,fBlueFont,theGame2.topx+310,theGame2.topy+150,strHolder.c_str());
        }
        strHolder = itoa(theGame2.chain);
        SFont_Write(screen,fBlueFont,theGame2.topx+310,theGame2.topy+200,strHolder.c_str());
        strHolder = itoa(theGame2.speedLevel);
        SFont_Write(screen,fBlueFont,theGame2.topx+310,theGame2.topy+250,strHolder.c_str());
    }
    //player2 finnish

    if (bNewGameOpen)
    {
        DrawIMG(b1player,screen,0,40);
        DrawIMG(b2players,screen,0,80);
#if NETWORK
        DrawIMG(bNetwork,screen,0,buttonXsize);
#endif
        if (b1playerOpen)
        {
            DrawIMG(bEndless,screen,buttonXsize,40);
            DrawIMG(bTimeTrial,screen,buttonXsize,80);
            DrawIMG(bStageClear,screen,buttonXsize,buttonXsize);
            DrawIMG(bPuzzle,screen,buttonXsize,160);
            DrawIMG(bVsMode,screen,buttonXsize,200);
        }
        else
            if (b2playersOpen)
            {
                DrawIMG(bTimeTrial,screen,buttonXsize,80);
                DrawIMG(bVsMode,screen,buttonXsize,120);
            }
#if NETWORK
            else
                if (bNetworkOpen)
                {
                    DrawIMG(bHost,screen,buttonXsize,120);
                    DrawIMG(bConnect,screen,buttonXsize,160);
                }
#endif
    }
    if (bOptionsOpen)
    {
        DrawIMG(bConfigure,screen,buttonXsize,40);
        DrawIMG(bSelectPuzzle,screen,buttonXsize,80);
        DrawIMG(bVsModeConfig,screen,buttonXsize,120);
        DrawIMG(bTheme,screen,buttonXsize,160);
    }
    if (bReplayOpen)
    {
        DrawIMG(bSave,screen,360,40);
        DrawIMG(bLoad,screen,360,80);
    }
    if (showOptions) DrawOptions(100,100);

    DrawBalls();

#if DEBUG
    Frames++;
    if (SDL_GetTicks() >= Ticks + 1000)
    {
        if (Frames > 999) Frames=999;
        sprintf(FPS, "%lu fps", Frames);
        Frames = 0;
        Ticks = SDL_GetTicks();
    }

    SFont_Write(screen,fBlueFont,800,4,FPS);
#endif

    //SDL_Flip(screen); Update screen is now called outside DrawEvrything, bacause the mouse needs to be painted

}

//Generates the standard background
void MakeBackground(int xsize,int ysize)
{
    int w = backgroundImage->w;
    int h = backgroundImage->h;
    for(int i=0;i*w<xsize;i++)
        for(int j=0;j*h<ysize;j++)
            DrawIMG(backgroundImage,background,i*w,j*h);
    standardBackground = true;
}

//Generates the background with red board backs
void MakeBackground(int xsize,int ysize,BlockGame &theGame, BlockGame &theGame2)
{
    MakeBackground(xsize,ysize);
    DrawIMG(boardBackBack,background,theGame.topx-60,theGame.topy-68);
    DrawIMG(boardBackBack,background,theGame2.topx-60,theGame2.topy-68);
    standardBackground = false;
}

void MakeBackground(int xsize, int ysize, BlockGame &theGame)
{
    MakeBackground(xsize,ysize);
    DrawIMG(boardBackBack,background,theGame.topx-60,theGame.topy-68);
    standardBackground = false;
}


//The function that allows the player to choose PuzzleLevel
int PuzzleLevelSelect()
{
    const int xplace = 200;
    const int yplace = 300;
    Uint8 *keys;
    int levelNr, mousex, mousey;
    bool levelSelected = false;
    bool tempBool;

    //Loads the levels, if they havn't been loaded:
    LoadPuzzleStages();

    //Keeps track of background;
    int nowTime=SDL_GetTicks();

    ifstream puzzleFile(puzzleSavePath.c_str(),ios::binary);
    MakeBackground(xsize,ysize);
    if (puzzleFile)
    {
        for (int i=0;(i<nrOfPuzzles)&&(!puzzleFile.eof()); i++)
        {
            puzzleFile.read(reinterpret_cast<char*>(&tempBool),sizeof(bool));
            puzzleCleared[i] = tempBool;
        }
        puzzleFile.close();
    }
    else
    {
        tempBool = false;
        for (int i=0; i<nrOfPuzzles; i++)
            puzzleCleared[i] = tempBool;
    }

    do
    {
        nowTime=SDL_GetTicks();


        DrawIMG(background, screen, 0, 0);
        DrawIMG(iCheckBoxArea,screen,xplace,yplace);
        SFont_Write(screen,fBlueFont,xplace+12,yplace+2,"Select Puzzle");
        //Now drow the fields you click in (and a V if clicked):
        for (int i = 0; i < nrOfPuzzles;i++)
        {
            DrawIMG(iLevelCheckBox,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
            if (puzzleCleared[i]==true) DrawIMG(iLevelCheck,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
        }

        SDL_Event event;
        while ( SDL_PollEvent(&event) )
            if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                    levelNr = -1;
                    levelSelected = true;
                }
            }

        keys = SDL_GetKeyState(NULL);

        SDL_GetMouseState(&mousex,&mousey);

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            int levelClicked = -1;
            int i;
            for (i = 0; (i<nrOfPuzzles/10)||((i<nrOfPuzzles/10+1)&&(nrOfPuzzles%10 != 0)); i++)
                if ((60+i*50<mousey-yplace)&&(mousey-yplace<i*50+92))
                    levelClicked = i*10;
            i++;
            if (levelClicked != -1)
                for (int j = 0; ((j<nrOfStageLevels%(i*10))&&(j<10)); j++)
                    if ((10+j*50<mousex-xplace)&&(mousex-xplace<j*50+42))
                    {
                        levelClicked +=j;
                        levelSelected = true;
                        levelNr = levelClicked;
                    }
        }

        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen); //draws it all to the screen

    } while (!levelSelected);
    DrawIMG(background, screen, 0, 0);
    return levelNr;
}

//The function that allows the player to choose Level number
int StageLevelSelect()
{
    const int xplace = 200;
    const int yplace = 300;
    Uint8 *keys;
    int levelNr, mousex, mousey;
    bool levelSelected = false;
    bool tempBool;
    Uint32 tempUInt32;
    Uint32 totalScore = 0;
    Uint32 totalTime = 0;

    //Keeps track of background;
    //int nowTime=SDL_GetTicks();

    MakeBackground(xsize,ysize);
    ifstream stageFile(stageClearSavePath.c_str(),ios::binary);
    if (stageFile)
    {
        for (int i = 0; i<nrOfStageLevels; i++)
        {
            stageFile.read(reinterpret_cast<char*>(&tempBool),sizeof(bool));
            stageCleared[i]=tempBool;
        }
        if(!stageFile.eof())
        {
            for(int i=0; i<nrOfStageLevels; i++)
            {
                tempUInt32 = 0;
                if(!stageFile.eof())
                    stageFile.read(reinterpret_cast<char*>(&tempUInt32),sizeof(Uint32));
                stageScores[i]=tempUInt32;
                totalScore+=tempUInt32;
            }
            for(int i=0; i<nrOfStageLevels; i++)
            {
                tempUInt32 = 0;
                if(!stageFile.eof())
                    stageFile.read(reinterpret_cast<char*>(&tempUInt32),sizeof(Uint32));
                stageTimes[i]=tempUInt32;
                totalTime += tempUInt32;
            }
        }
        else
        {
            for(int i=0; i<nrOfStageLevels; i++)
            {
                 stageScores[i]=0;
                 stageTimes[i]=0;
            }
        }
        stageFile.close();
    }
    else
    {
        for (int i=0; i<nrOfStageLevels; i++)
        {
            stageCleared[i]= false;
            stageScores[i]=0;
            stageTimes[i]=0;
        }
    }


    do
    {
        //nowTime=SDL_GetTicks();
        DrawIMG(background, screen, 0, 0);
        DrawIMG(iCheckBoxArea,screen,xplace,yplace);
        SFont_Write(screen,fBlueFont,xplace+12,yplace+2,"Stage Clear Level Select");
        for (int i = 0; i < nrOfStageLevels;i++)
        {
            DrawIMG(iLevelCheckBox,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
            if (stageCleared[i]==true) DrawIMG(iLevelCheck,screen,xplace+10+(i%10)*50, yplace+60+(i/10)*50);
        }

        SDL_Event event;
        while ( SDL_PollEvent(&event) )
            if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                    levelNr = -1;
                    levelSelected = true;
                }
            }

        keys = SDL_GetKeyState(NULL);

        SDL_GetMouseState(&mousex,&mousey);

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            int levelClicked = -1;
            int i;
            for (i = 0; (i<nrOfStageLevels/10)||((i<nrOfStageLevels/10+1)&&(nrOfStageLevels%10 != 0)); i++)
                if ((60+i*50<mousey-yplace)&&(mousey-yplace<i*50+92))
                    levelClicked = i*10;
            i++;
            if (levelClicked != -1)
                for (int j = 0; ((j<nrOfStageLevels%(i*10))&&(j<10)); j++)
                    if ((10+j*50<mousex-xplace)&&(mousex-xplace<j*50+42))
                    {
                        levelClicked +=j;
                        levelSelected = true;
                        levelNr = levelClicked;
                    }
        }
        //Find what we are over:
            int overLevel = -1;
            int i;
            for (i = 0; (i<nrOfStageLevels/10)||((i<nrOfStageLevels/10+1)&&(nrOfStageLevels%10 != 0)); i++)
                if ((60+i*50<mousey-yplace)&&(mousey-yplace<i*50+92))
                    overLevel = i*10;
            i++;
            if (overLevel != -1)
                for (int j = 0; ((j<nrOfStageLevels%(i*10))&&(j<10)); j++)
                    if ((10+j*50<mousex-xplace)&&(mousex-xplace<j*50+42))
                    {
                        overLevel +=j;
                        string scoreString = "Best score: 0";
                        string timeString = "Time used: -- : --";
                        
                        if(stageScores.at(overLevel)>0)
                            scoreString = "Best score: "+itoa(stageScores[overLevel]);
                        if(stageTimes[overLevel]>0)
                            timeString = "Time used: "+itoa(stageTimes[overLevel]/1000/60)+" : "+itoa2((stageTimes[overLevel]/1000)%60);
                        
                        SFont_Write(screen,fBlueFont,200,200,scoreString.c_str());
                        SFont_Write(screen,fBlueFont,200,250,timeString.c_str());
                        
                        overLevel;
                    }
            string totalString = "Total score: " +itoa(totalScore) + " in " + itoa(totalTime/1000/60) + " : " + itoa2((totalTime/1000)%60);
            SFont_Write(screen,fBlueFont,200,600,totalString.c_str());   

        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen); //draws it all to the screen

    } while (!levelSelected);
    DrawIMG(background, screen, 0, 0);
    return levelNr;
}

//Ask user for what AI level he will compete agains, return the number. Number must be 0..AIlevels
int startSingleVs()
{
    //Where to place the windows
    const int xplace = 200;
    const int yplace = 100;
    Uint8 *keys;	//To take keyboard input
    int mousex, mousey;	//To allow mouse
    bool done = false;	//When are we done?

    MakeBackground(xsize,ysize);
    DrawIMG(changeButtonsBack,background,xplace,yplace);
    SFont_Write(background,fBlueFont,xplace+10,yplace+10,"1 : Very Easy");
    SFont_Write(background,fBlueFont,xplace+10,yplace+40,"2 : Easy");
    SFont_Write(background,fBlueFont,xplace+10,yplace+70,"3 : Below Normal");
    SFont_Write(background,fBlueFont,xplace+10,yplace+100,"4 : Normal");
    SFont_Write(background,fBlueFont,xplace+10,yplace+130,"5 : Above Normal");
    SFont_Write(background,fBlueFont,xplace+10,yplace+160,"6 : Hard");
    SFont_Write(background,fBlueFont,xplace+10,yplace+190,"7 : Hardest");
    DrawIMG(background, screen, 0, 0);
    SDL_Flip(screen);
    do
    {

        SDL_Delay(10);
        SDL_Event event;
        while ( SDL_PollEvent(&event) )
            if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                    done = true;
                }
                if ( event.key.keysym.sym == SDLK_RETURN ) {
                    done = true;
                }
                if ( event.key.keysym.sym == SDLK_KP_ENTER ) {
                    done = true;
                }
                if ( event.key.keysym.sym == SDLK_1 ) {
                    return 0;
                }
                if ( event.key.keysym.sym == SDLK_2 ) {
                    return 1;
                }
                if ( event.key.keysym.sym == SDLK_3 ) {
                    return 2;
                }
                if ( event.key.keysym.sym == SDLK_4 ) {
                    return 3;
                }
                if ( event.key.keysym.sym == SDLK_5 ) {
                    return 4;
                }
                if ( event.key.keysym.sym == SDLK_6 ) {
                    return 5;
                }
                if ( event.key.keysym.sym == SDLK_7 ) {
                    return 6;
                }
                if ( event.key.keysym.sym == SDLK_KP1 ) {
                    return 0;
                }
                if ( event.key.keysym.sym == SDLK_KP2 ) {
                    return 1;
                }
                if ( event.key.keysym.sym == SDLK_KP3 ) {
                    return 2;
                }
                if ( event.key.keysym.sym == SDLK_KP4 ) {
                    return 3;
                }
                if ( event.key.keysym.sym == SDLK_KP5 ) {
                    return 4;
                }
                if ( event.key.keysym.sym == SDLK_KP6 ) {
                    return 5;
                }
                if ( event.key.keysym.sym == SDLK_KP7 ) {
                    return 6;
                }

            }

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            for (int i=0; i<7;i++)
            {
                if ((mousex>xplace+10)&&(mousex<xplace+410)&&(mousey>yplace+10+i*30)&&(mousey<yplace+38+i*30))
                    return i;
            }
        }

        SDL_GetMouseState(&mousex,&mousey);
        DrawIMG(background, screen, 0, 0);
        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen); //draws it all to the screen
    }while (!done);


    return 3; //Returns normal
}

//The function that allows the player to choose Level number
void startVsMenu()
{
    const int xplace = 200;
    const int yplace = 100;
    Uint8 *keys;
    int mousex, mousey;
    bool done = false;

    //Keeps track of background;
    //int nowTime=SDL_GetTicks();

    MakeBackground(xsize,ysize);
    SFont_Write(background,fBlueFont,360,650,"Press ESC to accept");
    DrawIMG(bBack,background,xsize/2-120/2,600);
    do
    {
        //nowTime=SDL_GetTicks();
        DrawIMG(background, screen, 0, 0);
        DrawIMG(changeButtonsBack,screen,xplace,yplace);
        SFont_Write(screen,fBlueFont,xplace+50,yplace+20,"Player 1");
        SFont_Write(screen,fBlueFont,xplace+300+50,yplace+20,"Player 2");
        SFont_Write(screen,fBlueFont,xplace+50,yplace+70,"Speed:");
        SFont_Write(screen,fBlueFont,xplace+50+300,yplace+70,"Speed:");
        for (int i=0; i<5;i++)
        {
            char levelS[2]; //level string;
            levelS[0]='1'+i;
            levelS[1]=0;
            SFont_Write(screen,fBlueFont,xplace+50+i*40,yplace+110,levelS);
            DrawIMG(iLevelCheckBox,screen,xplace+50+i*40,yplace+150);
            if (player1Speed==i)
                DrawIMG(iLevelCheck,screen,xplace+50+i*40,yplace+150);
        }
        for (int i=0; i<5;i++)
        {
            char levelS[2]; //level string;
            levelS[0]='1'+i;
            levelS[1]=0;
            SFont_Write(screen,fBlueFont,xplace+300+50+i*40,yplace+110,levelS);
            DrawIMG(iLevelCheckBox,screen,xplace+300+50+i*40,yplace+150);
            if (player2Speed==i)
                DrawIMG(iLevelCheck,screen,xplace+300+50+i*40,yplace+150);
        }
        SFont_Write(screen,fBlueFont,xplace+50,yplace+200,"AI: ");
        DrawIMG(iLevelCheckBox,screen,xplace+50+70,yplace+200);
        if (player1AI)
            DrawIMG(iLevelCheck,screen,xplace+50+70,yplace+200);
        SFont_Write(screen,fBlueFont,xplace+50,yplace+250,"TT Handicap: ");
        SFont_Write(screen,fBlueFont,xplace+50+300,yplace+200,"AI: ");
        DrawIMG(iLevelCheckBox,screen,xplace+50+70+300,yplace+200);
        if (player2AI)
            DrawIMG(iLevelCheck,screen,xplace+50+70+300,yplace+200);
        SFont_Write(screen,fBlueFont,xplace+50+300,yplace+250,"TT Handicap: ");
        for (int i=0; i<5;i++)
        {
            char levelS[2]; //level string;
            levelS[0]='1'+i;
            levelS[1]=0;
            SFont_Write(screen,fBlueFont,xplace+50+i*40,yplace+290,levelS);
            DrawIMG(iLevelCheckBox,screen,xplace+50+i*40,yplace+330);
            if (player1handicap==i)
                DrawIMG(iLevelCheck,screen,xplace+50+i*40,yplace+330);
        }
        for (int i=0; i<5;i++)
        {
            char levelS[2]; //level string;
            levelS[0]='1'+i;
            levelS[1]=0;
            SFont_Write(screen,fBlueFont,xplace+50+i*40+300,yplace+290,levelS);
            DrawIMG(iLevelCheckBox,screen,xplace+50+i*40+300,yplace+330);
            if (player2handicap==i)
                DrawIMG(iLevelCheck,screen,xplace+50+i*40+300,yplace+330);
        }

        SDL_Event event;
        while ( SDL_PollEvent(&event) )
            if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                    done = true;
                }
                if ( event.key.keysym.sym == SDLK_RETURN ) {
                    done = true;
                }
                if ( event.key.keysym.sym == SDLK_KP_ENTER ) {
                    done = true;
                }
            }

        keys = SDL_GetKeyState(NULL);

        SDL_GetMouseState(&mousex,&mousey);

        // If the mouse button is released, make bMouseUp equal true
        if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
        {
            bMouseUp=true;
        }

        if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
        {
            bMouseUp = false;

            if ((mousex>xplace+50+70)&&(mousey>yplace+200)&&(mousex<xplace+50+70+30)&&(mousey<yplace+200+30))
                player1AI=!player1AI;
            if ((mousex>xplace+50+70+300)&&(mousey>yplace+200)&&(mousex<xplace+50+70+30+300)&&(mousey<yplace+200+30))
                player2AI=!player2AI;
            for (int i=0; i<5;i++)
            {
                if ((mousex>xplace+50+i*40)&&(mousex<xplace+50+i*40+30)&&(mousey>yplace+150)&&(mousey<yplace+150+30))
                    player1Speed=i;
            }
            for (int i=0; i<5;i++)
            {
                if ((mousex>xplace+50+i*40+300)&&(mousex<xplace+50+i*40+30+300)&&(mousey>yplace+150)&&(mousey<yplace+150+30))
                    player2Speed=i;
            }
            for (int i=0; i<5;i++)
            {
                if ((mousex>xplace+50+i*40)&&(mousex<xplace+50+i*40+30)&&(mousey>yplace+330)&&(mousey<yplace+330+30))
                    player1handicap=i;
            }
            for (int i=0; i<5;i++)
            {
                if ((mousex>xplace+50+i*40+300)&&(mousex<xplace+50+i*40+30+300)&&(mousey>yplace+330)&&(mousey<yplace+330+30))
                    player2handicap=i;
            }
            if ((mousex>xsize/2-120/2)&&(mousex<xsize/2+120/2)&&(mousey>600)&&(mousey<640))
                done = true;
        }

        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen); //draws it all to the screen
        SDL_Delay(10);

    } while (!done);
    DrawIMG(background, screen, 0, 0);
}

//This function will promt for the user to select another file for puzzle mode
void changePuzzleLevels()
{
    char theFileName[30];
    strcpy(theFileName,puzzleName.c_str());
    for (int i=puzzleName.length();i<30;i++)
        theFileName[i]=' ';
    theFileName[29]=0;
    if (OpenFileDialogbox(200,100,theFileName))
    {
        for (int i=28;((theFileName[i]==' ')&&(i>0));i--)
            theFileName[i]=0;
        puzzleName = theFileName;
#if defined(__unix__)
        string home = getenv("HOME");
        puzzleSavePath = home+"/.gamesaves/blockattack/"+puzzleName+".save";
#elif defined(_WIN32)
        string home = getMyDocumentsPath();
        if (&home!=NULL)
        {
            puzzleSavePath = home+"/My Games/blockattack/"+puzzleName+".save";
        }
        else
        {
            puzzleSavePath = puzzleName+".save";
        }
#else
        puzzleSavePath = puzzleName+".save";
#endif
    }

}

class Keymenu {
    static void setXY(int x, int y)
    {
        keymenu.x = x;
        keymenu.y = y;
    }

    static void startMenu()
    {
        keymenu.activated = true;
        setXY(0,0);
    }

    static void stopMenu()
    {
        keymenu.activated = false;
    }
};

#if NETWORK
#include "NetworkThing.hpp"
//#include "MenuSystem.h"
#endif

//The main function, quite big... too big
int main(int argc, char *argv[])
{
    //We first create the folder there we will save (only on UNIX systems)
    //we call the external command "mkdir"... the user might have renamed this, but we hope he hasn't
#if defined(__unix__)
    //Compiler warns about unused result. The users envisonment should normally give the user all the information he needs
    if(system("mkdir -p ~/.gamesaves/blockattack/screenshots"))
        cout << "mkdir error creating ~/.gamesaves/blockattack/screenshots" << endl;
    if(system("mkdir -p ~/.gamesaves/blockattack/replays"))
        cout << "mkdir error creating ~/.gamesaves/blockattack/replays" << endl;
    if(system("mkdir -p ~/.gamesaves/blockattack/puzzles"))
        cout << "mkdir error creating ~/.gamesaves/blockattack/puzzles" << endl;
#elif defined(_WIN32)
    //Now for Windows NT/2k/xp/2k3 etc.
    string tempA = getMyDocumentsPath()+"\\My Games";
    CreateDirectory(tempA.c_str(),NULL);
    tempA = getMyDocumentsPath()+"\\My Games\\blockattack";
    CreateDirectory(tempA.c_str(),NULL);
    tempA = getMyDocumentsPath()+"\\My Games\\blockattack\\replays";
    CreateDirectory(tempA.c_str(),NULL);
    tempA = getMyDocumentsPath()+"\\My Games\\blockattack\\screenshots";
    CreateDirectory(tempA.c_str(),NULL);
#endif
    highPriority = false;	//if true the game will take most resources, but increase framerate.
    bFullscreen = false;
    if (argc > 1)
    {
        int argumentNr = 1;
        forceredraw = 2;
        while (argc>argumentNr)
        {
            char helpString[] = "--help";
            char priorityString[] = "-priority";
            char forceRedrawString[] = "-forceredraw";
            char forcepartdrawString[] = "-forcepartdraw";
            char singlePuzzleString[] = "-SP";
            char noSoundAtAll[] = "-nosound";
            char IntegratedEditor[] = "-editor";
            if (!(strncmp(argv[argumentNr],helpString,6)))
            {
                cout << "Block Attack Help" << endl << "--help Display this message" <<
                endl << "-priority  Starts game in high priority" << endl <<
                "-forceredraw  Redraw the whole screen every frame, prevents garbage" << endl <<
                "-forcepartdraw  Only draw what is changed, sometimes cause garbage" << endl <<
                "-nosound  No sound will be played at all, and sound hardware want be loaded (use this if game crashes because of sound)" << endl <<
                "-editor  Starts the build-in editor (not yet integrated)" << endl;
#ifdef WIN32
                system("Pause");
#endif
                return 0;
            }
            if (!(strncmp(argv[argumentNr],priorityString,9)))
            {
                cout << "Priority mode" << endl;
                highPriority = true;
            }
            if (!(strncmp(argv[argumentNr],forceRedrawString,12)))
            {
                forceredraw = 1;
            }
            if (!(strncmp(argv[argumentNr],forcepartdrawString,14)))
            {
                forceredraw = 2;
            }
            if (!(strncmp(argv[argumentNr],singlePuzzleString,3)))
            {
                singlePuzzle = true; //We will just have one puzzle
                if (argv[argumentNr+1][1]!=0)
                    singlePuzzleNr = (argv[argumentNr+1][1]-'0')+(argv[argumentNr+1][0]-'0')*10;
                else
                    singlePuzzleNr = (argv[argumentNr+1][0]-'0');
                singlePuzzleFile = argv[argumentNr+2];
                argumentNr+=2;
                cout << "SinglePuzzleMode, File: " << singlePuzzleFile << " and Level: " << singlePuzzleNr << endl;
            }
            if (!(strncmp(argv[argumentNr],noSoundAtAll,8)))
            {
                NoSound = true;
            }
            if (!(strncmp(argv[argumentNr],IntegratedEditor,7)))
            {
                #if LEVELEDITOR
                editorMode = true;
                cout << "Integrated Puzzle Editor Activated" << endl;
                #else
                cout << "Integrated Puzzle Editor was disabled at compile time" << endl;
                #endif
            }
            argumentNr++;
        }   //while
    }   //if

    SoundEnabled = true;
    MusicEnabled = true;
    int mousex, mousey;   //Mouse coordinates
    showOptions = false;
    b1playerOpen = false;
    b2playersOpen = false;
    bReplayOpen = false;
    bScreenLocked = false;
    bool twoPlayers = false;	//true if two players splitscreen
    bool vsMode = false;
    theTopScoresEndless = Highscore(1);
    theTopScoresTimeTrial = Highscore(2);
    drawBalls = true;
    puzzleLoaded = false;
    bool weWhereConnected = false;

    //Things used for repeating keystrokes:
    bool repeatingP1N = false; //The key is being held
    bool repeatingP1S = false; //The key is being held
    bool repeatingP1W = false; //The key is being held
    bool repeatingP1E = false; //The key is being held
    bool repeatingP2N = false; //The key is being held
    bool repeatingP2S = false; //The key is being held
    bool repeatingP2W = false; //The key is being held
    bool repeatingP2E = false; //The key is being held
    const int startRepeat = 200;
    const int repeatDelay = 100;    //Repeating
    unsigned long timeHeldP1N = 0;
    unsigned long timeHeldP1S = 0;
    unsigned long timeHeldP1E = 0;
    unsigned long timeHeldP1W = 0;
    unsigned long timeHeldP2N = 0;
    unsigned long timeHeldP2S = 0;
    unsigned long timeHeldP2E = 0;
    unsigned long timeHeldP2W = 0;
    unsigned long timesRepeatedP1N = 0;
    unsigned long timesRepeatedP1S = 0;
    unsigned long timesRepeatedP1E = 0;
    unsigned long timesRepeatedP1W = 0;
    unsigned long timesRepeatedP2N = 0;
    unsigned long timesRepeatedP2S = 0;
    unsigned long timesRepeatedP2E = 0;
    unsigned long timesRepeatedP2W = 0;

    theBallManeger = ballManeger();
    theExplosionManeger = explosionManeger();

//We now set the paths were we are saving, we are using the keyword __unix__ . I hope that all UNIX systems has a home folder
#if defined(__unix__)
    string home = getenv("HOME");
    string optionsPath = home+"/.gamesaves/blockattack/options.dat";
#elif defined(_WIN32)
    string home = getMyDocumentsPath();
    string optionsPath;
    if (&home!=NULL) //Null if no APPDATA dir exists (win 9x)
        optionsPath = home+"/My Games/blockattack/options.dat";
    else
        optionsPath = "options.dat";
#else
    string optionsPath = "options.dat";
#endif

#if defined(__unix__)
    stageClearSavePath = home+"/.gamesaves/blockattack/stageClear.SCsave";
    puzzleSavePath = home+"/.gamesaves/blockattack/puzzle.levels.save";
#elif defined(_WIN32)
    if (&home!=NULL)
    {
        stageClearSavePath = home+"/My Games/blockattack/stageClear.SCsave";
        puzzleSavePath = home+"/My Games/blockattack/puzzle.levels.save";
    }
    else
    {
        stageClearSavePath = "stageClear.SCsave";
        puzzleSavePath = "puzzle.levels.save";
    }
#else
    stageClearSavePath = "stageClear.SCsave";
    puzzleSavePath = "puzzle.levels.save";
#endif
    puzzleName="puzzle.levels";

    Uint8 *keys;



    //Init SDL
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        cout << "Unable to init SDL: " << SDL_GetError() << endl;
        exit(1);
    }
    atexit(SDL_Quit);		//quits SDL when the game stops for some reason (like you hit exit or Esc)
    
    SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);

    Joypad_init();    //Prepare the joysticks

    Joypad joypad1 = Joypad();    //Creates a joypad
    Joypad joypad2 = Joypad();    //Creates a joypad

    theTextManeger = textManeger();

    //Open Audio
    if (!NoSound) //If sound has not been disabled, then load the sound system
        if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0)
        {
            cout << "Warning: Couldn't set 44100 Hz 16-bit audio - Reason: " << SDL_GetError() << endl
            << "Sound will be disabled!" << endl;
            NoSound = true; //Tries to stop all sound from playing/loading
        }

    SDL_WM_SetCaption("Block Attack - Rise of the Blocks", NULL); //Sets title line
    

    //Copyright notice:
    cout << "Block Attack - Rise of the Blocks (" << VERSION_NUMBER << ")" << endl << "http://blockattack.sf.net" << endl << "Copyright 2004-2009 Poul Sander" << endl <<
    "A SDL based game (see www.libsdl.org)" << endl <<
    "The game is availeble under the GPL, see COPYING for details." << endl;
#if defined(_WIN32)
    cout << "Windows build" << endl;
#elif defined(__linux__)
    cout << "Linux build" <<  endl;
#elif defined(__unix__)
    cout << "Unix build" <<  endl;
#else
    cout << "Alternative build" << endl;
#endif
    cout << "-------------------------------------------" << endl;

    //The menu cannot be activated the first second to prevent unexpected events
    keymenu.canBeActivatedTime = SDL_GetTicks()+1000;
    keymenu.activated = false;
    for(int i=0;i<KEYMENU_MAXWITH;i++)
        keymenu.menumap[i][0] = true;

    keySettings[0].up= SDLK_UP;
    keySettings[0].down = SDLK_DOWN;
    keySettings[0].left = SDLK_LEFT;
    keySettings[0].right = SDLK_RIGHT;
    keySettings[0].change = SDLK_RCTRL;
    keySettings[0].push = SDLK_RSHIFT;

    keySettings[2].up= SDLK_w;
    keySettings[2].down = SDLK_s;
    keySettings[2].left = SDLK_a;
    keySettings[2].right = SDLK_d;
    keySettings[2].change = SDLK_LCTRL;
    keySettings[2].push = SDLK_LSHIFT;

    player1keys=0;
    player2keys=2;

    strcpy(player1name, "Player 1                    \0");
    strcpy(player2name, "Player 2                    \0");

    Config *configSettings = Config::getInstance();
    //configSettings->setString("aNumber"," A string");
    //configSettings->save();
    if(configSettings->exists("fullscreen")) //Test if an configFile exists
    {
        bFullscreen = (bool)configSettings->getInt("fullscreen");
        MusicEnabled = (bool)configSettings->getInt("musicenabled");
        SoundEnabled = (bool)configSettings->getInt("soundenabled");
        mouseplay1 = (bool)configSettings->getInt("mouseplay1");
        mouseplay2 = (bool)configSettings->getInt("mouseplay2");
        joyplay1 = (bool)configSettings->getInt("joypad1");
        joyplay2 = (bool)configSettings->getInt("joypad2");
        
        if(configSettings->exists("player1keyup")) keySettings[0].up = (SDLKey)configSettings->getInt("player1keyup");
        if(configSettings->exists("player1keydown")) keySettings[0].down = (SDLKey)configSettings->getInt("player1keydown");
        if(configSettings->exists("player1keyleft")) keySettings[0].left = (SDLKey)configSettings->getInt("player1keyleft");
        if(configSettings->exists("player1keyright")) keySettings[0].right = (SDLKey)configSettings->getInt("player1keyright");
        if(configSettings->exists("player1keychange")) keySettings[0].change = (SDLKey)configSettings->getInt("player1keychange");
        if(configSettings->exists("player1keypush")) keySettings[0].push = (SDLKey)configSettings->getInt("player1keypush");
        
        if(configSettings->exists("player2keyup")) keySettings[2].up = (SDLKey)configSettings->getInt("player2keyup");
        if(configSettings->exists("player2keydown")) keySettings[2].down = (SDLKey)configSettings->getInt("player2keydown");
        if(configSettings->exists("player2keyleft")) keySettings[2].left = (SDLKey)configSettings->getInt("player2keyleft");
        if(configSettings->exists("player2keyright")) keySettings[2].right = (SDLKey)configSettings->getInt("player2keyright");
        if(configSettings->exists("player2keychange")) keySettings[2].change = (SDLKey)configSettings->getInt("player2keychange");
        if(configSettings->exists("player2keypush")) keySettings[2].push = (SDLKey)configSettings->getInt("player2keypush");
        if(configSettings->exists("player1name"))
            strncpy(player1name,(configSettings->getString("player1name")).c_str(),28);
        if(configSettings->exists("player2name"))
            strncpy(player2name,(configSettings->getString("player2name")).c_str(),28);
        cout << "Data loaded from config file" << endl;
    }
    else
    {
        //Reads options from file:
        ifstream optionsFile(optionsPath.c_str(), ios::binary);
        if (optionsFile)
        {
            //reads data: xsize,ysize,fullescreen, player1keys, player2keys, MusicEnabled, SoundEnabled,player1name,player2name
            optionsFile.read(reinterpret_cast<char*>(&xsize), sizeof(int));
            optionsFile.read(reinterpret_cast<char*>(&ysize), sizeof(int));
            optionsFile.read(reinterpret_cast<char*>(&bFullscreen), sizeof(bool));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[0].up), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[0].down), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[0].left), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[0].right), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[0].change), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[0].push), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[2].up), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[2].down), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[2].left), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[2].right), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[2].change), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&keySettings[2].push), sizeof(SDLKey));
            optionsFile.read(reinterpret_cast<char*>(&MusicEnabled), sizeof(bool));
            optionsFile.read(reinterpret_cast<char*>(&SoundEnabled), sizeof(bool));
            optionsFile.read(player1name, 30*sizeof(char));
            optionsFile.read(player2name, 30*sizeof(char));
            //mouseplay?
            if (!optionsFile.eof())
            {
                optionsFile.read(reinterpret_cast<char*>(&mouseplay1), sizeof(bool));
                optionsFile.read(reinterpret_cast<char*>(&mouseplay2), sizeof(bool));
                optionsFile.read(reinterpret_cast<char*>(&joyplay1),sizeof(bool));
                optionsFile.read(reinterpret_cast<char*>(&joyplay2),sizeof(bool));
            }
            optionsFile.close();
            cout << "Data loaded from oldstyle options file" << endl;
        }
        else
        {
            cout << "Unable to load options file, using default values" << endl;
        }
    }
    
#if NETWORK
    strcpy(serverAddress, "192.168.0.2                 \0");
    if(configSettings->exists("address0"))
    {
        strcpy(serverAddress, "                            \0");
        strncpy(serverAddress,configSettings->getString("address0").c_str(),sizeof(serverAddress)-1);
    }
#endif

    if (singlePuzzle)
    {
        xsize=300;
        ysize=600;
    }
    
    
    //Open video
    if ((bFullscreen)&&(!singlePuzzle)) screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_FULLSCREEN|SDL_ANYFORMAT);
    else screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_ANYFORMAT);

    if ( screen == NULL )
    {
        cout << "Unable to set " << xsize << "x" << ysize << " video: " << SDL_GetError() << endl;
        exit(1);
    }

#if USE_ABSTRACT_FS
    //Init the file system abstraction layer
    PHYSFS_init(argv[0]);
    //Load default theme
    loadTheme("start");
#else
    //Loading all images into memory
    InitImages();
#endif
    //Now sets the icon:
    SDL_Surface *icon = IMG_Load2((char*)"gfx/icon.png");
    SDL_WM_SetIcon(icon,NULL);
    //SDL_FreeSurface(icon);

    cout << "Images loaded" << endl;

    //InitMenues();

    BlockGame theGame = BlockGame(50,100);			//creates game objects
    BlockGame theGame2 = BlockGame(xsize-500,100);
    if (singlePuzzle)
    {
        theGame.topy=0;
        theGame.topx=0;
        theGame2.topy=10000;
        theGame2.topx=10000;
    }
    theGame.DoPaintJob();			//Makes sure what there is something to paint
    theGame2.DoPaintJob();
    theGame.SetGameOver();		//sets the game over in the beginning
    theGame2.SetGameOver();


    //Takes names from file instead
    strcpy(theGame.name, player1name);
    strcpy(theGame2.name, player2name);

    //Keeps track of background;
    int nowTime=SDL_GetTicks();


#if NETWORK
    NetworkThing nt = NetworkThing();
    nt.setBGpointers(&theGame,&theGame2);
#endif

    if (singlePuzzle)
    {
        LoadPuzzleStages();
        theGame.NewPuzzleGame(singlePuzzleNr,0,0);
        showGame = true;
        vsMode = true;
    }
    //Draws everything to screen
    if (!editorMode)
        MakeBackground(xsize,ysize,theGame,theGame2);
    else
        MakeBackground(xsize,ysize,theGame);
    DrawIMG(background, screen, 0, 0);
    DrawEverything(xsize,ysize,theGame,theGame2);
    SDL_Flip(screen);
    //game loop
    int done = 0;
    cout << "Starting game loop" << endl;
    while (done == 0)
    {
        if (!(highPriority)) SDL_Delay(10);

        if ((standardBackground)&&(!editorMode))
        {
            MakeBackground(xsize,ysize,theGame,theGame2);
            DrawIMG(background, screen, 0, 0);
        }

        if ((standardBackground)&&(editorMode))
        {
            DrawIMG(backgroundImage, screen, 0, 0);
            MakeBackground(xsize,ysize,theGame);
            DrawIMG(background, screen, 0, 0);
        }

        //updates the balls and explosions:
        theBallManeger.update();
        theExplosionManeger.update();
        theTextManeger.update();

#if NETWORK
        if (nt.isConnected())
        {
            nt.updateNetwork();
            networkActive = true;
            if (!nt.isConnectedToPeer())
                DrawIMG(background, screen, 0, 0);
        }
        else
            networkActive = false;
        if (nt.isConnectedToPeer())
        {
            networkPlay=true;
            if (!weWhereConnected) //We have just connected
            {
                theGame.NewVsGame(50,100,&theGame2);
                theGame.putStartBlocks(nt.theSeed);
                theGame2.playNetwork(xsize-500,100);
                nt.theGameHasStarted();
                DrawIMG(background, screen, 0, 0);
            }
            weWhereConnected = true;
        }
        else
        {
            networkPlay=false;
            weWhereConnected = false;
        }
#endif

        if (!bScreenLocked)
        {
            SDL_Event event;

            while ( SDL_PollEvent(&event) )
            {
                if ( event.type == SDL_QUIT )  {
                    done = 1;
                }

                if ( event.type == SDL_KEYDOWN )
                {
                    if ( event.key.keysym.sym == SDLK_ESCAPE )
                    {
                            if (showOptions)
                            {
                                showOptions = false;
                            }
                            else
                                done=1;
                        DrawIMG(background, screen, 0, 0);

                    }
                    if ((!editorMode)&&(!editorModeTest)&&(!theGame.AI_Enabled))
                    {
                        //player1:
                        if ( event.key.keysym.sym == keySettings[player1keys].up ) {
                            theGame.MoveCursor('N');
                            repeatingP1N=true;
                            timeHeldP1N=SDL_GetTicks();
                            timesRepeatedP1N=0;
                        }
                        if ( event.key.keysym.sym == keySettings[player1keys].down ) {
                            theGame.MoveCursor('S');
                            repeatingP1S=true;
                            timeHeldP1S=SDL_GetTicks();
                            timesRepeatedP1S=0;
                        }
                        if ( (event.key.keysym.sym == keySettings[player1keys].left) && (showGame) ) {
                            theGame.MoveCursor('W');
                            repeatingP1W=true;
                            timeHeldP1W=SDL_GetTicks();
                            timesRepeatedP1W=0;
                        }
                        if ( (event.key.keysym.sym == keySettings[player1keys].right) && (showGame) ) {
                            theGame.MoveCursor('E');
                            repeatingP1E=true;
                            timeHeldP1E=SDL_GetTicks();
                            timesRepeatedP1E=0;
                        }
                        if ( event.key.keysym.sym == keySettings[player1keys].push ) {
                            theGame.PushLine();
                        }
                        if ( event.key.keysym.sym == keySettings[player1keys].change ) {
                            theGame.SwitchAtCursor();
                        }
                    }
                    if (!editorMode && !theGame2.AI_Enabled)
                    {
                        //player2:
                        if ( event.key.keysym.sym == keySettings[player2keys].up ) {
                            theGame2.MoveCursor('N');
                            repeatingP2N=true;
                            timeHeldP2N=SDL_GetTicks();
                            timesRepeatedP2N=0;
                        }
                        if ( event.key.keysym.sym == keySettings[player2keys].down ) {
                            theGame2.MoveCursor('S');
                            repeatingP2S=true;
                            timeHeldP2S=SDL_GetTicks();
                            timesRepeatedP2S=0;
                        }
                        if ( (event.key.keysym.sym == keySettings[player2keys].left) && (showGame) ) {
                            theGame2.MoveCursor('W');
                            repeatingP2W=true;
                            timeHeldP2W=SDL_GetTicks();
                            timesRepeatedP2W=0;
                        }
                        if ( (event.key.keysym.sym == keySettings[player2keys].right) && (showGame) ) {
                            theGame2.MoveCursor('E');
                            repeatingP2E=true;
                            timeHeldP2E=SDL_GetTicks();
                            timesRepeatedP2E=0;
                        }
                        if ( event.key.keysym.sym == keySettings[player2keys].push ) {
                            theGame2.PushLine();
                        }
                        if ( event.key.keysym.sym == keySettings[player2keys].change ) {
                            theGame2.SwitchAtCursor();
                        }
                    }
                    //common:
                    if ((!singlePuzzle)&&(!editorMode))
                    {
                        if ( event.key.keysym.sym == SDLK_F2 ) {
                            #if NETWORK
                            if ((!showOptions)&&(!networkActive)){
                            #else
                            if ((!showOptions)){
                            #endif
                                theGame.NewGame(50,100);
                                theGame.timetrial = false;
                                theGame.putStartBlocks();
                                closeAllMenus();
                                twoPlayers =false;
                                theGame2.SetGameOver();
                                showGame = true;
                                vsMode = false;
                                strcpy(theGame.name, player1name);
                                strcpy(theGame2.name, player2name);
                            }}
                        if ( event.key.keysym.sym == SDLK_F3 ) {
                            #if NETWORK
                            if ((!showOptions)&&(!networkActive)){
                            #else
                            if ((!showOptions)){    
                            #endif
                                theGame.NewGame(50,100);
                                theGame.timetrial = true;
                                theGame.putStartBlocks();
                                closeAllMenus();
                                twoPlayers =false;
                                theGame2.SetGameOver();
                                showGame = true;
                                vsMode = false;
                                strcpy(theGame.name, player1name);
                                strcpy(theGame2.name, player2name);
                            }}
                        if ( event.key.keysym.sym == SDLK_F5 )
                        {
                            #if NETWORK
                            if ((!showOptions)&&(!networkActive))
                            #else
                            if ((!showOptions))    
                            #endif
                            {
                                int myLevel = StageLevelSelect();
                                theGame.NewStageGame(myLevel,50,100);
                                MakeBackground(xsize,ysize,theGame,theGame2);
                                DrawIMG(background, screen, 0, 0);
                                closeAllMenus();
                                twoPlayers =false;
                                theGame2.SetGameOver();
                                showGame = true;
                                vsMode = false;
                                strcpy(theGame.name, player1name);
                                strcpy(theGame2.name, player2name);
                            }
                        }
                        if ( event.key.keysym.sym == SDLK_F6 )
                        {
                            #if NETWORK
                            if ((!showOptions)&&(!networkActive))
                            #else
                            if ((!showOptions))    
                            #endif
                            {
                                theGame.NewVsGame(50,100,&theGame2);
                                theGame2.NewVsGame(xsize-500,100,&theGame);
                                closeAllMenus();
                                vsMode = true;
                                strcpy(theGame.name, player1name);
                                strcpy(theGame2.name, player2name);
                                theGame.setGameSpeed(player1Speed);
                                theGame2.setGameSpeed(player2Speed);
                                theGame.setHandicap(player1handicap);
                                theGame2.setHandicap(player2handicap);
                                theGame.AI_Enabled = player1AI;
                                theGame2.AI_Enabled = player2AI;
                                theGame.setAIlevel(player1AIlevel);
                                theGame2.setAIlevel(player2AIlevel);
                                int theTime = time(0);
                                theGame.putStartBlocks(theTime);
                                theGame2.putStartBlocks(theTime);
                                twoPlayers = true;
                            }
                        }
                        if ( event.key.keysym.sym == SDLK_F4 )
                        {
                            #if NETWORK
                            if ((!showOptions)&&(!networkActive))
                            #else
                            if ((!showOptions))    
                            #endif
                            {
                                theGame.NewGame(50,100);
                                theGame2.NewGame(xsize-500,100);
                                theGame.timetrial = true;
                                theGame2.timetrial = true;
                                int theTime = time(0);
                                theGame.putStartBlocks(theTime);
                                theGame2.putStartBlocks(theTime);
                                closeAllMenus();
                                twoPlayers = true;
                                theGame.setGameSpeed(player1Speed);
                                theGame2.setGameSpeed(player2Speed);
                                theGame.setHandicap(player1handicap);
                                theGame2.setHandicap(player2handicap);
                                theGame.AI_Enabled = player1AI;
                                theGame2.AI_Enabled = player2AI;
                                theGame.setAIlevel(player1AIlevel);
                                theGame2.setAIlevel(player2AIlevel);
                                strcpy(theGame.name, player1name);
                                strcpy(theGame2.name, player2name);
                            }
                        }
                        if ( event.key.keysym.sym == SDLK_F7 )
                        {
                            #if NETWORK
                            if ((!showOptions)&&(!networkActive))
                            #else
                            if ((!showOptions))    
                            #endif
                            {
                                int myLevel = PuzzleLevelSelect();
                                theGame.NewPuzzleGame(myLevel,50,100);
                                MakeBackground(xsize,ysize,theGame,theGame2);
                                DrawIMG(background, screen, 0, 0);
                                closeAllMenus();
                                twoPlayers = false;
                                theGame2.SetGameOver();
                                showGame = true;
                                vsMode = true;
                                strcpy(theGame.name, player1name);
                                strcpy(theGame2.name, player2name);
                            }
                        }
                        if ( event.key.keysym.sym == SDLK_F8 )
                        {
                            if(!keymenu.activated && keymenu.canBeActivatedTime > SDL_GetTicks())
                            {
                                keymenu.activated = true;
                                keymenu.x = 0;
                                keymenu.y = 0;
                            }
                        }
                        if ( event.key.keysym.sym == SDLK_F9 ) {
                            writeScreenShot();
                        }
                        if ( event.key.keysym.sym == SDLK_F11 ) {
                            /*This is the test place, place function to test here*/

                            //theGame.CreateGreyGarbage();
                            //char mitNavn[30];
                            //SelectThemeDialogbox(300,400,mitNavn);
                           // MainMenu();
                            OpenScoresDisplay();
                        } //F11
                    }
                    if ( event.key.keysym.sym == SDLK_F12 ) {
                        done=1;
                    }
                }
            } //while event PollEvent - read keys

            /**********************************************************************
            **************************** Repeating start **************************
            **********************************************************************/

            keys = SDL_GetKeyState(NULL);
//Also the joysticks:
//Repeating not implemented

//Player 1 start
            if (!(keys[keySettings[player1keys].up]))
                repeatingP1N=false;
            while ((repeatingP1N)&&(keys[keySettings[player1keys].up])&&(SDL_GetTicks()>timeHeldP1N+timesRepeatedP1N*repeatDelay+startRepeat))
            {
                theGame.MoveCursor('N');
                timesRepeatedP1N++;
            }

            if (!(keys[keySettings[player1keys].down]))
                repeatingP1S=false;
            while ((repeatingP1S)&&(keys[keySettings[player1keys].down])&&(SDL_GetTicks()>timeHeldP1S+timesRepeatedP1S*repeatDelay+startRepeat))
            {
                theGame.MoveCursor('S');
                timesRepeatedP1S++;
            }

            if (!(keys[keySettings[player1keys].left]))
                repeatingP1W=false;
            while ((repeatingP1W)&&(keys[keySettings[player1keys].left])&&(SDL_GetTicks()>timeHeldP1W+timesRepeatedP1W*repeatDelay+startRepeat))
            {
                timesRepeatedP1W++;
                theGame.MoveCursor('W');
            }

            if (!(keys[keySettings[player1keys].right]))
                repeatingP1E=false;
            while ((repeatingP1E)&&(keys[keySettings[player1keys].right])&&(SDL_GetTicks()>timeHeldP1E+timesRepeatedP1E*repeatDelay+startRepeat))
            {
                timesRepeatedP1E++;
                theGame.MoveCursor('E');
            }

//Player 1 end

//Player 2 start
            if (!(keys[keySettings[player2keys].up]))
                repeatingP2N=false;
            while ((repeatingP2N)&&(keys[keySettings[player2keys].up])&&(SDL_GetTicks()>timeHeldP2N+timesRepeatedP2N*repeatDelay+startRepeat))
            {
                theGame2.MoveCursor('N');
                timesRepeatedP2N++;
            }

            if (!(keys[keySettings[player2keys].down]))
                repeatingP2S=false;
            while ((repeatingP2S)&&(keys[keySettings[player2keys].down])&&(SDL_GetTicks()>timeHeldP2S+timesRepeatedP2S*repeatDelay+startRepeat))
            {
                theGame2.MoveCursor('S');
                timesRepeatedP2S++;
            }

            if (!(keys[keySettings[player2keys].left]))
                repeatingP2W=false;
            while ((repeatingP2W)&&(keys[keySettings[player2keys].left])&&(SDL_GetTicks()>timeHeldP2W+timesRepeatedP2W*repeatDelay+startRepeat))
            {
                theGame2.MoveCursor('W');
                timesRepeatedP2W++;
            }

            if (!(keys[keySettings[player2keys].right]))
                repeatingP2E=false;
            while ((repeatingP2E)&&(keys[keySettings[player2keys].right])&&(SDL_GetTicks()>timeHeldP2E+timesRepeatedP2E*repeatDelay+startRepeat))
            {
                theGame2.MoveCursor('E');
                timesRepeatedP2E++;
            }

//Player 2 end

            /**********************************************************************
            **************************** Repeating end ****************************
            **********************************************************************/

            /**********************************************************************
            ***************************** Joypad start ****************************
            **********************************************************************/

            //Menu
            if(joypad1.working)
            {
                joypad1.update();
                if(joypad1.but1)
                {
                    if(!keymenu.activated && keymenu.canBeActivatedTime > SDL_GetTicks())
                    {
                        keymenu.activated = true;
                        keymenu.x = 0;
                        keymenu.y = 0;
                    }
                }
            }

            //Gameplay
            if (joyplay1||joyplay2)
            {
                if (joypad1.working && !theGame.AI_Enabled)
                    if (joyplay1)
                    {
                        joypad1.update();
                        if (joypad1.up)
                        {
                            theGame.MoveCursor('N');
                            repeatingP1N=true;
                            timeHeldP1N=SDL_GetTicks();
                            timesRepeatedP1N=0;
                        }
                        if (joypad1.down)
                        {
                            theGame.MoveCursor('S');
                            repeatingP1S=true;
                            timeHeldP1S=SDL_GetTicks();
                            timesRepeatedP1S=0;
                        }
                        if (joypad1.left)
                        {
                            theGame.MoveCursor('W');
                            repeatingP1W=true;
                            timeHeldP1W=SDL_GetTicks();
                            timesRepeatedP1W=0;
                        }
                        if (joypad1.right)
                        {
                            theGame.MoveCursor('E');
                            repeatingP1E=true;
                            timeHeldP1E=SDL_GetTicks();
                            timesRepeatedP1E=0;
                        }
                        if (joypad1.but1)
                            theGame.SwitchAtCursor();
                        if (joypad1.but2)
                            theGame.PushLine();
                    }
                    else
                    {
                        joypad1.update();
                        if (joypad1.up)
                        {
                            theGame2.MoveCursor('N');
                            repeatingP2N=true;
                            timeHeldP2N=SDL_GetTicks();
                            timesRepeatedP2N=0;
                        }
                        if (joypad1.down)
                        {
                            theGame2.MoveCursor('S');
                            repeatingP2S=true;
                            timeHeldP2S=SDL_GetTicks();
                            timesRepeatedP2S=0;
                        }
                        if (joypad1.left)
                        {
                            theGame2.MoveCursor('W');
                            repeatingP2W=true;
                            timeHeldP2W=SDL_GetTicks();
                            timesRepeatedP2W=0;
                        }
                        if (joypad1.right)
                        {
                            theGame2.MoveCursor('E');
                            repeatingP2E=true;
                            timeHeldP2E=SDL_GetTicks();
                            timesRepeatedP2E=0;
                        }
                        if (joypad1.but1)
                            theGame2.SwitchAtCursor();
                        if (joypad1.but2)
                            theGame2.PushLine();
                    }
                if (joypad2.working && !theGame2.AI_Enabled)
                    if (!joyplay2)
                    {
                        joypad2.update();
                        if (joypad2.up)
                        {
                            theGame.MoveCursor('N');
                            repeatingP1N=true;
                            timeHeldP1N=SDL_GetTicks();
                            timesRepeatedP1N=0;
                        }
                        if (joypad2.down)
                        {
                            theGame.MoveCursor('S');
                            repeatingP1S=true;
                            timeHeldP1S=SDL_GetTicks();
                            timesRepeatedP1S=0;
                        }
                        if (joypad2.left)
                        {
                            theGame.MoveCursor('W');
                            repeatingP1W=true;
                            timeHeldP1W=SDL_GetTicks();
                            timesRepeatedP1W=0;
                        }
                        if (joypad2.right)
                        {
                            theGame.MoveCursor('E');
                            repeatingP1E=true;
                            timeHeldP1E=SDL_GetTicks();
                            timesRepeatedP1E=0;
                        }
                        if (joypad2.but1)
                            theGame.SwitchAtCursor();
                        if (joypad2.but2)
                            theGame.PushLine();
                    }
                    else
                    {
                        joypad2.update();
                        if (joypad2.up)
                        {
                            theGame2.MoveCursor('N');
                            repeatingP2N=true;
                            timeHeldP2N=SDL_GetTicks();
                            timesRepeatedP2N=0;
                        }
                        if (joypad2.down)
                        {
                            theGame2.MoveCursor('S');
                            repeatingP2S=true;
                            timeHeldP2S=SDL_GetTicks();
                            timesRepeatedP2S=0;
                        }
                        if (joypad2.left)
                        {
                            theGame2.MoveCursor('W');
                            repeatingP2W=true;
                            timeHeldP2W=SDL_GetTicks();
                            timesRepeatedP2W=0;
                        }
                        if (joypad2.right)
                        {
                            theGame2.MoveCursor('E');
                            repeatingP2E=true;
                            timeHeldP2E=SDL_GetTicks();
                            timesRepeatedP2E=0;
                        }
                        if (joypad2.but1)
                            theGame2.SwitchAtCursor();
                        if (joypad2.but2)
                            theGame2.PushLine();
                    }
            }

            /**********************************************************************
            ***************************** Joypad end ******************************
            **********************************************************************/


            keys = SDL_GetKeyState(NULL);

            if(keymenu.activated)
            {
                mousex = keymenu.x*buttonXsize-10;
                mousey = keymenu.y*buttonYsize-10;
            }
            else
                SDL_GetMouseState(&mousex,&mousey);

            /********************************************************************
            **************** Here comes mouse play ******************************
            ********************************************************************/

            if ((mouseplay1)&&((!editorMode)&&(!theGame.AI_Enabled)||(editorModeTest))) //player 1
                if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600))
                {
                    int yLine, xLine;
                    yLine = ((100+600)-(mousey-100+theGame.pixels))/50;
                    xLine = (mousex-50+25)/50;
                    yLine-=2;
                    xLine-=1;
                    if ((yLine>10)&&(theGame.TowerHeight<12))
                        yLine=10;
                    if (((theGame.pixels==50)||(theGame.pixels==0)) && (yLine>11))
                        yLine=11;
                    if (yLine<0)
                        yLine=0;
                    if (xLine<0)
                        xLine=0;
                    if (xLine>4)
                        xLine=4;
                    theGame.cursorx=xLine;
                    theGame.cursory=yLine;
                }

            if ((mouseplay2)&&(!editorMode)&&(!theGame2.AI_Enabled)) //player 2
                if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600))
                {
                    int yLine, xLine;
                    yLine = ((100+600)-(mousey-100+theGame2.pixels))/50;
                    xLine = (mousex-(xsize-500)+25)/50;
                    yLine-=2;
                    xLine-=1;
                    if ((yLine>10)&&(theGame2.TowerHeight<12))
                        yLine=10;
                    if (((theGame2.pixels==50)||(theGame2.pixels==0)) && (yLine>11))
                        yLine=11;
                    if (yLine<0)
                        yLine=0;
                    if (xLine<0)
                        xLine=0;
                    if (xLine>4)
                        xLine=4;
                    theGame2.cursorx=xLine;
                    theGame2.cursory=yLine;
                }

            /********************************************************************
            **************** Here ends mouse play *******************************
            ********************************************************************/

            // If the mouse button is released, make bMouseUp equal true
            if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
            {
                bMouseUp=true;
            }

            // If the mouse button 2 is released, make bMouseUp2 equal true
            if ((SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3))!=SDL_BUTTON(3))
            {
                bMouseUp2=true;
            }

            if ((!singlePuzzle)&&(!editorMode))
            {
                //read mouse events
                if (SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(1) && bMouseUp)
                {
                    bMouseUp = false;
                    DrawIMG(background, screen, 0, 0);
                    #if NETWORK
                    //Nothing
                    #else
                    bool networkActive=false;
                    #endif
                    if ((0<mousex) && (mousex<buttonXsize) && (0<mousey) && (mousey<40) &&(!networkActive) )
                    {
                        //New game clicked
                        bool state = (!bNewGameOpen);
                        closeAllMenus();
                        bNewGameOpen = state; //theGame.NewGame(50,100);
                        showOptions = false;
                    }
                    else
#if NETWORK
                        if ((0<mousex) && (mousex<buttonXsize) && (0<mousey) && (mousey<40) &&(networkActive))
                        {
                            //Disconnect clicked!
                            cout << "Disconnect clicked!" << endl;
                            nt.ntDisconnect();
                        }
                        else
#endif
                            if ((0<mousex) && (mousex<buttonXsize) && (40<mousey) && (mousey<80) && (bNewGameOpen) &&(!networkActive))
                            {
                                //1player
                                b1playerOpen = (!b1playerOpen);
                                b2playersOpen = false;
#if NETWORK
                                bNetworkOpen = false;
#endif
                            }
                            else
                                if ((0<mousex) && (mousex<buttonXsize) && (80<mousey) && (mousey<120) && (bNewGameOpen) &&(!networkActive))
                                {
                                    //2player
                                    b2playersOpen = (!b2playersOpen);
                                    b1playerOpen = false;
#if NETWORK
                                    bNetworkOpen = false;
#endif
                                }
#if NETWORK
                                else
                                    if ((0<mousex) && (mousex<buttonXsize) && (120<mousey) && (mousey<160) && (bNewGameOpen) &&(!networkActive))
                                    {
                                        //Network
                                        b1playerOpen = false;
                                        b2playersOpen = false;
                                        bNetworkOpen = (!bNetworkOpen);
                                    }
#endif
                                    else
                                        if ((buttonXsize<mousex) && (mousex<240) && (40<mousey) && (mousey<80) && (b1playerOpen) &&(!networkActive))
                                        {
                                            //1 player - endless
                                            theGame.NewGame(50,100);
                                            theGame.putStartBlocks();
                                            bNewGameOpen = false;
                                            b1playerOpen = false;
                                            twoPlayers =false;
                                            theGame2.SetGameOver();
                                            showGame = true;
                                            strcpy(theGame.name, player1name);
                                            strcpy(theGame2.name, player2name);
                                        }
                                        else
                                            if ((buttonXsize<mousex) && (mousex<240) && (80<mousey) && (mousey<120) && (b1playerOpen) &&(!networkActive) )
                                            {
                                                //1 player - time trial
                                                theGame.NewGame(50,100);
                                                theGame.timetrial=true;
                                                theGame.putStartBlocks();
                                                bNewGameOpen = false;
                                                b1playerOpen = false;
                                                twoPlayers =false;
                                                theGame2.SetGameOver();
                                                showGame = true;
                                                strcpy(theGame.name, player1name);
                                                strcpy(theGame2.name, player2name);
                                            }
                                            else
                                                if ((buttonXsize<mousex) && (mousex<240) && (120<mousey) && (mousey<160) && (b1playerOpen)&&(!networkActive) )
                                                {
                                                    //1 player - stage clear
                                                    bNewGameOpen = false;
                                                    b1playerOpen = false;
                                                    int myLevel = StageLevelSelect();
                                                    theGame.NewStageGame(myLevel,50,100);
                                                    MakeBackground(xsize,ysize,theGame,theGame2);
                                                    DrawIMG(background, screen, 0, 0);
                                                    twoPlayers =false;
                                                    theGame2.SetGameOver();
                                                    showGame = true;
                                                    vsMode = false;
                                                    strcpy(theGame.name, player1name);
                                                    strcpy(theGame2.name, player2name);
                                                }
                                                else
                                                    if ((buttonXsize<mousex) && (mousex<240) && (160<mousey) && (mousey<200) && (b1playerOpen))
                                                    {
                                                        //1 player - puzzle
                                                        bNewGameOpen = false;
                                                        b1playerOpen = false;
                                                        int myLevel = PuzzleLevelSelect();
                                                        theGame.NewPuzzleGame(myLevel,50,100);
                                                        MakeBackground(xsize,ysize,theGame,theGame2);
                                                        DrawIMG(background, screen, 0, 0);
                                                        twoPlayers =false;
                                                        theGame2.SetGameOver();
                                                        showGame = true;
                                                        vsMode = false;
                                                        strcpy(theGame.name, player1name);
                                                        strcpy(theGame2.name, player2name);
                                                    }
                                                    else
                                                        if ((buttonXsize<mousex) && (mousex<240) && (200<mousey) && (mousey<240) && (b1playerOpen))
                                                        {
                                                            //1 player - Vs mode
                                                            bNewGameOpen = false;
                                                            b1playerOpen = false;
                                                            int theAIlevel = startSingleVs();
                                                            theGame.NewVsGame(50,100,&theGame2);
                                                            theGame2.NewVsGame(xsize-500,100,&theGame);
                                                            MakeBackground(xsize,ysize,theGame,theGame2);
                                                            DrawIMG(background, screen, 0, 0);
                                                            twoPlayers = true; //Single player, but AI plays
                                                            showGame = true;
                                                            vsMode = true;
                                                            theGame2.AI_Enabled=true; //Of course we need an AI
                                                            theGame2.setAIlevel((Uint8)theAIlevel);
                                                            int theTime = time(0);
                                                            theGame.putStartBlocks(theTime);
                                                            theGame2.putStartBlocks(theTime);
                                                            strcpy(theGame.name, player1name);
                                                            strcpy(theGame2.name, player2name);
                                                        }
                                                        else
                                                            if ((buttonXsize<mousex) && (mousex<240) && (80<mousey) && (mousey<120) && (b2playersOpen))
                                                            {
                                                                //2 player - time trial
                                                                theGame.NewGame(50,100);
                                                                theGame.timetrial=true;
                                                                bNewGameOpen = false;
                                                                b2playersOpen = false;
                                                                theGame.NewGame(50,100);
                                                                theGame2.NewGame(xsize-500,100);
                                                                theGame.timetrial = true;
                                                                theGame2.timetrial = true;
                                                                int theTime = time(0);
                                                                theGame.putStartBlocks(theTime);
                                                                theGame2.putStartBlocks(theTime);
                                                                theGame.setGameSpeed(player1Speed);
                                                                theGame2.setGameSpeed(player2Speed);
                                                                theGame.setHandicap(player1handicap);
                                                                theGame2.setHandicap(player2handicap);
                                                                theGame.AI_Enabled = player1AI;
                                                                theGame2.AI_Enabled = player2AI;
                                                                theGame.setAIlevel(player1AIlevel);
                                                                theGame2.setAIlevel(player2AIlevel);
                                                                twoPlayers = true;
                                                                strcpy(theGame.name, player1name);
                                                                strcpy(theGame2.name, player2name);
                                                            }
                                                            else
                                                                if ((buttonXsize<mousex) && (mousex<240) && (120<mousey) && (mousey<160) && (b2playersOpen))
                                                                {
                                                                    //2 player - VsMode
                                                                    theGame.NewVsGame(50,100,&theGame2);
                                                                    theGame2.NewVsGame(xsize-500,100,&theGame);
                                                                    bNewGameOpen = false;
                                                                    vsMode = true;
                                                                    twoPlayers = true;
                                                                    b2playersOpen = false;
                                                                    theGame.setGameSpeed(player1Speed);
                                                                    theGame2.setGameSpeed(player2Speed);
                                                                    theGame.setHandicap(player1handicap);
                                                                    theGame2.setHandicap(player2handicap);
                                                                    theGame.AI_Enabled = player1AI;
                                                                    theGame2.AI_Enabled = player2AI;
                                                                    theGame.setAIlevel(player1AIlevel);
                                                                    theGame2.setAIlevel(player2AIlevel);
                                                                    int theTime = time(0);
                                                                    theGame.putStartBlocks(theTime);
                                                                    theGame2.putStartBlocks(theTime);
                                                                    strcpy(theGame.name, player1name);
                                                                    strcpy(theGame2.name, player2name);
                                                                }
#if NETWORK
                                                                else
                                                                    if ((buttonXsize<mousex) && (mousex<240) && (120<mousey) && (mousey<160) && (bNetworkOpen))
                                                                    {
                                                                        //Host
                                                                        cout << "Host" << endl;
                                                                        closeAllMenus();
                                                                        theGame.SetGameOver();
                                                                        theGame2.SetGameOver();
                                                                        nt.startServer();
                                                                    }
                                                                    else
                                                                        if ((buttonXsize<mousex) && (mousex<240) && (160<mousey) && (mousey<200) && (bNetworkOpen))
                                                                        {
                                                                            //Connect
                                                                            cout << "Connect" << endl;
                                                                            closeAllMenus();
                                                                            theGame.SetGameOver();
                                                                            theGame2.SetGameOver();
                                                                            if (OpenDialogbox(200,100,serverAddress))
                                                                            {
                                                                                char buf[30];
                                                                                memcpy(buf,serverAddress,30);
                                                                                nt.connectToServer(strtok(buf," "));
                                                                            }
                                                                            while ( SDL_PollEvent(&event) ); //If the user have pressed ESC or the like
                                                                        }
#endif
                                                                        else
                                                                            if ((buttonXsize<mousex) && (mousex<2*buttonXsize) && (0<mousey) && (mousey<40) &&(!networkActive))
                                                                            {
                                                                                //options button clicked
                                                                                if (bOptionsOpen)
                                                                                {
                                                                                    closeAllMenus();
                                                                                }
                                                                                else
                                                                                {
                                                                                    closeAllMenus();
                                                                                    bOptionsOpen = true;
                                                                                    DrawIMG(background, screen, 0, 0);
                                                                                }
                                                                            }
                                                                            else
                                                                                if ((buttonXsize<mousex) && (mousex<2*buttonXsize) && (40<mousey) && (mousey<80) && (bOptionsOpen) )
                                                                                {
                                                                                    //Configure button clicked
                                                                                    closeAllMenus();
                                                                                    if (!showOptions) {
                                                                                        showOptions = true;
                                                                                    }
                                                                                    else showOptions = false;
                                                                                    DrawIMG(background, screen, 0, 0);
                                                                                }
                                                                                else
                                                                                    if ((buttonXsize<mousex) && (mousex<2*buttonXsize) && (80<mousey) && (mousey<120) && (bOptionsOpen) )
                                                                                    {
                                                                                        //Configure button clicked
                                                                                        closeAllMenus();
                                                                                        changePuzzleLevels();
                                                                                    }
                                                                                    else
                                                                                        if ((buttonXsize<mousex) && (mousex<2*buttonXsize) && (120<mousey) && (mousey<160) && (bOptionsOpen) )
                                                                                        {
                                                                                            //vsMode button clicked
                                                                                            closeAllMenus();
                                                                                            startVsMenu();
                                                                                        }
                                                                                        else
                                                                                        if ((buttonXsize<mousex) && (mousex<2*buttonXsize) && (160<mousey) && (mousey<200) && (bOptionsOpen) )
                                                                                        {
                                                                                            //selectThemClicked
                                                                                            closeAllMenus();
                                                                                            char temp[30];
                                                                                            SelectThemeDialogbox(200,100,temp);
                                                                                        }
                                                                                        else
                                                                                            if ((buttonXsize*2<mousex) && (mousex<3*buttonXsize) && (0<mousey) && (mousey<40) &&(!networkActive))
                                                                                            {
                                                                                                //highscore button clicked
                                                                                                OpenScoresDisplay();

                                                                                            }
                                                                                            else
                                                                                                if ((360<mousex) && (mousex<4*buttonXsize) && (0<mousey) && (mousey<40) &&(!networkActive))
                                                                                                {
                                                                                                    //Replay clicked!
                                                                                                    bool state = (!bReplayOpen);
                                                                                                    closeAllMenus();
                                                                                                    bReplayOpen = state; //theGame.NewGame(50,100);
                                                                                                    showOptions = false;
                                                                                                }
                                                                                                else
                                                                                                    if ((360<mousex) && (mousex<4*buttonXsize) && (40<mousey) && (mousey<80) &&(bReplayOpen))
                                                                                                    {
                                                                                                        //Replay->Save clicked!
                                                                                                        //cout << "Replay->Save clicked" << endl;
                                                                                                        char buf[30];
                                                                                                        for (int i=0;i<29;i++)buf[i]=' ';
                                                                                                        buf[30]=0;
                                                                                                        OpenDialogbox(200,100,buf);
                                                                                                        for (int i=28;buf[i]==' ';i--)
                                                                                                            buf[i]=0;
#ifdef __unix__
                                                                                                        string saveHere = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/replays/"+(string)buf;
#elif WIN32
                                                                                                        string saveHere = home+(string)"/My Games/blockattack/replays/"+(string)buf;
#else
                                                                                                        string saveHere = (string)"./replays"+(string)buf;
#endif
                                                                                                        if (lastNrOfPlayers<2)
                                                                                                            theGame.theReplay.saveReplay(saveHere);
                                                                                                        else
                                                                                                            theGame.theReplay.saveReplay(saveHere,theGame2.theReplay);

                                                                                                        closeAllMenus();
                                                                                                    }
                                                                                                    else
                                                                                                        if ((360<mousex) && (mousex<4*buttonXsize) && (80<mousey) && (mousey<120)&&(bReplayOpen))
                                                                                                        {
                                                                                                            //Replay->Load clicked!
                                                                                                            //cout << "Replay->Load clicked" << endl;

                                                                                                            char buf[30];
                                                                                                            for (int i=0;i<29;i++)buf[i]=' ';
                                                                                                            buf[30]=0;
                                                                                                            if (OpenReplayDialogbox(50,100,buf))
                                                                                                            {
                                                                                                                //cout << "Good way" << endl;
                                                                                                                for (int i=28;buf[i]==' ';i--)
                                                                                                                    buf[i]=0;
#ifdef __unix__
                                                                                                                string loadThis = (string)getenv("HOME")+(string)"/.gamesaves/blockattack/replays/"+(string)buf;
#elif WIN32
                                                                                                                string loadThis = home+(string)"/My Games/blockattack/replays/"+(string)buf;
#else
                                                                                                                string loadThis = (string)"./replays/"+(string)buf;
#endif
                                                                                                                ifstream replayFileIn;
                                                                                                                replayFileIn.open(loadThis.c_str(),ios::binary);
                                                                                                                bool play2 = false;
                                                                                                                if (replayFileIn)
                                                                                                                {
                                                                                                                    Uint8 version = 0;
                                                                                                                    replayFileIn.read(reinterpret_cast<char*>(&version),sizeof(Uint8));                                                                                                                  replayFileIn.close();
                                                                                                                    Replay r1;
                                                                                                                    if(r1.loadReplay(loadThis))
                                                                                                                    {
                                                                                                                        Replay r2;
                                                                                                                        play2 = r2.loadReplay2(loadThis);
                                                                                                                        theGame.playReplay(50,100); //Fejlen sker her
                                                                                                                        theGame.theReplay = r1;
                                                                                                                        if (play2)
                                                                                                                        {
                                                                                                                            theGame2.playReplay(xsize-500,100);
                                                                                                                            theGame2.theReplay = r2;
                                                                                                                        }
                                                                                                                        else
                                                                                                                            theGame2.SetGameOver();
                                                                                                                        cout << "Replay should have been read" << endl;
                                                                                                                    }
                                                                                                                    else
                                                                                                                        cout << "Failed to read replay" << endl;
                                                                                                                }
                                                                                                            }
                                                                                                            closeAllMenus();
                                                                                                        }
                                                                                                        else
                                                                                                            if ((xsize-120<mousex) && (xsize-20>mousex) && (ysize-buttonXsize<mousey) && (ysize-20>mousey))
                                                                                                            {
                                                                                                                //Exit clicked
                                                                                                                done=1;
                                                                                                            }
                                                                                                            else
                                                                                                                if ((showOptions) && (mousex>500) && (mousex<560) && (mousey>220) && (mousey<260))
                                                                                                                {
                                                                                                                    MusicEnabled = !MusicEnabled;
                                                                                                                    if (!MusicEnabled) Mix_FadeOutMusic(500);
                                                                                                                }
                    if ((showOptions) && (mousex>500) && (mousex<560) && (mousey>270) && (mousey<310))
                    {
                        SoundEnabled = !SoundEnabled;
                    }
                    if ((showOptions) && (mousex>500) && (mousex<560) && (mousey>320) && (mousey<360))
                    {
                        //Fullscreen
                        bFullscreen = !bFullscreen;
#if defined(WIN32)
                        if (bFullscreen) screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_FULLSCREEN|SDL_ANYFORMAT);
                        else screen=SDL_SetVideoMode(xsize,ysize,32,SDL_SWSURFACE|SDL_ANYFORMAT);
                        DrawIMG(background, screen, 0, 0);
#else
                        SDL_WM_ToggleFullScreen(screen); //Will only work in Linux
#endif
                        SDL_ShowCursor(SDL_DISABLE);
                    }

                    if ((showOptions) && (mousex>330) && (mousex<470) && (mousey>535) && (mousey<585))
                    {
                        //change name
                        bScreenLocked = true;
                        showDialog = true;
                        strcpy(theGame.name, player1name);
                        if (OpenDialogbox(200,100,player1name))
                            strcpy(theGame.name, player1name);
                        else
                            strcpy(player1name,theGame.name);
                        bScreenLocked = false;
                        showDialog = false;
                        MakeBackground(xsize,ysize,theGame,theGame2);
                        DrawIMG(background, screen, 0, 0);
                    }
                    if ((showOptions) && (mousex>330) && (mousex<470) && (mousey>600) && (mousey<640))
                    {
                        //change name
                        bScreenLocked = true;
                        showDialog = true;
                        strcpy(theGame2.name, player2name);
                        if (OpenDialogbox(200,100,player2name))
                            strcpy(theGame2.name, player2name);
                        else
                            strcpy(player2name,theGame2.name);
                        bScreenLocked = false;
                        showDialog = false;
                        MakeBackground(xsize,ysize,theGame,theGame2);
                        DrawIMG(background, screen, 0, 0);
                    }
                    if ((showOptions) && (mousex>510) && (mousex<630) && (mousey>535) && (mousey<585))
                    {
                        //changeControls
                        OpenControlsBox(200,100,0);
                        MakeBackground(xsize,ysize,theGame,theGame2);
                        DrawIMG(background, screen, 0, 0);
                    }
                    if ((showOptions) && (mousex>510) && (mousex<630) && (mousey>600) && (mousey<640))
                    {
                        //changeControls
                        OpenControlsBox(200,100,2);
                        MakeBackground(xsize,ysize,theGame,theGame2);
                        DrawIMG(background, screen, 0, 0);
                    }

                    /********************************************************************
                    **************** Here comes mouse play ******************************
                    ********************************************************************/
                    if ((!showOptions))
                    {
                        if (mouseplay1 && !theGame.AI_Enabled) //player 1
                            if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600))
                            {
                                theGame.SwitchAtCursor();
                            }
                        if (mouseplay2 && !theGame2.AI_Enabled) //player 2
                            if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600))
                            {
                                theGame2.SwitchAtCursor();
                            }
                    }
                    /********************************************************************
                    **************** Here ends mouse play *******************************
                    ********************************************************************/

                    if(stageButtonStatus != SBdontShow && (mousex > theGame.topx+cordNextButton.x)
                            &&(mousex < theGame.topx+cordNextButton.x+cordNextButton.xsize)
                            &&(mousey > theGame.topy+cordNextButton.y)&&(mousey < theGame.topy+cordNextButton.y+cordNextButton.ysize))
                    {
                        //Clicked the next button after a stage clear or puzzle
                        theGame.nextLevel();
                    }
                    if(stageButtonStatus != SBdontShow && (mousex > theGame.topx+cordRetryButton .x)
                            &&(mousex < theGame.topx+cordRetryButton.x+cordRetryButton.xsize)
                            &&(mousey > theGame.topy+cordRetryButton.y)&&(mousey < theGame.topy+cordRetryButton.y+cordRetryButton.ysize))
                    {
                        //Clicked the retry button
                        theGame.retryLevel();
                    }
                        
                    
                    //cout << "Mouse x: " << mousex << ", mouse y: " << mousey << endl;
                }

                //Mouse button 2:
                if ((SDL_GetMouseState(NULL,NULL)&SDL_BUTTON(3))==SDL_BUTTON(3) && bMouseUp2)
                {
                    bMouseUp2=false; //The button is pressed
                    /********************************************************************
                    **************** Here comes mouse play ******************************
                    ********************************************************************/
                    if (!showOptions)
                    {
                        if (mouseplay1 && !theGame.AI_Enabled) //player 1
                            if ((mousex > 50)&&(mousey>100)&&(mousex<50+300)&&(mousey<100+600))
                            {
                                theGame.PushLine();
                            }
                        if (mouseplay2 && !theGame2.AI_Enabled) //player 2
                            if ((mousex > xsize-500)&&(mousey>100)&&(mousex<xsize-500+300)&&(mousey<100+600))
                            {
                                theGame2.PushLine();
                            }
                    }
                    /********************************************************************
                    **************** Here ends mouse play *******************************
                    ********************************************************************/
                }
            } //if !singlePuzzle
            else
            {

            }
        } //if !bScreenBocked;


        //Sees if music is stopped and if music is enabled
        if ((!NoSound)&&(!Mix_PlayingMusic())&&(MusicEnabled)&&(!bNearDeath))
        {
            // then starts playing it.
            Mix_VolumeMusic(MIX_MAX_VOLUME);
            Mix_PlayMusic(bgMusic, -1); //music loop
        }

        if(bNearDeath!=bNearDeathPrev)
        {
            if(bNearDeath)
            {
                if(!NoSound &&(MusicEnabled)) {
                    Mix_VolumeMusic(MIX_MAX_VOLUME);
                    Mix_PlayMusic(highbeatMusic, 1);
                }
            }
            else
            {
                if(!NoSound &&(MusicEnabled)) {
                    Mix_VolumeMusic(MIX_MAX_VOLUME);
                    Mix_PlayMusic(bgMusic, -1);
                }
            }
        }

        bNearDeathPrev = bNearDeath;


        //set bNearDeath to false theGame*.Update() will change to true as needed
        bNearDeath = false;
        //Updates the objects
        theGame.Update();
        theGame2.Update();

//see if anyone has won (two players only)
        #if NETWORK
        if (!networkPlay)
        #endif
            if (twoPlayers)
            {
                lastNrOfPlayers = 2;
                if ((theGame.bGameOver) && (theGame2.bGameOver))
                {
                    if (theGame.score+theGame.handicap>theGame2.score+theGame2.handicap)
                        theGame.setPlayerWon();
                    else
                        if (theGame.score+theGame.handicap<theGame2.score+theGame2.handicap)
                            theGame2.setPlayerWon();
                        else {
                            theGame.setDraw();
                            theGame2.setDraw();
                        }
                    twoPlayers = false;
                }
                if ((theGame.bGameOver) && (!theGame2.bGameOver))
                {
                    theGame2.setPlayerWon();
                    twoPlayers = false;
                }
                if ((!theGame.bGameOver) && (theGame2.bGameOver))
                {
                    theGame.setPlayerWon();
                    twoPlayers = false;
                }
            }

        //Once evrything has been checked, update graphics
        DrawEverything(xsize,ysize,theGame, theGame2);
        SDL_GetMouseState(&mousex,&mousey);
        //Remember mouse placement
        oldMousex = mousex;
        oldMousey = mousey;
        //Draw the mouse:
        DrawIMG(mouse,screen,mousex,mousey);
        SDL_Flip(screen);
    } //game loop



    //Saves options
    if (!editorMode)
    {
        configSettings->setInt("fullscreen",(int)bFullscreen);
        configSettings->setInt("musicenabled",(int)MusicEnabled);
        configSettings->setInt("soundenabled",(int)SoundEnabled);
        configSettings->setInt("mouseplay1",(int)mouseplay1);
        configSettings->setInt("mouseplay2",(int)mouseplay2);
        configSettings->setInt("joypad1",(int)joyplay1);
        configSettings->setInt("joypad2",(int)joyplay2);
        
        configSettings->setInt("player1keyup",(int)keySettings[0].up);
        configSettings->setInt("player1keydown",(int)keySettings[0].down);
        configSettings->setInt("player1keyleft",(int)keySettings[0].left);
        configSettings->setInt("player1keyright",(int)keySettings[0].right);
        configSettings->setInt("player1keychange",(int)keySettings[0].change);
        configSettings->setInt("player1keypush",(int)keySettings[0].push);
        
        configSettings->setInt("player2keyup",(int)keySettings[2].up);
        configSettings->setInt("player2keydown",(int)keySettings[2].down);
        configSettings->setInt("player2keyleft",(int)keySettings[2].left);
        configSettings->setInt("player2keyright",(int)keySettings[2].right);
        configSettings->setInt("player2keychange",(int)keySettings[2].change);
        configSettings->setInt("player2keypush",(int)keySettings[2].push);
        
        configSettings->setString("player1name",player1name);
        configSettings->setString("player2name",player2name);
        configSettings->save();
    }

    //calculate uptime:
    //int hours, mins, secs,
    commonTime ct = TimeHandler::ms2ct(SDL_GetTicks());

    cout << "Block Attack - Rise of the Blocks ran for: " << ct.hours << " hours " << ct.minutes << " mins and " << ct.seconds << " secs" << endl;

    ct = TimeHandler::addTime("totalTime",ct);

    cout << "Total run time is now: " << ct.days << " days " << ct.hours << " hours " << ct.minutes << " mins and " << ct.seconds << " secs" << endl;

    Stats::getInstance()->save();

    Config::getInstance()->save();
    
    //Frees memory from music and fonts
    //This is done after writing of configurations and stats since it often crashes the program :(
    UnloadImages();
    
#if USE_ABSTRACT_FS    
    //Close file system Apstraction layer!
    PHYSFS_deinit();
#endif
    return 0;
}
