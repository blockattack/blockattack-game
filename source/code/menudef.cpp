/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2013 Poul Sander

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
#include "global.hpp"
#include <iostream>
#include <stdlib.h>
#include "MenuSystem.h"
#include "common.h"
#include "HelpHowtoState.hpp"
#include "HelpGamepadState.hpp"
#include "HelpAboutState.hpp"
#include "ShowFileState.hpp"

using std::string;
using std::cerr;
using std::cout;
using std::vector;

#if 0
//Menu
static void PrintHi(Button* b) {
	cout << "Hi" <<"\n";
}
#endif

//Stores the controls
struct control {
	SDL_Keycode up;
	SDL_Keycode down;
	SDL_Keycode left;
	SDL_Keycode right;
	SDL_Keycode change;
	SDL_Keycode push;
};

void OpenScoresDisplay();
extern control keySettings[3];

//Function to return the name of a key, to be displayed...
string getKeyName(SDL_Keycode key) {
	string keyname(SDL_GetKeyName(key));
	if (key == SDLK_UP) {
		keyname = _("Up arrow");
	}
	if (key == SDLK_DOWN) {
		keyname = _("Down arrow");
	}
	if (key == SDLK_LEFT) {
		keyname = _("Left arrow");
	}
	if (key == SDLK_RIGHT) {
		keyname = _("Right arrow");
	}
	if (key == SDLK_RCTRL) {
		keyname = _("Right Ctrl");
	}
	if (key == SDLK_LCTRL) {
		keyname = _("Left Ctrl");
	}
	if (key == SDLK_RSHIFT) {
		keyname = _("Right shift");
	}
	if (key == SDLK_LSHIFT) {
		keyname = _("Left shift");
	}
	if (key == SDLK_RALT) {
		keyname = _("Right alt");
	}
	if (key == SDLK_LALT) {
		keyname = _("Left alt");
	}
	if (key == SDLK_RETURN) {
		keyname = _("Return");
	}
	if (key == SDLK_SPACE) {
		keyname = _("Space");
	}
	if (globalData.verboseLevel) {
		cout << key << " translated to " << keyname << "\n";
	}
	return keyname;
}

class Button_changekey : public Button {
private:
	SDL_Keycode* m_key2change;
	string m_keyname;
public:
	Button_changekey(SDL_Keycode* key, string keyname);
	void doAction();
};


Button_changekey::Button_changekey(SDL_Keycode* key, string keyname) {
	m_key2change = key;
	m_keyname = keyname;
	setLabel(m_keyname+" : "+getKeyName(*m_key2change));
}

void Button_changekey::doAction() {
	SDL_Event event;

	bool finnish = false;
	while (!finnish) {
		SDL_Delay(10);
		while ( SDL_PollEvent(&event) ) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym != SDLK_ESCAPE) {
					*m_key2change = event.key.keysym.sym;
				}
				finnish = true;
			}
		}
	}
	setLabel(m_keyname+" : "+getKeyName(*m_key2change));
}

void InitMenues() {
	standardButton.setSurfaces();
}

static void runSinglePlayerEndless0() {
	runGame(Gametype::SinglePlayerEndless, 0);
}

static void runSinglePlayerEndless1() {
	runGame(Gametype::SinglePlayerEndless, 1);
}

static void runSinglePlayerEndless2() {
	runGame(Gametype::SinglePlayerEndless, 2);
}

static void runSinglePlayerEndless3() {
	runGame(Gametype::SinglePlayerEndless, 3);
}

static void runSinglePlayerEndless4() {
	runGame(Gametype::SinglePlayerEndless, 4);
}

static void runSinglePlayerEndless() {
	Menu sp(globalData.screen,_("Single Player Endless"),true);
	Button d0,d1,d2,d3,d4;
	d0.setPopOnRun(true);
	d1.setPopOnRun(true);
	d2.setPopOnRun(true);
	d3.setPopOnRun(true);
	d4.setPopOnRun(true);
	d0.setLabel(_("Slow"));
	d1.setLabel(_("Fast"));
	d2.setLabel(_("Faster"));
	d3.setLabel(_("Even faster"));
	d4.setLabel(_("Fastest"));
	d0.setAction(runSinglePlayerEndless0);
	d1.setAction(runSinglePlayerEndless1);
	d2.setAction(runSinglePlayerEndless2);
	d3.setAction(runSinglePlayerEndless3);
	d4.setAction(runSinglePlayerEndless4);
	sp.addButton(&d0);
	sp.addButton(&d1);
	sp.addButton(&d2);
	sp.addButton(&d3);
	sp.addButton(&d4);
	RunGameState(sp);
}

static void runSinglePlayerTimeTrial() {
	runGame(Gametype::SinglePlayerTimeTrial, 0);
}

static void runStageClear() {
	runGame(Gametype::StageClear, 0);
}

static void runSinglePlayerPuzzle() {
	runGame(Gametype::Puzzle, 0);
}

class RunSinglePlayerVsButton : public Button {
	virtual void doAction() override {
		runGame(Gametype::SinglePlayerVs, iGeneric1);
	}
};

