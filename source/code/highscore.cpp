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
http://blockattack.net
===========================================================================
*/

#include "highscore.h"
#include "os.hpp"
#include "physfs_stream.hpp"

using namespace std;

Highscore::Highscore(int type) {
	string filename1 = "endless.dat"; 
	string filename2 = "timetrial.dat"; 
	ourType = type;
	if (type == 1) {
		filename = filename1;
	}
	if (type == 2) {
		filename = filename2;
	}
	PhysFS::ifstream scorefile(filename.c_str());
	if (scorefile) {
		for (int i = 0; i<top; i++) {
			scorefile.read(tabel[i].name,30*sizeof(char));
			scorefile.read(reinterpret_cast<char*>(&tabel[i].score), sizeof(int));
		}
	}
	else {
		for (int i = 0; i<top; i++) {
			strcpy(tabel[i].name,"Poul Sander                  \0");
			tabel[i].score = 2000 - i*100;
		}
	}
	scorefile.close();
	writeFile();
}

void Highscore::writeFile() {
	string filename1 = "endless.dat";
	string filename2 = "timetrial.dat"; 
	if (ourType == 1) {
		filename = filename1;
	}
	if (ourType == 2) {
		filename = filename2;
	}

	PhysFS::ofstream outfile(filename.c_str(), ios::binary);
	if (!outfile) {
		cout << "Error writing to file: " << filename << endl;
		exit(1);
	}
	for (int i = 0; i<top; i++) {
		outfile.write(tabel[i].name,30*sizeof(char));
		outfile.write(reinterpret_cast<char*>(&tabel[i].score),sizeof(int));
	}
	outfile.close();
}

bool Highscore::isHighScore(int newScore) {
	if (newScore>tabel[top-1].score) {
		return true;
	}
	else {
		return false;
	}
}

void Highscore::addScore(const string& newName, int newScore) {
	int ranking = top-1;
	while ( (ranking != 0) && (tabel[ranking-1].score<newScore)) {
		ranking--;
	}
	for (int i=top-1; i>ranking; i--) {
		tabel[i].score = tabel[i-1].score;
		strcpy(tabel[i].name,tabel[i-1].name);
	}
	tabel[ranking].score = newScore;
	snprintf(tabel[ranking].name, sizeof(tabel[ranking].name), "%s", newName.c_str());
	Highscore::writeFile();
}

int Highscore::getScoreNumber(int room) {
	return tabel[room].score;
}

const char* Highscore::getScoreName(int room) {
	return &tabel[room].name[0];
}
