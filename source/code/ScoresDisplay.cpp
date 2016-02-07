/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ScoresDisplay.cpp
 * Author: poul
 * 
 * Created on 7. februar 2016, 17:34
 */

#include "ScoresDisplay.hpp"
#include "global.hpp"
#include "common.h"
#include "stats.h"

using namespace std;

static void NFont_Write(SDL_Renderer* target, int x, int y, const char* text) {
	nf_standard_blue_font.draw(target, x, y, "%s", text);
}

const int numberOfPages = 3;

//Draws the highscores
void ScoresDisplay::DrawHighscores(int x, int y, bool endless) {
	backgroundImage.Draw(screen, 0, 0, 0);
	if (endless) {
		nf_standard_blue_font.draw(screen, x+100,y+100, "%s",_("Endless:") );
	}
	else {
		nf_standard_blue_font.draw(screen, x+100,y+100, "%s",_("Time Trial:") );
	}
	for (int i =0; i<10; i++) {
		char playerScore[32];
		char playerName[32];
		if (endless) {
			snprintf(playerScore, sizeof(playerScore), "%i", theTopScoresEndless.getScoreNumber(i));
		}
		else {
			snprintf(playerScore, sizeof(playerScore), "%i", theTopScoresTimeTrial.getScoreNumber(i));
		}
		if (endless) {
			snprintf(playerName, sizeof(playerName), "%s", theTopScoresEndless.getScoreName(i));
		}
		else {
			snprintf(playerName, sizeof(playerName), "%s", theTopScoresTimeTrial.getScoreName(i));
		}
		nf_standard_blue_font.draw(screen, x+420,y+150+i*35, "%s",playerScore);
		nf_standard_blue_font.draw(screen, x+60,y+150+i*35, "%s",playerName);
	}
}

void ScoresDisplay::DrawStats() {
	backgroundImage.Draw(screen, 0, 0, 0);
	int y = 5;
	const int y_spacing = 30;
	NFont_Write(screen, 10,y,_("Stats") );
	y+=y_spacing*2;
	NFont_Write(screen, 10,y,_("Chains") );
	for (int i=2; i<13; i++) {
		y+=y_spacing;
		NFont_Write(screen, 10,y,(itoa(i)+"X").c_str());
		string numberAsString = itoa(Stats::getInstance()->getNumberOf("chainX"+itoa(i)));
		NFont_Write(screen, 300,y,numberAsString.c_str());
	}
	y+=y_spacing*2;
	NFont_Write(screen, 10,y,_("Lines Pushed: ") );
	string numberAsString = itoa(Stats::getInstance()->getNumberOf("linesPushed"));
	NFont_Write(screen, 300,y,numberAsString.c_str());

	y+=y_spacing;
	NFont_Write(screen, 10,y, _("Puzzles solved: ") );
	numberAsString = itoa(Stats::getInstance()->getNumberOf("puzzlesSolved"));
	NFont_Write(screen, 300,y,numberAsString.c_str());

	y+=y_spacing*2;
	NFont_Write(screen, 10,y, _("Run time: ") );
	commonTime ct = TimeHandler::peekTime("totalTime",TimeHandler::ms2ct(SDL_GetTicks()));
	y+=y_spacing;
	NFont_Write(screen, 10, y, SPrintCF( _("Days: %i"), ct.days) );
	y+=y_spacing;
	NFont_Write(screen, 10, y, SPrintCF( _("Hours: %i"), ct.hours) );
	y+=y_spacing;
	NFont_Write(screen, 10, y, SPrintCF( _("Minutes: %i"), ct.minutes) );
	y+=y_spacing;
	NFont_Write(screen, 10, y, SPrintCF( _("Seconds: %i"), ct.seconds) );

	y-=y_spacing*4; //Four rows back
	const int x_offset3 = xsize/3+10; //Ofset for three rows
	NFont_Write(screen, x_offset3,y, _("Play time: ") );
	ct = TimeHandler::getTime("playTime");
	y+=y_spacing;
	NFont_Write(screen, x_offset3, y, SPrintCF( _("Days: %i"), ct.days) );
	y+=y_spacing;
	NFont_Write(screen, x_offset3, y, SPrintCF( _("Hours: %i"), ct.hours) );
	y+=y_spacing;
	NFont_Write(screen, x_offset3, y, SPrintCF( _("Minutes: %i"), ct.minutes) );
	y+=y_spacing;
	NFont_Write(screen, x_offset3, y, SPrintCF( _("Seconds: %i"), ct.seconds) );

	const int x_offset = xsize/2+10;
	y = 5+y_spacing*2;
	NFont_Write(screen, x_offset,y, _("VS CPU (win/loss)") );
	for (int i=0; i<7; i++) {
		y += y_spacing;
		NFont_Write(screen, x_offset,y,string("AI "+itoa(i+1)).c_str());
		numberAsString = itoa(Stats::getInstance()->getNumberOf("defeatedAI"+itoa(i)));
		string numberAsString2 = itoa(Stats::getInstance()->getNumberOf("defeatedByAI"+itoa(i)));
		string toPrint = numberAsString + "/" + numberAsString2;
		NFont_Write(screen, x_offset+230,y,toPrint.c_str());
	}
}

