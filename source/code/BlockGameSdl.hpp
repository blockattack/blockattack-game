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
https://blockattack.net
===========================================================================
*/

#include "BlockGame.hpp"
#include "global.hpp"
#include "sago/SagoTextField.hpp"
#include "BallManager.hpp"
#include "puzzlehandler.hpp"

extern BallManager theBallManager;

extern ExplosionManager theExplosionManager;


static void setScoreboardFont(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("penguinattack");
	field.SetColor({255,255,255,255});
	field.SetFontSize(20);
	field.SetText(text);
}

static void setButtonFont(const sago::SagoDataHolder* holder, sago::SagoTextField& field, const char* text) {
	field.SetHolder(holder);
	field.SetFont("freeserif");
	field.SetColor({255,255,255,255});
	field.SetFontSize(24);
	field.SetOutline(1, {64,64,64,255});
	field.SetText(text);
}

template <class T> void sagoTextSetHelpFont(T& field) {
	field.SetHolder(&globalData.spriteHolder->GetDataHolder());
	field.SetFont("freeserif");
	field.SetFontSize(30);
	field.SetOutline(1, {128,128,128,255});
}

class BlockGameSdl : public BlockGame {
public:
	BlockGameSdl(int tx, int ty, const sago::SagoDataHolder* holder) {
		topx = tx;
		topy = ty;
		setScoreboardFont(holder, scoreLabel, _("Score:"));
		setScoreboardFont(holder, timeLabel, _("Time:"));
		setScoreboardFont(holder, chainLabel, _("Chain:"));
		setScoreboardFont(holder, speedLabel, _("Speed:"));
		setButtonFont(holder, buttonNext, _("Next"));
		setButtonFont(holder, buttonRetry, _("Retry"));
		setButtonFont(holder, buttonSkip, _("Skip"));
		setButtonFont(holder, stopIntField, "");

		sagoTextSetBlueFont(player_name);
		sagoTextSetBlueFont(player_time);
		sagoTextSetBlueFont(player_score);
		sagoTextSetBlueFont(player_chain);
		sagoTextSetBlueFont(player_speed);
	}


	void DrawImgBoard(const sago::SagoSprite& img, int x, int y) const {
		DrawIMG(img, globalData.screen, x+topx, y+topy);
	}

	void DrawImgBoardBounded(const sago::SagoSprite& img, int x, int y) const {
		DrawIMG_Bounded(img, globalData.screen, x+topx, y+topy, topx, topy, topx + BOARD_WIDTH, topy + BOARD_HEIGHT);
	}

	void PrintTextCenteredBoard(int x, int y, sago::SagoTextField& field) {
		field.Draw(globalData.screen, x+topx+60, y+topy+20,
		           sago::SagoTextField::Alignment::center, sago::SagoTextField::VerticalAlignment::center);
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
		x = (mousex-topx) / bsize;
		y = (bsize*12+topy-mousey-pixels) / bsize;
	}

	bool IsInTheBoard(int mousex, int mousey) const {
		if (mousex < topx || mousex > topx+bsize*6 || mousey < topy || mousey > topy+bsize*12) {
			return false;
		}
		return true;
	}

	bool IsUnderTheBoard(int mousex, int mousey) const {
		if (mousex < topx || mousex > topx+bsize*6 || mousey < topy+bsize*12) {
			return false;
		}
		return true;
	}

	void AddText(int x, int y, unsigned int text, int time) const override {
		globalData.theTextManager.addText(topx-10+x*bsize, topy+12*bsize-y*bsize, text, time);
	}

	void AddBall(int x, int y, bool right, int color) const  override {
		theBallManager.addBall(topx+40+x*bsize, topy+bsize*12-y*bsize, right, color);
	}

	void AddExplosion(int x, int y) const  override {
		theExplosionManager.addExplosion(topx-10+x*bsize, topy+bsize*12-10-y*bsize);
	}

	void PlayerWonEvent() const  override {
		if (!globalData.SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, globalData.applause.get(), 0);
	}

	void DrawEvent() const override {
		Mix_HaltChannel(1);
	}

	void BlockPopEvent() const  override {
		if (!globalData.SoundEnabled) {
			return;
		}
		Mix_PlayChannel(0, globalData.boing.get(), 0);
	}

	void LongChainDoneEvent() const  override {
		if (!globalData.SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, globalData.applause.get(), 0);
	}

