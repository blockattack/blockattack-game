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

#ifndef BALLMANAGER_HPP
#define BALLMANAGER_HPP

#include <array>

extern GlobalData globalData;

//other ball constants:
const double gravity = 200.8; //acceleration
const double startVelocityY = 50.0;
const double VelocityX = 50.0;
const int ballSize = 16;
const double minVelocity = 200.0;

//The small things that are faaling when you clear something
class ABall {
private:
	double x = 0.0;
	double y = 0.0;
	double velocityY = 0.0;
	double velocityX = 0.0;
	int color = 0;
	unsigned long int lastTime = 0;
public:
	bool inUse = false;

	ABall() {
	}

	//constructor:
	ABall(int X, int Y, bool right, int coulor) {
		double tal = 1.0+((double)rand()/((double)RAND_MAX));
		velocityY = -tal*startVelocityY;
		lastTime = SDL_GetTicks();
		x = (double)X;
		y = (double)Y;
		color = coulor;
		if (right) {
			velocityX = tal*VelocityX;
		}
		else {
			velocityX = -tal*VelocityX;
		}
	}  //constructor

	void update() {
		double timePassed = (((double)(SDL_GetTicks()-lastTime))/1000.0);  //time passed in seconds
		x = x+timePassed*velocityX;
		y = y+timePassed*velocityY;
		velocityY = velocityY + gravity*timePassed;
		if (y<1.0) {
			velocityY=10.0;
		}
		if ((velocityY>minVelocity) && (y>(globalData.ysize-ballSize)) && (y<globalData.ysize)) {
			velocityY = -0.70*velocityY;
			y = globalData.ysize-ballSize;
		}
		lastTime = SDL_GetTicks();
	}

	int getX() {
		return (int)x;
	}

	int getY() {
		return (int)y;
	}

	int getColor() {
		return color;
	}
};  //aBall


class BallManager {
	static const int maxNumberOfBalls = 6*12*2*2;
public:
	std::array<ABall, maxNumberOfBalls> ballArray;

	BallManager() {
	}

	//Adds a ball to the screen at given coordiantes, traveling right or not with color
	int addBall(int x, int y,bool right,int color) {
		size_t ballNumber = 0;
		//Find a free ball
		while (ballNumber<ballArray.size() && ballArray[ballNumber].inUse) {
			ballNumber++;
		}
		//Could not find a free ball, return -1
		if (ballNumber==ballArray.size()) {
			return -1;
		}
		ballArray[ballNumber] = ABall(x,y,right,color);
		ballArray[ballNumber].inUse = true;
		return 1;
	}  //addBall

	void update() {
		for (size_t i = 0; i<ballArray.size(); i++) {

			if (ballArray[i].inUse) {
				ballArray[i].update();
				if (ballArray[i].getY()>globalData.ysize+100 || ballArray[i].getX()>globalData.xsize || ballArray[i].getX()<-ballSize) {
					ballArray[i].inUse = false;
				}
			}
		}
	} //update


}; //theBallManager

#endif /* BALLMANAGER_HPP */

