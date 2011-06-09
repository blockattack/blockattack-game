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

bool OpenDialogbox(int x, int y, char *name);
void OpenScoresDisplay();

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

class ChangeKeysMenu {
public:
    static long player;
    static void CreateKeysMenu(long playernumber);
    static void ChangeLeftKey(Button *b);
    static void ChangeRightKey(Button *b);
    static void ChangeUpKey(Button *b);
    static void ChangeDownKey(Button *b);
    static void ChangePushKey(Button *b);
    static void ChangeSwitchKey(Button *b);
    static void ChangeJoypadStatus(Button *b);
    static void ChangeMouseStatus(Button *b);
};

long ChangeKeysMenu::player;

static void ChangeKeysMenu(long playernumber) {
    ChangeKeysMenu::player = playernumber;
    Button bLeft, bRight, bUp, bDown, bPush, bSwitch, bJoypad, bMouse;
    bLeft.setLabel("Left : ");
    bRight.setLabel("Right : ");
    bUp.setLabel("Up : ");
    bDown.setLabel("Down : ");
}

void ConfigureMenu(Button *b) {
    Menu cm(&screen,false);
    Button bMusic,bSound,buttonFullscreen,bPlayer1Name,bPlayer2Name;
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
    cm.addButton(bMusic);
    cm.addButton(bSound);
    cm.addButton(buttonFullscreen);
    cm.addButton(bPlayer1Name);
    cm.addButton(bPlayer2Name);
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
    m.addButton(bHi);
    m.addButton(bTimetrial1);
    m.addButton(bPuzzle);
    m.addButton(bVs1);
    m.addButton(bConfigure);
    m.addButton(bHighscore);
    m.run();
}