	void TimeTrialEndEvent() const  override {
		if (!globalData.NoSound && globalData.SoundEnabled) {
			Mix_PlayChannel(1,globalData.counterFinalChunk.get(),0);
		}
	}

	void EndlessHighscoreEvent() const  override {
		if (!globalData.SoundEnabled) {
			return;
		}
		Mix_PlayChannel(1, globalData.applause.get(), 0);
	}
private:

	/**
	This crops "dstrect" and "srcrect" to fit inside "bounds"
	dstrect and srcrect are modified in place.
	*/
	void CropTexture(SDL_Rect& dstrect, SDL_Rect& srcrect, const SDL_Rect& bounds) const {
		if (dstrect.x > bounds.x+bounds.w) {
			dstrect = {0,0,0,0};
			return;
		}
		if (dstrect.y > bounds.y+bounds.h) {
			dstrect = {0,0,0,0};
			return;
		}
		if (dstrect.x+dstrect.w < bounds.x) {
			dstrect = {0,0,0,0};
			return;
		}
		if (dstrect.y+dstrect.h < bounds.y) {
			dstrect = {0,0,0,0};
			return;
		}
		if (dstrect.x < bounds.x) {
			Sint16 absDiff = bounds.x-dstrect.x;
			dstrect.x+=absDiff;
			srcrect.x+=absDiff;
			dstrect.w-=absDiff;
			srcrect.w-=absDiff;
		}
		if (dstrect.y < bounds.y) {
			Sint16 absDiff = bounds.y-dstrect.y;
			dstrect.y+=absDiff;
			srcrect.y+=absDiff;
			dstrect.h-=absDiff;
			srcrect.h-=absDiff;
		}
		if (dstrect.x+dstrect.w > bounds.x+bounds.w) {
			Sint16 absDiff = dstrect.x+dstrect.w-(bounds.x+bounds.w);
			dstrect.w -= absDiff;
			srcrect.w -= absDiff;
		}
		if (dstrect.y+dstrect.h > bounds.y+bounds.h) {
			Sint16 absDiff = dstrect.y+dstrect.h-(bounds.y+bounds.h);
			dstrect.h -= absDiff;
			srcrect.h -= absDiff;
		}
	}

	/**
	 * Creates a new texture for a garabge block of size size_x*size_y at x,y and crops it.
	 * Also sets globalData.screen to point to the texture, so Draw now draws on the Texture.
	*/
	SDL_Texture* CreateGarbageTexture(int x, int y, int size_x, int size_y, SDL_Rect& dstrect, SDL_Rect& srcrect) const {
		dstrect = { x, y, size_x*bsize, size_y*bsize };
		srcrect = { 0, 0, size_x*bsize, size_y*bsize };
		SDL_Rect bound = {topx, topy, BOARD_WIDTH, BOARD_HEIGHT};
		CropTexture (dstrect, srcrect, bound);
		SDL_Texture* mTexture = SDL_CreateTexture( globalData.screen, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size_x*bsize, size_y*bsize );
		SDL_SetRenderTarget( globalData.screen, mTexture );

		SDL_SetRenderDrawBlendMode(globalData.screen, SDL_BLENDMODE_NONE);
		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(globalData.screen, 0, 0, 0, 0);
		SDL_RenderFillRect(globalData.screen, NULL);
		SDL_SetRenderDrawBlendMode(globalData.screen, SDL_BLENDMODE_BLEND);
		return mTexture;
	}

