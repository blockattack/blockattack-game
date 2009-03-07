/*
replay.cpp
Copyright (C) 2005 Poul Sander

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
    R�veh�jvej 36, V. 1111
    2800 Kgs. Lyngby
    DENMARK
    blockattack@poulsander.com
*/

/*
Handles replay
*/

#include "replay.h"

Replay::Replay()
{
    nrOfFrames=0;
    isLoaded = false;
}

Replay::Replay(const Replay& r)
{
    bps = r.bps;
    finalPack = r.finalPack;
    nrOfFrames = r.nrOfFrames;
    isLoaded = r.isLoaded;
    theResult = r.theResult;
}

Uint32 Replay::getNumberOfFrames()
{
    return nrOfFrames;
}

void Replay::setFrameSecTo(Uint32 miliseconds, boardPackage bp)
{
    if (isLoaded)
        return;
    int framesToSet = (miliseconds*FRAMESPERSEC)/1000;
    for (int i=nrOfFrames;i<framesToSet;i++)
    {
        bps.push_back(bp);
    }
    nrOfFrames=framesToSet;
}

void Replay::setFinalFrame(boardPackage bp,int theStatus)
{
    if (isLoaded)
        return;
    finalPack = bp;
    switch (theStatus)
    {
    case 1:
        theResult = winner;
        break;
    case 2:
        theResult = looser;
        break;
    case 3:
        theResult = draw;
        break;
    default:
        theResult = gameOver;
    };

}

//Returns the frame to the current time, if time too high the final frame is returned
boardPackage Replay::getFrameSec(Uint32 miliseconds)
{
    int frameToGet = (miliseconds*FRAMESPERSEC)/1000;
    if (!(frameToGet<nrOfFrames))
        return getFinalFrame();
    return bps.at(frameToGet);
}

bool Replay::isFinnished(Uint32 miliseconds)
{
    int frameToGet = (miliseconds*FRAMESPERSEC)/1000;
    if (!(frameToGet<nrOfFrames))
        return true;
    return false;
}

boardPackage Replay::getFinalFrame()
{
    return finalPack;
}

int Replay::getFinalStatus()
{
    return theResult;
}



bool Replay::saveReplay(string filename)
{
    //Saving as fileversion 3
    cout << "Saving as version 3 save file" << endl;
    ofstream saveFile;
    saveFile.open(filename.c_str(),ios::binary|ios::trunc);
    if (saveFile)
    {
        Uint32 version = 3;
        boardPackage bp;
        saveFile.write(reinterpret_cast<char*>(&version),sizeof(Uint32)); //Fileversion
        Uint8 nrOfReplays = 1;
        saveFile.write(reinterpret_cast<char*>(&nrOfReplays),sizeof(Uint8)); //nrOfReplaysIn File
        saveFile.write(reinterpret_cast<char*>(&nrOfFrames),sizeof(Uint32)); //Nr of frames in file
        for (int i=0; i<nrOfFrames && i<bps.size();i++)
        { //Writing frames
            bp = bps.at(i);
            saveFile.write(reinterpret_cast<char*>(&bp),sizeof(bp));
            
        }
        saveFile.write(reinterpret_cast<char*>(&finalPack),sizeof(finalPack));
        saveFile.close();
        return true;
    }
    else
    {
        return false;
    }
}