static void runTwoPlayerTimeTrial() {
	runGame(Gametype::TwoPlayerTimeTrial, 0);
}

static void runTwoPlayerVs() {
	runGame(Gametype::TwoPlayerVs, 0);
}

static void SetAlwaysSoftwareLabel(Button* b) {
	b->setLabel(Config::getInstance()->getInt("always-software")?
			_("Always use software render: On")
			: _("Always use software render: Off"));
}

static void SetMusicLabel (Button* b) {
	b->setLabel(globalData.MusicEnabled? _("Music: On") : _("Music: Off") );
}

static void SetSoundLabel (Button* b) {
	b->setLabel(globalData.SoundEnabled? _("Sound: On") : _("Sound: Off") );
}

static void SetFullscreenLabel (Button* b) {
	b->setLabel(globalData.bFullscreen? _("Fullscreen: On") : _("Fullscreen: Off") );
}

class AlwaysSoftwareRenderButton : public Button {
	virtual void doAction() override {
		Config::getInstance()->setInt("always-software", !Config::getInstance()->getInt("always-software"));
		SetAlwaysSoftwareLabel(this);
	}
};

class MusicButton : public Button {
	virtual void doAction() override {
		globalData.MusicEnabled = !globalData.MusicEnabled;
		SetMusicLabel(this);
	}
};

class SoundButton : public Button {
	virtual void doAction() override {
		globalData.SoundEnabled = !globalData.SoundEnabled;
		SetSoundLabel(this);
	}
};

class FullscreenButton : public Button {
	virtual void doAction() override {
		globalData.bFullscreen = !globalData.bFullscreen;
		SetFullscreenLabel(this);
		ResetFullscreen();
	}
};

static void buttonActionPlayer1Name() {
	if ( OpenDialogbox(200, 100, globalData.player1name, _("Enter player 1 name:")) ) {
		return;    //must save if true
	}
}

static void buttonActionPlayer2Name() {
	if ( OpenDialogbox(200, 100, globalData.player2name, _("Enter player 2 name:")) ) {
		return;    //must save if true
	}
}

static void buttonActionHighscores() {
	OpenScoresDisplay();
}

static void ChangeKeysMenu(long playernumber) {
	Menu km(globalData.screen,_("Change key bindings"),true);
	Button_changekey bLeft(&keySettings[playernumber].left,_("Left") );
	Button_changekey bRight(&keySettings[playernumber].right,_("Right") );
	Button_changekey bUp(&keySettings[playernumber].up,_("Up") );
	Button_changekey bDown(&keySettings[playernumber].down,_("Down") );
	Button_changekey bPush(&keySettings[playernumber].push,_("Push") );
	Button_changekey bSwitch(&keySettings[playernumber].change,_("Change") );
	km.addButton(&bLeft);
	km.addButton(&bRight);
	km.addButton(&bUp);
	km.addButton(&bDown);
	km.addButton(&bPush);
	km.addButton(&bSwitch);
	RunGameState(km);
}

static void ChangeKeysMenu1() {
	ChangeKeysMenu(0);
}

static void ChangeKeysMenu2() {
	ChangeKeysMenu(2);
}

static void ConfigureMenu() {
	Menu cm(globalData.screen,_("Configuration"),true);
	Button bPlayer1Name,bPlayer2Name;
	Button bPlayer1Keys, bPlayer2Keys;
	AlwaysSoftwareRenderButton bSoftware;
	MusicButton bMusic;
	SoundButton bSound;
	FullscreenButton buttonFullscreen;
	SetAlwaysSoftwareLabel(&bSoftware);
	SetMusicLabel(&bMusic);
	SetSoundLabel(&bSound);
	SetFullscreenLabel(&buttonFullscreen);
	bPlayer1Name.setAction(buttonActionPlayer1Name);
	bPlayer1Name.setLabel(_("Change player 1's name") );
	bPlayer2Name.setAction(buttonActionPlayer2Name);
	bPlayer2Name.setLabel(_("Change player 2's name") );
	bPlayer1Keys.setAction(ChangeKeysMenu1);
	bPlayer1Keys.setLabel(_("Change player 1's keys") );
	bPlayer2Keys.setAction(ChangeKeysMenu2);
	bPlayer2Keys.setLabel(_("Change player 2's keys") );
	cm.addButton(&bMusic);
	cm.addButton(&bSound);
	cm.addButton(&bSoftware);
	cm.addButton(&buttonFullscreen);
	cm.addButton(&bPlayer1Name);
	cm.addButton(&bPlayer2Name);
	cm.addButton(&bPlayer1Keys);
	cm.addButton(&bPlayer2Keys);
	RunGameState(cm);
}

