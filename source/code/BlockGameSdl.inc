/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2016 Poul Sander

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

#include "BlockGame.hpp"


class BlockGameSdl : public BlockGame {
public:
	BlockGameSdl(int tx, int ty) {
		topx = tx;
		topy = ty;
	}
	

	void DrawImgBoard(const sago::SagoSprite& img, int x, int y) const {
		DrawIMG(img, screen, x+topx, y+topy);
	}

	void DrawImgBoardBounded(const sago::SagoSprite& img, int x, int y) const {
		DrawIMG_Bounded(img, screen, x+topx, y+topy, topx, topy, topx + backBoard.GetWidth(), topy + backBoard.GetHeight());
	}

	void PrintTextCenteredBoard(int x, int y, const char* text) {
		nf_button_font.draw(screen, x+topx+60, y+topy+10, NFont::CENTER, "%s", text);
	}
	
	void PrintIntRightAlignedBoard(int x, int y, int number) {
		nf_button_font.draw(screen, x+topx+60, y+topy+10, NFont::RIGHT, "%d", number);
	}
	
	void SetTopXY(int tx, int ty) {
		topx = tx;
		topy = ty;
	}

	int GetTopX() const {
		return topx;
	}
	int GetTopY() const {
		return topy;
	}
	
	/**
	 * Retrives the coordinate to a given brick based on mouse input.
	 * @param match true if the coordinate are within borders
	 * @param mousex mouse coordinate input
	 * @param mousey mouse coordiante input
	 * @param x brick x. Unchanged if outside border
	 * @param y brick y. Unchanged if outside border
	 */
	void GetBrickCoordinateFromMouse(bool& match, int mousex, int mousey, int& x, int& y) const {
		if (!IsInTheBoard(mousex, mousey)) {
			match = false;
			return;
		}
		match = true;
		x = (mousex-topx) / 50;
		y = (50*12+topy-mousey-pixels) / 50;
	}
	
	bool IsInTheBoard(int mousex, int mousey) const {
		if (mousex < topx || mousex > topx+50*6 || mousey < topy || mousey > topy+50*12) {
			return false;
		}
		return true;
	}
	
	bool IsUnderTheBoard(int mousex, int mousey) const {
		if (mousex < topx || mousex > topx+50*6 || mousey < topy+50*12) {
			return false;
		}
		return true;
	}

	void AddText(int x, int y, const std::string& text, int time) const override {
		theTextManager.addText(topx-10+x*bsize, topy+12*bsize-y*bsize, text, time);
	}

	void AddBall(int x, int y, bool right, int color) const  override {
		theBallManager.addBall(topx+40+x*bsize, topy+bsize*12-y*bsize, right, color);
	}

	void AddExplosion(int x, int y) const  override {
		theExplosionManager.addExplosion(topx-10+x*bsize, topy+bsize*12-10-y*bsize);
	}

	void PlayerWonEvent() const  override {
		if (!SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, applause, 0);
	}

	void DrawEvent() const override {
		Mix_HaltChannel(1);
	}

	void BlockPopEvent() const  override {
		if (!SoundEnabled) {
			return;
		}
		Mix_PlayChannel(0, boing, 0);
	}

	void LongChainDoneEvent() const  override {
		if (!SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, applause, 0);
	}

	void TimeTrialEndEvent() const  override {
		if (!NoSound && SoundEnabled) {
			Mix_PlayChannel(1,counterFinalChunk,0);
		}
	}

