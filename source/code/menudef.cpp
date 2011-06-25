/*
menudef.cpp
Copyright (C) 2011 Poul Sander

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
    blockattack@poulsander.com
*/

#include "global.hpp"
#include <iostream>
#include <stdlib.h>
#include "menu/MenuItem.hpp"
#include "MenuSystem.h"

using namespace std;

//Menu
void PrintHi()
{
    cout << "Hi" <<endl;
}

//Stores the controls
struct control
{
    SDLKey up;
    SDLKey down;
    SDLKey left;
    SDLKey right;
    SDLKey change;
    SDLKey push;
};

bool OpenDialogbox(int x, int y, char *name);
void OpenScoresDisplay();
extern control keySettings[3];

//Function to return the name of a key, to be displayed...
static string getKeyName(SDLKey key)
{
    string keyname(SDL_GetKeyName(key));
    cout << key << " translated to " << keyname << endl;
    return keyname;
}

class Button_changekey : public Button {
private:
   SDLKey *m_key2change; 
   string m_keyname;
public:
    Button_changekey(SDLKey *key, string keyname);
    void doAction();
};

Button_changekey::Button_changekey(SDLKey* key, string keyname) {
    m_key2change = key;
    m_keyname = keyname;
    setLabel(m_keyname+" : "+getKeyName(*m_key2change));
}

void Button_changekey::doAction() {
    setLabel(m_keyname+" : "+getKeyName(*m_key2change));
}

void InitMenues()
{
    ButtonGfx::setSurfaces(menuMarked,menuUnmarked);
    ButtonGfx::thefont = nf_scoreboard_font;
}

void runGame(int gametype);

void runSinglePlayerEndless(Button* b) {
    runGame(0);
}

void runSinglePlayerTimeTrial(Button* b) {
    runGame(1);
}

void runSinglePlayerPuzzle(Button* b) {
    runGame(3);
}

void runSinglePlayerVs(Button* b) {
    runGame(4);
}

void buttonActionMusic(Button* b) {
    MusicEnabled = !MusicEnabled;
    b->setLabel(MusicEnabled? "Music: On" : "Music: Off");
}

void buttonActionSound(Button* b) {
    SoundEnabled = !SoundEnabled;
    b->setLabel(SoundEnabled? "Music: On" : "Music: Off");
}

void buttonActionFullscreen(Button* b) {
    bFullscreen = !bFullscreen;
    b->setLabel(bFullscreen? "Fullscreen: On" : "Fullscreen: Off");
    ResetFullscreen();
}

void buttonActionPlayer1Name(Button *b) {
   if (OpenDialogbox(200,100,player1name))
        return; //must save if true
}

void buttonActionPlayer2Name(Button *b) {
   if (OpenDialogbox(200,100,player2name))
        return; //must save if true
}

void buttonActionHighscores(Button *b) {
    OpenScoresDisplay();
}

static void ChangeKeysMenu(long playernumber) {
    Menu km(&screen,false);
    Button_changekey bLeft(&keySettings[playernumber].left,"Left");
    Button_changekey bRight(&keySettings[playernumber].right,"Right");
    Button_changekey bUp(&keySettings[playernumber].up,"Up");
    Button_changekey bDown(&keySettings[playernumber].down,"Down");
    Button_changekey bPush(&keySettings[playernumber].push,"Push");
    Button_changekey bSwitch(&keySettings[playernumber].change,"Change");
    km.addButton(&bLeft);
    km.addButton(&bRight);
    km.addButton(&bUp);
    km.addButton(&bDown);
    km.addButton(&bPush);
    km.addButton(&bSwitch);
    km.run();
}

static void ChangeKeysMenu1(Button *b) {
    ChangeKeysMenu(0);
}

static void ChangeKeysMenu2(Button *b) {
    ChangeKeysMenu(2);
}

void ConfigureMenu(Button *b) {
    Menu cm(&screen,false);
    Button bMusic,bSound,buttonFullscreen,bPlayer1Name,bPlayer2Name;
    Button bPlayer1Keys, bPlayer2Keys;
    bMusic.setLabel(MusicEnabled? "Music: On" : "Music: Off");
    bMusic.setAction(buttonActionMusic);
    bSound.setLabel(SoundEnabled? "Music: On" : "Music: Off");
    bSound.setAction(buttonActionSound);
    buttonFullscreen.setLabel(bFullscreen? "Fullscreen: On" : "Fullscreen: Off");
    buttonFullscreen.setAction(buttonActionFullscreen);
    bPlayer1Name.setAction(buttonActionPlayer1Name);
    bPlayer1Name.setLabel("Change player 1's name");
    bPlayer2Name.setAction(buttonActionPlayer2Name);
    bPlayer2Name.setLabel("Change player 2's name");
    bPlayer1Keys.setAction(ChangeKeysMenu1);
    bPlayer1Keys.setLabel("Change player 1's keys");
    bPlayer2Keys.setAction(ChangeKeysMenu2);
    bPlayer2Keys.setLabel("Change player 2's keys");
    cm.addButton(&bMusic);
    cm.addButton(&bSound);
    cm.addButton(&buttonFullscreen);
    cm.addButton(&bPlayer1Name);
    cm.addButton(&bPlayer2Name);
    cm.addButton(&bPlayer1Keys);
    cm.addButton(&bPlayer2Keys);
    cm.run();
}

void MainMenu()
{
    InitMenues();
    Menu m(&screen,true);
    Button bHi,bTimetrial1, bPuzzle, bVs1, bConfigure,bHighscore;
    bHi.setLabel("Single player - endless");
    bHi.setAction(runSinglePlayerEndless);
    bTimetrial1.setLabel("Single player - time trial");
    bTimetrial1.setAction(runSinglePlayerTimeTrial);
    bPuzzle.setLabel("Single player - puzzle mode");
    bPuzzle.setAction(runSinglePlayerPuzzle);
    bVs1.setLabel("Single player - vs");
    bVs1.setAction(runSinglePlayerVs);
    bConfigure.setLabel("Configure");
    bConfigure.setAction(ConfigureMenu);
    bHighscore.setLabel("Highscores");
    bHighscore.setAction(buttonActionHighscores);
    m.addButton(&bHi);
    m.addButton(&bTimetrial1);
    m.addButton(&bPuzzle);
    m.addButton(&bVs1);
    m.addButton(&bConfigure);
    m.addButton(&bHighscore);
    m.run();
}