static void SinglePlayerVsMenu() {
	Menu spvs(globalData.screen,_("Single player VS"),true);
	RunSinglePlayerVsButton d1,d2,d3,d4,d5,d6,d7;
	d1.setPopOnRun(true);
	d2.setPopOnRun(true);
	d3.setPopOnRun(true);
	d4.setPopOnRun(true);
	d5.setPopOnRun(true);
	d6.setPopOnRun(true);
	d7.setPopOnRun(true);
	d1.iGeneric1 = 0;
	d2.iGeneric1 = 1;
	d3.iGeneric1 = 2;
	d4.iGeneric1 = 3;
	d5.iGeneric1 = 4;
	d6.iGeneric1 = 5;
	d7.iGeneric1 = 6;
	d1.setLabel(_("Very easy"));
	d2.setLabel(_("Easy"));
	d3.setLabel(_("Below normal"));
	d4.setLabel(_("Normal"));
	d5.setLabel(_("Above normal"));
	d6.setLabel(_("Hard"));
	d7.setLabel(_("Hardest"));
	spvs.addButton(&d1);
	spvs.addButton(&d2);
	spvs.addButton(&d3);
	spvs.addButton(&d4);
	spvs.addButton(&d5);
	spvs.addButton(&d6);
	spvs.addButton(&d7);
	RunGameState(spvs);
}

static void MultiplayerMenu() {
	Menu mm(globalData.screen,_("Multiplayer"),true);
	Button bTT, bVs;
	bTT.setLabel(_("Two player - time trial"));
	bTT.setAction(runTwoPlayerTimeTrial);
	bVs.setLabel(_("Two player - vs"));
	bVs.setAction(runTwoPlayerVs);
	mm.addButton(&bTT);
	mm.addButton(&bVs);
	RunGameState(mm);
}

static void runHowto() {
	HelpHowtoState howto;
	RunGameState(howto);
}

static void runHelpGamepad() {
	HelpGamepadState helpGamepad;
	RunGameState(helpGamepad);
}

static void runHelpAbout() {
	HelpAboutState helpAbout;
	RunGameState(helpAbout);
}

static void runCredits() {
	ShowFileState creditsFile;
	creditsFile.SetData("misc/AUTHORS", _("Credits"));
	RunGameState(creditsFile);
}

static void HelpMenu() {
	Menu m(globalData.screen, _("Help"), true);
	Button bHowto;
	bHowto.setLabel(_("How to"));
	bHowto.setAction(runHowto);
	m.addButton(&bHowto);
	Button bGamepad;
	bGamepad.setLabel(_("Gamepad"));
	bGamepad.setAction(runHelpGamepad);
	m.addButton(&bGamepad);
	Button bCredits;
	bCredits.setLabel(_("Credits"));
	bCredits.setAction(runCredits);
	m.addButton(&bCredits);
	Button bAbout;
	bAbout.setLabel(_("About"));
	bAbout.setAction(runHelpAbout);
	m.addButton(&bAbout);
	RunGameState(m);
}

static void SinglePlayerMenu() {
	Menu m(globalData.screen, _("Single player"), true);
	Button bHi,bTimetrial1, bStageClear, bPuzzle, bVs1;
	bHi.setLabel(_("Single player - endless") );
	bHi.setAction(runSinglePlayerEndless);
	bTimetrial1.setLabel(_("Single player - time trial") );
	bTimetrial1.setAction(runSinglePlayerTimeTrial);
	bStageClear.setLabel(_("Single player - stage clear") );
	bStageClear.setAction(runStageClear);
	bPuzzle.setLabel(_("Single player - puzzle mode") );
	bPuzzle.setAction(runSinglePlayerPuzzle);
	bVs1.setLabel(_("Single player - vs") );
	bVs1.setAction(SinglePlayerVsMenu);
	m.addButton(&bHi);
	m.addButton(&bTimetrial1);
	m.addButton(&bStageClear);
	m.addButton(&bPuzzle);
	m.addButton(&bVs1);
	RunGameState(m);
}

void SafeModeMenu() {
	if (Config::getInstance()->getInt("always-software")) {
		return;
	}
	InitMenues();
	Menu safeMode(globalData.screen, _("Game did not shutdown as it should"), true);
	Button bOnce;
	Button bAlways;
	bOnce.setLabel(_("Use software renderer this time"));
	bAlways.setLabel(_("Always use software renderer"));
	bOnce.setPopOnRun(true);
	bAlways.setAction([]() {Config::getInstance()->setInt("always-software", 1); });
	bAlways.setPopOnRun(true);
	safeMode.addButton(&bOnce);
	safeMode.addButton(&bAlways);
	RunGameState(safeMode);
}

void MainMenu() {
	InitMenues();
	Menu m(globalData.screen,_("Block Attack - Rise of the blocks"),false);
	Button bHi, bMulti, bConfigure, bHighscore, bHelp;
	bHi.setLabel(_("Single player") );
	bHi.setAction(SinglePlayerMenu);
	bMulti.setLabel(_("Multi player") );
	bMulti.setAction(MultiplayerMenu);
	bConfigure.setLabel(_("Configure") );
	bConfigure.setAction(ConfigureMenu);
	bHighscore.setLabel(_("Highscores") );
	bHighscore.setAction(buttonActionHighscores);
	bHelp.setLabel(_("Help"));
	bHelp.setAction(HelpMenu);
	m.addButton(&bHi);
	m.addButton(&bMulti);
	m.addButton(&bConfigure);
	m.addButton(&bHighscore);
	m.addButton(&bHelp);
	RunGameState(m);
}