bool Replay::saveReplay(string filename,Replay p2)
{
    //Saving as fileversion 3
    cout << "Saving as version 3 save file (2 players)" << endl;
    ofstream saveFile;
    saveFile.open(filename.c_str(),ios::binary|ios::trunc);
    if (saveFile)
    {
        Uint32 version = 3;
        boardPackage bp;
        saveFile.write(reinterpret_cast<char*>(&version),sizeof(Uint32)); //Fileversion
        Uint8 nrOfReplays = 2;
        saveFile.write(reinterpret_cast<char*>(&nrOfReplays),sizeof(Uint8)); //nrOfReplaysIn File
        saveFile.write(reinterpret_cast<char*>(&nrOfFrames),sizeof(Uint32)); //Nr of frames in file
        for (int i=0; i<nrOfFrames && i<bps.size();i++)
        { //Writing frames
            bp = bps.at(i);
            saveFile.write(reinterpret_cast<char*>(&bp),sizeof(bp));
        }
        saveFile.write(reinterpret_cast<char*>(&finalPack),sizeof(finalPack));
        ///Player 2 starts here!!!!!!!!!!!!!!!!!!!!!!
        saveFile.write(reinterpret_cast<char*>(&p2.nrOfFrames),sizeof(Uint32)); //Nr of frames in file
        for (int i=0; (i<p2.nrOfFrames)&& i<p2.bps.size();i++)
        { //Writing frames
            bp = p2.bps.at(i);
            saveFile.write(reinterpret_cast<char*>(&bp),sizeof(bp));
        }
        saveFile.write(reinterpret_cast<char*>(&p2.finalPack),sizeof(finalPack));
        saveFile.close();
        return true;
    }
    else
    {
        return false;
    }
}

bool Replay::loadReplay(string filename)
{
    isLoaded = true;
    ifstream loadFile;
    loadFile.open(filename.c_str(),ios::binary);
    if (loadFile)
    {
        Uint32 version;
        loadFile.read(reinterpret_cast<char*>(&version),sizeof(Uint32));
        switch (version)
        {
        case 3:
            cout << "Loading a version 3 save game" << endl;
            Uint8 nrOfPlayers;
            boardPackage bp;
            loadFile.read(reinterpret_cast<char*>(&nrOfPlayers),sizeof(Uint8));
            loadFile.read(reinterpret_cast<char*>(&nrOfFrames),sizeof(Uint32));
            bps.clear();
            for (int i=0; (i<nrOfFrames);i++)
            {
                loadFile.read(reinterpret_cast<char*>(&bp),sizeof(bp));
                bps.push_back(bp);
            }
            loadFile.read(reinterpret_cast<char*>(&finalPack),sizeof(finalPack));
            break;
        default:
            cout << "Unknown version" << endl;
            return false;
        };
        loadFile.close();
        cout << "Loaded 1 player" << endl;
    }
    else
    {
        cout << "File not found or couldn't open: " << filename << endl;
        return false;
    }

}

bool Replay::loadReplay2(string filename)
{
    isLoaded = true;
    ifstream loadFile;
    loadFile.open(filename.c_str(),ios::binary);
    if (loadFile)
    {
        Uint32 version;
        loadFile.read(reinterpret_cast<char*>(&version),sizeof(Uint32));
        switch (version)
        {
        case 3:
            Uint8 nrOfPlayers;
            boardPackage bp;
            loadFile.read(reinterpret_cast<char*>(&nrOfPlayers),sizeof(Uint8));
            if (nrOfPlayers<2)
            {
                loadFile.close();
                cout << "Only one player in replay" << endl;
                return false;
            }
            cout << "loading player 2" << endl;
            loadFile.read(reinterpret_cast<char*>(&nrOfFrames),sizeof(Uint32));
            for (int i=0; (i<nrOfFrames);i++)
            {
                loadFile.read(reinterpret_cast<char*>(&bp),sizeof(bp));
                //bps.push_back(bp); We have already read player 1 with another function
            }
            loadFile.read(reinterpret_cast<char*>(&finalPack),sizeof(finalPack));
            loadFile.read(reinterpret_cast<char*>(&nrOfFrames),sizeof(Uint32));
            bps.reserve(nrOfFrames);
            for (int i=0; (i<nrOfFrames);i++)
            {
                loadFile.read(reinterpret_cast<char*>(&bp),sizeof(bp));
                bps.push_back(bp);
            }
            loadFile.read(reinterpret_cast<char*>(&finalPack),sizeof(finalPack));
            break;
        default:
            cout << "Unknown version: " << version << endl;
            return false;
        };
        loadFile.close();
    }
    else
    {
        cout << "File not found or couldn't open: " << filename << endl;
        return false;
    }

}