ScoresDisplay::ScoresDisplay() {
}

ScoresDisplay::~ScoresDisplay() {
}

bool ScoresDisplay::IsActive() {
	return isActive;
}

void ScoresDisplay::Draw(SDL_Renderer* target) {
	switch (page) {
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
	bHighScore.Draw(screen, 0, scoreX,scoreY);
	bBack.Draw(screen, 0, backX, backY);
	nf_button_font.draw(screen, backX+60,backY+10, NFont::CENTER ,_("Back"));
	bNext.Draw(screen, 0, nextX, nextY);
	nf_button_font.draw(screen, nextX+60,nextY+10, NFont::CENTER,_("Next"));

	//Draw page number
	string pageXofY = (boost::format(_("Page %1% of %2%") )%(page+1)%numberOfPages).str();
	NFont_Write(screen, xsize/2-nf_standard_blue_font.getWidth( "%s", pageXofY.c_str())/2,ysize-60,pageXofY.c_str());
}

void ScoresDisplay::ProcessInput(const SDL_Event& event, bool &processed) {
	if ( event.type == SDL_KEYDOWN ) {
		if ( (event.key.keysym.sym == SDLK_RIGHT)) {
			page++;
			if (page>=numberOfPages) {
				page = 0;
			}
		}
		else if ( (event.key.keysym.sym == SDLK_LEFT)) {
			page--;
			if (page<0) {
				page = numberOfPages-1;
			}
		}

		if ( (event.key.keysym.sym == SDLK_RETURN)||(event.key.keysym.sym == SDLK_KP_ENTER) ) {
			isActive = false;
		}
		else if ( (event.key.keysym.sym == SDLK_ESCAPE) ) {
			isActive = false;
		}
	}
}

void ScoresDisplay::Update() {
	int mousex, mousey;
	SDL_GetMouseState(&mousex,&mousey);
	
	// If the mouse button is released, make bMouseUp equal true
	if (!SDL_GetMouseState(nullptr, nullptr)&SDL_BUTTON(1)) {
		bMouseUp=true;
	}

	if (SDL_GetMouseState(nullptr,nullptr)&SDL_BUTTON(1) && bMouseUp) {
		bMouseUp = false;

		//The Score button:
		if ((mousex>scoreX) && (mousex<scoreX+buttonXsize) && (mousey>scoreY) && (mousey<scoreY+buttonYsize)) {
			isActive = false;
		}

		//The back button:
		if ((mousex>backX) && (mousex<backX+buttonXsize) && (mousey>backY) && (mousey<backY+buttonYsize)) {
			page--;
			if (page<0) {
				page = numberOfPages-1;
			}
		}

		//The next button:
		if ((mousex>nextX) && (mousex<nextX+buttonXsize) && (mousey>nextY) && (mousey<nextY+buttonYsize)) {
			page++;
			if (page>=numberOfPages) {
				page = 0;
			}
		}
	}
}