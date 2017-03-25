/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2017 Poul Sander

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

#ifndef EXPLOSIONMANAGER_HPP
#define EXPLOSIONMANAGER_HPP

//a explosions, non moving
class AnExplosion {
private:
	int x = 0;
	int y = 0;
	Uint8 frameNumber = 0;
#define frameLength 80
	//How long an image in an animation should be showed
#define maxFrame 4
	//How many images there are in the animation
	unsigned long int placeTime = 0; //Then the explosion occored
public:
	bool inUse = false;

	AnExplosion() {
	}

	//constructor:
	AnExplosion(int X, int Y) {
		placeTime = SDL_GetTicks();
		x = X;
		y = Y;
		frameNumber=0;
	}  //constructor

	//true if animation has played and object should be removed from the screen
	bool removeMe() {
		frameNumber = (SDL_GetTicks()-placeTime)/frameLength;
		return (!(frameNumber<maxFrame));
	}

	int getX() {
		return (int)x;
	}

	int getY() {
		return (int)y;
	}

	int getFrame() {
		return frameNumber;
	}
};  //nExplosion

class ExplosionManager {
public:
	static const int maxNumberOfExplosions = 6*12*2*2;
	AnExplosion explosionArray[maxNumberOfExplosions];

	ExplosionManager() {
	}

	int addExplosion(int x, int y) {
		int explosionNumber = 0;
		while ( explosionNumber<maxNumberOfExplosions && explosionArray[explosionNumber].inUse) {
			explosionNumber++;
		}
		if (explosionNumber==maxNumberOfExplosions) {
			return -1;
		}
		explosionArray[explosionNumber] = AnExplosion(x,y);
		explosionArray[explosionNumber].inUse = true;
		return 1;
	}  //addBall

	void update() {
		for (int i = 0; i<maxNumberOfExplosions; i++) {

			if (explosionArray[i].inUse) {
				if (explosionArray[i].removeMe()) {
					explosionArray[i].inUse = false;
				}
			}
		}
	} //update


}; //explosionManager

#endif /* EXPLOSIONMANAGER_HPP */