	void EndlessHighscoreEvent() const  override {
		if (!SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, applause, 0);
	}
private:
	//Draws all the bricks to the board (including garbage)
	void PaintBricks() const {
		for (int i=0; ((i<13)&&(i<30)); i++) {
			for (int j=0; j<6; j++) {
				int basicBrick = board[j][i]%10; //The basic brick, stored on the least significant digit
				if ((basicBrick > -1) && (basicBrick < 7) && ((board[j][i]/1000000)%10==0)) {
					DrawImgBoardBounded(bricks[basicBrick],  j*bsize, bsize*12-i*bsize-pixels);
					if ((board[j][i]/BLOCKWAIT)%10==1) {
						DrawImgBoard(bomb,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((board[j][i]/BLOCKHANG)%10==1) {
						DrawImgBoardBounded(ready,  j*bsize, bsize*12-i*bsize-pixels);
					}

				}
				if ((board[j][i]/1000000)%10==1) {
					int left, right, over, under;
					int number = board[j][i];
					if (j<1) {
						left = -1;
					}
					else {
						left = board[j-1][i];
					}
					if (j>=5) {
						right = -1;
					}
					else {
						right = board[j+1][i];
					}
					if (i>28) {
						over = -1;
					}
					else {
						over = board[j][i+1];
					}
					if (i<1) {
						under = -1;
					}
					else {
						under = board[j][i-1];
					}
					if ((left == number)&&(right == number)&&(over == number)&&(under == number)) {
						DrawImgBoardBounded(garbageFill,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left != number)&&(right == number)&&(over == number)&&(under == number)) {
						DrawImgBoardBounded(garbageL,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right != number)&&(over == number)&&(under == number)) {
						DrawImgBoardBounded(garbageR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right == number)&&(over != number)&&(under == number)) {
						DrawImgBoardBounded(garbageT,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right == number)&&(over == number)&&(under != number)) {
						DrawImgBoardBounded(garbageB,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left != number)&&(right == number)&&(over != number)&&(under == number)) {
						DrawImgBoardBounded(garbageTL,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left != number)&&(right == number)&&(over == number)&&(under != number)) {
						DrawImgBoardBounded(garbageBL,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right != number)&&(over != number)&&(under == number)) {
						DrawImgBoardBounded(garbageTR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right != number)&&(over == number)&&(under != number)) {
						DrawImgBoardBounded(garbageBR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right != number)&&(over != number)&&(under != number)) {
						DrawImgBoardBounded(garbageMR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left == number)&&(right == number)&&(over != number)&&(under != number)) {
						DrawImgBoardBounded(garbageM,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((left != number)&&(right == number)&&(over != number)&&(under != number)) {
						DrawImgBoardBounded(garbageML,  j*bsize, bsize*12-i*bsize-pixels);
					}
				}
				if ((board[j][i]/1000000)%10==2) {
					if (j==0) {
						DrawImgBoardBounded(garbageGML,  j*bsize, bsize*12-i*bsize-pixels);
					}
					else if (j==5) {
						DrawImgBoardBounded(garbageGMR,  j*bsize, bsize*12-i*bsize-pixels);
					}
					else {
						DrawImgBoardBounded(garbageGM,  j*bsize, bsize*12-i*bsize-pixels);
					}
				}
			}
		}
		const int j = 0;

		int garbageSize=0;
		for (int i=0; i<20; i++) {
			if ((board[j][i]/1000000)%10==1) {
				int left, right, over, under;
				int number = board[j][i];
				if (j<1) {
					left = -1;
				}
				else {
					left = board[j-1][i];
				}
				if (j>5) {
					right = -1;
				}
				else {
					right = board[j+1][i];
				}
				if (i>28) {
					over = -1;
				}
				else {
					over = board[j][i+1];
				}
				if (i<1) {
					under = -1;
				}
				else {
					under = board[j][i-1];
				}
				if (((left != number)&&(right == number)&&(over != number)&&(under == number))&&(garbageSize>0)) {
					DrawImgBoardBounded(smiley[board[j][i]%4],  2*bsize, 12*bsize-i*bsize-pixels+(bsize/2)*garbageSize);
				}
				if (!((left != number)&&(right == number)&&(over == number)&&(under == number))) { //not in garbage
					garbageSize=0;
				}
				else {
					garbageSize++;
				}

			}
		}
		for (int i=0; i<6; i++) {
			if (board[i][0]!=-1) {
				DrawImgBoardBounded(transCover,  i*bsize, 12*bsize-pixels);    //Make the appering blocks transperant
			}
		}

	}
public:
	//Draws everything
	void DoPaintJob() {
		DrawIMG(boardBackBack,screen,this->GetTopX()-60,this->GetTopY()-68);

		nf_scoreboard_font.draw(screen, this->GetTopX()+310,this->GetTopY()-68+148,_("Score:") );
		nf_scoreboard_font.draw(screen, this->GetTopX()+310,this->GetTopY()-68+197,_("Time:") );
		nf_scoreboard_font.draw(screen, this->GetTopX()+310,this->GetTopY()-68+246,_("Chain:") );
		nf_scoreboard_font.draw(screen, this->GetTopX()+310,this->GetTopY()-68+295,_("Speed:") );
		DrawImgBoard(backBoard,  0, 0);

		PaintBricks();
		if (stageClear) {
			DrawImgBoard(blackLine,  0, bsize*(12+2)+bsize*(stageClearLimit-linesCleared)-pixels-1);
		}
		if (puzzleMode&&(!bGameOver)) {
			//We need to write nr. of moves left!
			strHolder = _("Moves left: ") + itoa(MovesLeft);
			nf_standard_blue_font.draw(screen, topx+5, topy+5, "%s",strHolder.c_str());

		}
		if (puzzleMode && stageButtonStatus == SBpuzzleMode) {
			DrawImgBoard(bRetry, cordRetryButton.x, cordRetryButton.y);
			PrintTextCenteredBoard(cordRetryButton.x, cordRetryButton.y, _("Retry"));
			if (getLevel()<PuzzleGetNumberOfPuzzles()-1) {
				if (hasWonTheGame) {
					DrawImgBoard(bNext,cordNextButton.x, cordNextButton.y);
					PrintTextCenteredBoard(cordNextButton.x, cordNextButton.y, _("Next"));
				}
				else {
					DrawImgBoard(bSkip,cordNextButton.x, cordNextButton.y);
					PrintTextCenteredBoard(cordNextButton.x, cordNextButton.y, _("Skip"));
				}
			}
			else {
				strHolder = "Last puzzle";
				nf_standard_blue_font.draw(screen, topx+5, topy+5, "%s",strHolder.c_str());
			}
		}
		if (stageClear && stageButtonStatus == SBstageClear) {
			DrawImgBoard(bRetry, cordRetryButton.x, cordRetryButton.y);
			PrintTextCenteredBoard(cordRetryButton.x, cordRetryButton.y, _("Retry"));
			if (getLevel()<50-1) {
				if (hasWonTheGame) {
					DrawImgBoard(bNext,cordNextButton.x, cordNextButton.y);
					PrintTextCenteredBoard(cordNextButton.x, cordNextButton.y, _("Next"));
				}
				else {
					DrawImgBoard(bSkip,cordNextButton.x, cordNextButton.y);
					PrintTextCenteredBoard(cordNextButton.x, cordNextButton.y, _("Skip"));
				}
			}
			else {
				strHolder = "Last stage";
				nf_standard_blue_font.draw(screen, topx+5, topy+5, "%s",strHolder.c_str());
			}
		}
		if (!bGameOver && stop > 20) {
			PrintIntRightAlignedBoard(240, -40, stop/10);
		}

#if DEBUG
		if (AI_Enabled&&(!bGameOver)) {
			strHolder = "AI_status: " + itoa(AIstatus)+ ", "+ itoa(AIlineToClear);
			//NFont_Write(   5, 5, strHolder.c_str());
			nf_standard_blue_font.draw(screen, topx+5, topy+5, "%s",strHolder.c_str());
		}
#endif
		if (!bGameOver) {
			bool touched = false;
			int mx = 0;
			int my = 0;
			this->GetMouseCursor(touched, mx, my);
			if (touched) {
				DrawImgBoard(spriteHolder->GetSprite("touchcursor"),mx*bsize, 11*bsize-my*bsize-pixels);
			}
			else {
				DrawImgBoard(cursor,cursorx*bsize-4,11*bsize-cursory*bsize-pixels-4);
			}
		}
		if (ticks<gameStartedAt) {
			int currentCounter = abs((int)ticks-(int)gameStartedAt)/1000;
			if ( (currentCounter!=lastCounter) && (SoundEnabled)&&(!NoSound)) {
				Mix_PlayChannel(1,counterChunk,0);
			}
			lastCounter = currentCounter;
			switch (currentCounter) {
			case 2:
				DrawImgBoard(counter[2],  2*bsize, 5*bsize);
				break;
			case 1:
				DrawImgBoard(counter[1],  2*bsize, 5*bsize);
				break;
			case 0:
				DrawImgBoard(counter[0],  2*bsize, 5*bsize);
				break;
			default:
				break;
			}
		}
		else {
			if (SoundEnabled&&(!NoSound)&&(timetrial)&&(ticks>gameStartedAt+10000)&&(!bGameOver)) {
				int currentCounter = (ticks-(int)gameStartedAt)/1000;
				if (currentCounter!=lastCounter) {
					if (currentCounter>115 && currentCounter<120) {
						Mix_PlayChannel(1,counterChunk,0);
					}
				}
				lastCounter = currentCounter;
			}
			else {
				if ( (0==lastCounter) && (SoundEnabled)&&(!NoSound)) {
					Mix_PlayChannel(1,counterFinalChunk,0);
				}
				lastCounter = -1;
			}
		}

		if ((bGameOver)&&(!editorMode)) {
			if (hasWonTheGame) {
				DrawImgBoard(iWinner,  0, 5*bsize);
			}
			else {
				if (bDraw) {
					DrawImgBoard(iDraw,  0, 5*bsize);
				}
				else {
					DrawImgBoard(iGameOver,  0, 5*bsize);
				}
			}
		}
	}

private:
	int topx, topy;
};