	/**
	Paints a garbage block of size size_x*size_y (size in blocks) at x,y (coordinates in pixels).
	The garbage block will be painted on an internal texture and rendered to screen to protect against tearing.
	The result will be cropped to the playable area.
	*/
	void PaintGarbageBlock(int x, int y, int size_x, int size_y) const {
		SDL_Rect dstrect ;
		SDL_Rect srcrect ;
		SDL_Texture* mTexture = CreateGarbageTexture(x, y, size_x, size_y, dstrect, srcrect);
		const Sint32 draw_time = SDL_GetTicks();

		if (size_y == 1) {
			//single line
			globalData.garbageML.Draw(globalData.screen, draw_time, 0, 0);
			for (int i = 1; i < size_x-1; ++i) {
				globalData.garbageM.Draw(globalData.screen, draw_time, i*bsize, 0);
			}
			globalData.garbageMR.Draw(globalData.screen, draw_time, (size_x-1)*bsize, 0);
		}
		if (size_y > 1) {
			// Top line
			globalData.garbageTL.Draw(globalData.screen, draw_time, 0, 0);
			for (int i = 1; i < size_x-1; ++i) {
				globalData.garbageT.Draw(globalData.screen, draw_time, i*bsize, 0);
			}
			globalData.garbageTR.Draw(globalData.screen, draw_time, (size_x-1)*bsize, 0);
			// Middle lines (if any)
			for (int j=1; j < size_y-1; ++j) {
				globalData.garbageL.Draw(globalData.screen, draw_time, 0, j*bsize);
				for (int i = 1; i < size_x-1; ++i) {
					globalData.garbageFill.Draw(globalData.screen, draw_time, i*bsize, j*bsize);
				}
				globalData.garbageR.Draw(globalData.screen, draw_time, (size_x-1)*bsize, j*bsize);
			}
			//Buttom line
			globalData.garbageBL.Draw(globalData.screen, draw_time, 0, (size_y-1)*bsize);
			for (int i = 1; i < size_x-1; ++i) {
				globalData.garbageB.Draw(globalData.screen, draw_time, i*bsize, (size_y-1)*bsize);
			}
			globalData.garbageBR.Draw(globalData.screen, draw_time, (size_x-1)*bsize, (size_y-1)*bsize);
		}

		SDL_SetRenderTarget( globalData.screen, nullptr );
		SDL_RenderCopy( globalData.screen, mTexture, &srcrect, &dstrect );
		SDL_DestroyTexture(mTexture);
	}

	/**
	Paints a grey garbage block of size size_x (size in blocks, always 6) at x,y (coordinates in pixels).
	The result will be cropped to the playable area.
	*/
	void PaintGreyGarbageBlock(int x, int y, int size_x) const {
		const int size_y = bsize;
		SDL_Rect dstrect ;
		SDL_Rect srcrect ;
		SDL_Texture* mTexture = CreateGarbageTexture(x, y, size_x, size_y, dstrect, srcrect);
		const Sint32 draw_time = SDL_GetTicks();

		for (int j=0; j < size_y; ++j) {
			if (j==0) {
				globalData.garbageGML.Draw(globalData.screen, draw_time, j*bsize, 0);
			}
			else if (j==5) {
				globalData.garbageGMR.Draw(globalData.screen, draw_time, j*bsize, 0);
			}
			else {
				globalData.garbageGM.Draw(globalData.screen, draw_time, j*bsize, 0);
			}
		}

		SDL_SetRenderTarget( globalData.screen, nullptr );
		SDL_RenderCopy( globalData.screen, mTexture, &srcrect, &dstrect );
		SDL_DestroyTexture(mTexture);
	}

	std::pair<int, int> getGarbageSize(int topx, int topy) const {
		int size_x = 0;
		int size_y = 0;
		int number = board[topx][topy];
		for (int i=topy; i < 30 ; ++i) {
			if (number == board[topx][i]) {
				++size_y;
			}
		}
		for (int i=topx; i < 7 ; ++i) {
			if (number == board[i][topy]) {
				++size_x;
			}
		}
		return std::make_pair(size_x, size_y);
	}

	//Draws all the bricks to the board (including garbage)
	void PaintBricks() const {
		int lastGarbageNumber = 0;
		for (int i=0; i<13; ++i) {
			for (int j=0; j<6; ++j) {
				int basicBrick = board[j][i]%10; //The basic brick, stored on the least significant digit
				if ((basicBrick > -1) && (basicBrick < 7) && ((board[j][i]/1000000)%10==0)) {
					DrawImgBoardBounded(globalData.bricks[basicBrick],  j*bsize, bsize*12-i*bsize-pixels);
					if ((board[j][i]/BLOCKWAIT)%10==1) {
						DrawImgBoard(globalData.bomb,  j*bsize, bsize*12-i*bsize-pixels);
					}
					if ((board[j][i]/BLOCKHANG)%10==1) {
						DrawImgBoardBounded(globalData.ready,  j*bsize, bsize*12-i*bsize-pixels);
					}

				}
				if ((board[j][i]/1000000)%10==1) {
					int number = board[j][i];
					if (number != lastGarbageNumber) {
						const auto& garbage_size = getGarbageSize(j, i);
						lastGarbageNumber = number;
						PaintGarbageBlock(topx+j*bsize, topy+12*bsize-i*bsize-(garbage_size.second-1)*bsize -pixels, garbage_size.first, garbage_size.second);
					}
				}
				if ((board[j][i]/1000000)%10==2) {
					int number = board[j][i];
					if (number != lastGarbageNumber) {
						const auto& garbage_size = getGarbageSize(j, i);
						lastGarbageNumber = number;
						PaintGreyGarbageBlock(topx+j*bsize, topy+12*bsize-i*bsize-(garbage_size.second-1)*bsize -pixels, garbage_size.first);
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
				left = -1;
				right = board[j+1][i];
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
					if (globalData.theme.decoration.decoration_sprites.size()) {
						const sago::SagoSprite& sprite = globalData.spriteHolder->GetSprite(globalData.theme.decoration.decoration_sprites[board[j][i]%globalData.theme.decoration.decoration_sprites.size()]);
						DrawImgBoardBounded(sprite,  2*bsize, 12*bsize-i*bsize-pixels+(bsize/2)*garbageSize);
					}
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
				DrawImgBoardBounded(globalData.transCover,  i*bsize, 12*bsize-pixels);    //Make the appering blocks transperant
			}
		}

	}
public:
	//Draws everything
	void DoPaintJob() {
		DrawIMG(globalData.boardBackBack,globalData.screen,this->GetTopX()-60,this->GetTopY()-68);

		this->scoreLabel.Draw(globalData.screen, this->GetTopX()+310,this->GetTopY()-68+148);
		this->timeLabel.Draw(globalData.screen, this->GetTopX()+310,this->GetTopY()-68+197);
		this->chainLabel.Draw(globalData.screen, this->GetTopX()+310,this->GetTopY()-68+246);
		this->speedLabel.Draw(globalData.screen, this->GetTopX()+310,this->GetTopY()-68+295);
		globalData.spriteHolder->GetSprite(globalData.theme.back_board).DrawScaled(globalData.screen, SDL_GetTicks(), this->GetTopX(),this->GetTopY(), BOARD_WIDTH, BOARD_HEIGHT);

		PaintBricks();
		if (stageClear) {
			DrawImgBoard(globalData.blackLine,  0, bsize*(12+2)+bsize*(stageClearLimit-linesCleared)-pixels-1);
		}
		if (puzzleMode&&(!bGameOver)) {
			//We need to write nr. of moves left!
			strHolder = _("Moves left: ") + std::to_string(MovesLeft);
			static sago::SagoTextField movesPuzzleLabel;
			sagoTextSetHelpFont(movesPuzzleLabel);
			movesPuzzleLabel.SetText(strHolder);
			movesPuzzleLabel.Draw(globalData.screen, topx+5, topy+5);

		}
		if (puzzleMode && stageButtonStatus == SBpuzzleMode) {
			DrawImgBoard(globalData.bRetry, globalData.cordRetryButton.x, globalData.cordRetryButton.y);
			PrintTextCenteredBoard(globalData.cordRetryButton.x, globalData.cordRetryButton.y, buttonRetry);
			if (getLevel()<PuzzleGetNumberOfPuzzles()-1) {
				if (hasWonTheGame) {
					DrawImgBoard(globalData.bNext, globalData.cordNextButton.x, globalData.cordNextButton.y);
					PrintTextCenteredBoard(globalData.cordNextButton.x, globalData.cordNextButton.y, buttonNext);
				}
				else {
					DrawImgBoard(globalData.bSkip, globalData.cordNextButton.x, globalData.cordNextButton.y);
					PrintTextCenteredBoard(globalData.cordNextButton.x, globalData.cordNextButton.y, buttonSkip);
				}
			}
			else {
				static sago::SagoTextField lastPuzzleLabel;
				sagoTextSetHelpFont(lastPuzzleLabel);
				lastPuzzleLabel.SetText(_("Last puzzle"));
				lastPuzzleLabel.Draw(globalData.screen, topx+5, topy+5);
			}
		}
		if (stageClear && stageButtonStatus == SBstageClear) {
			DrawImgBoard(globalData.bRetry, globalData.cordRetryButton.x, globalData.cordRetryButton.y);
			PrintTextCenteredBoard(globalData.cordRetryButton.x, globalData.cordRetryButton.y, buttonRetry);
			if (getLevel()<50-1) {
				if (hasWonTheGame) {
					DrawImgBoard(globalData.bNext, globalData.cordNextButton.x, globalData.cordNextButton.y);
					PrintTextCenteredBoard(globalData.cordNextButton.x, globalData.cordNextButton.y, buttonNext);
				}
				else {
					DrawImgBoard(globalData.bSkip, globalData.cordNextButton.x, globalData.cordNextButton.y);
					PrintTextCenteredBoard(globalData.cordNextButton.x, globalData.cordNextButton.y, buttonSkip);
				}
			}
			else {
				static sago::SagoTextField lastStageLabel;
				sagoTextSetHelpFont(lastStageLabel);
				lastStageLabel.SetText(_("Last stage"));
				lastStageLabel.Draw(globalData.screen, topx+5, topy+5);
			}
		}
		if (!bGameOver && stop > 20) {
			stopIntField.SetText(std::to_string(stop/10));
			stopIntField.Draw(globalData.screen, 240+topx, -40+topy);
		}

#if DEBUG
		if (AI_Enabled&&(!bGameOver)) {
			sagoTextSetBlueFont(aiStatusField);
			strHolder = "AI_status: " + std::to_string(AIstatus)+ ", "+ std::to_string(AIlineToClear);
			aiStatusField.SetText(strHolder);
			aiStatusField.Draw(globalData.screen, topx+5, topy+5);
		}
#endif
		if (!bGameOver) {
			bool touched = false;
			int mx = 0;
			int my = 0;
			this->GetMouseCursor(touched, mx, my);
			if (touched) {
				DrawImgBoard(globalData.spriteHolder->GetSprite("touchcursor"),mx*bsize, 11*bsize-my*bsize-pixels);
			}
			else {
				DrawImgBoard(globalData.cursor,cursorx*bsize,11*bsize-cursory*bsize-pixels);
			}
		}
		if (ticks<gameStartedAt) {
			int currentCounter = abs((int)ticks-(int)gameStartedAt)/1000;
			if ( (currentCounter!=lastCounter) && (globalData.SoundEnabled)&&(!globalData.NoSound)) {
				Mix_PlayChannel(1, globalData.counterChunk.get(), 0);
			}
			lastCounter = currentCounter;
			switch (currentCounter) {
			case 2:
				DrawImgBoard(globalData.counter[2],  2*bsize, 5*bsize);
				break;
			case 1:
				DrawImgBoard(globalData.counter[1],  2*bsize, 5*bsize);
				break;
			case 0:
				DrawImgBoard(globalData.counter[0],  2*bsize, 5*bsize);
				break;
			default:
				break;
			}
		}
		else {
			if (globalData.SoundEnabled&&(!globalData.NoSound)&&(timetrial)&&(ticks>gameStartedAt+10000)&&(!bGameOver)) {
				int currentCounter = (ticks-(int)gameStartedAt)/1000;
				if (currentCounter!=lastCounter) {
					if (currentCounter>115 && currentCounter<120) {
						Mix_PlayChannel(1, globalData.counterChunk.get(), 0);
					}
				}
				lastCounter = currentCounter;
			}
			else {
				if ( (0==lastCounter) && (globalData.SoundEnabled)&&(!globalData.NoSound)) {
					Mix_PlayChannel(1, globalData.counterFinalChunk.get(), 0);
				}
				lastCounter = -1;
			}
		}

		if (bGameOver) {
			if (hasWonTheGame) {
				globalData.tbWinner.Draw(globalData.screen, topx+150, topy+200, sago::SagoTextField::Alignment::center);
			}
			else {
				if (bDraw) {
					globalData.tbDraw.Draw(globalData.screen, topx+150, topy+200, sago::SagoTextField::Alignment::center);
				}
				else {
					if (this->infostring.empty()) {
						globalData.tbGameOver.Draw(globalData.screen, topx+150, topy+200, sago::SagoTextField::Alignment::center);
					}
				}
			}
		}

		std::string strHolder;
		strHolder = std::to_string(this->GetScore()+this->GetHandicap());
		player_score.SetText(strHolder);
		player_score.Draw(globalData.screen, this->GetTopX()+310,this->GetTopY()+100);
		if (this->GetAIenabled()) {
			player_name.SetText(_("AI"));
		}
		else {
			player_name.SetText(name);
		}
		player_name.Draw(globalData.screen, this->GetTopX()+10,this->GetTopY()-34);
		if (this->isTimeTrial()) {
			int tid = (int)SDL_GetTicks()-this->GetGameStartedAt();
			int minutes;
			int seconds;
			if (tid>=0) {
				minutes = (2*60*1000-(abs((int)SDL_GetTicks()-(int)this->GetGameStartedAt())))/60/1000;
				seconds = ((2*60*1000-(abs((int)SDL_GetTicks()-(int)this->GetGameStartedAt())))%(60*1000))/1000;
			}
			else {
				minutes = ((abs((int)SDL_GetTicks()-(int)this->GetGameStartedAt())))/60/1000;
				seconds = (((abs((int)SDL_GetTicks()-(int)this->GetGameStartedAt())))%(60*1000))/1000;
			}
			if (this->isGameOver()) {
				minutes=0;
			}
			if (this->isGameOver()) {
				seconds=0;
			}
			if (seconds>9) {
				strHolder = std::to_string(minutes)+":"+std::to_string(seconds);
			}
			else {
				strHolder = std::to_string(minutes)+":0"+std::to_string(seconds);
			}
			player_time.SetText(strHolder);
		}
		else {
			int minutes = ((abs((int)SDL_GetTicks()-(int)this->GetGameStartedAt())))/60/1000;
			int seconds = (((abs((int)SDL_GetTicks()-(int)this->GetGameStartedAt())))%(60*1000))/1000;
			if (this->isGameOver()) {
				minutes=(this->GetGameEndedAt()/1000/60)%100;
			}
			if (this->isGameOver()) {
				seconds=(this->GetGameEndedAt()/1000)%60;
			}
			if (seconds>9) {
				strHolder = std::to_string(minutes)+":"+std::to_string(seconds);
			}
			else {
				strHolder = std::to_string(minutes)+":0"+std::to_string(seconds);
			}
			player_time.SetText(strHolder);
		}
		player_time.Draw(globalData.screen, this->GetTopX()+310,this->GetTopY()+150);
		strHolder = std::to_string(this->GetChains());
		player_chain.SetText(strHolder);
		player_chain.Draw(globalData.screen, this->GetTopX()+310,this->GetTopY()+200);
		//drawspeedLevel:
		strHolder = std::to_string(this->GetSpeedLevel());
		player_speed.SetText(strHolder);
		player_speed.Draw(globalData.screen, this->GetTopX()+310,this->GetTopY()+250);
		if ((this->isStageClear()) &&(this->GetTopY()+700+50*(this->GetStageClearLimit()-this->GetLinesCleared())-this->GetPixels()-1<600+this->GetTopY())) {
			oldBubleX = this->GetTopX()+280;
			oldBubleY = this->GetTopY()+650+50*(this->GetStageClearLimit()-this->GetLinesCleared())-this->GetPixels()-1;
			DrawIMG(globalData.stageBobble,globalData.screen,this->GetTopX()+280,this->GetTopY()+650+50*(this->GetStageClearLimit()-this->GetLinesCleared())-this->GetPixels()-1);
		}

		if (infostring.length() > 0) {
			static sago::SagoTextBox infoBox;
			static sago::SagoTextField objectiveField;
			static sago::SagoTextField gametypeNameField;
			sagoTextSetHelpFont(infoBox);
			infoBox.SetMaxWidth(290);
			infoBox.SetText(infostring);
			sagoTextSetHelpFont(objectiveField);
			objectiveField.SetText(_("Objective:"));
			sagoTextSetHelpFont(gametypeNameField);
			gametypeNameField.SetText(infostringName);
			gametypeNameField.Draw(globalData.screen, this->GetTopX()+7,this->GetTopY()+10);
			objectiveField.Draw(globalData.screen, this->GetTopX()+7, this->GetTopY()+160);
			infoBox.Draw(globalData.screen, this->GetTopX()+7, this->GetTopY()+160+32);

			int y = this->GetTopY()+400;
			static sago::SagoTextBox controldBox;
			controldBox.SetHolder(&globalData.spriteHolder->GetDataHolder());
			sagoTextSetHelpFont(controldBox);
			controldBox.SetMaxWidth(290);
			controldBox.SetText(controldBoxText);
			controldBox.Draw(globalData.screen, this->GetTopX()+7,y);
		}
	}

	sago::SagoTextField player_name;
	sago::SagoTextField player_time;
	sago::SagoTextField player_score;
	sago::SagoTextField player_chain;
	sago::SagoTextField player_speed;

	//Old Stage Clear Buble
	int oldBubleX;
	int oldBubleY;

	std::string infostring;
	std::string infostringName;
	std::string controldBoxText;


private:
	int topx, topy;
	sago::SagoTextField scoreLabel;
	sago::SagoTextField timeLabel;
	sago::SagoTextField chainLabel;
	sago::SagoTextField speedLabel;
	sago::SagoTextField buttonSkip;
	sago::SagoTextField buttonRetry;
	sago::SagoTextField buttonNext;
	sago::SagoTextField stopIntField;
#if DEBUG
	sago::SagoTextField aiStatusField;
#endif
};


