/*
ReadKeyboard.cpp
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

#include "ReadKeyboard.h"

ReadKeyboard::ReadKeyboard(void)
{
    length = 0;
    maxLength = 16;
    position = 0;
    strcpy(textstring,"                              ");
}

ReadKeyboard::~ReadKeyboard(void)
{
}

Uint8 ReadKeyboard::CharsBeforeCursor()
{
    return position;
}

ReadKeyboard::ReadKeyboard(char *oldName)
{
    length = 0;
    maxLength = 16;
    position = 0;
    strcpy(textstring,"                              ");
    strcpy(textstring,oldName);
    char charecter = textstring[maxLength+1];
    int i = maxLength+1;
    while ((charecter == ' ') && (i>0))
    {
        i--;
        charecter = textstring[i];
    }
    if (i>0)length = i+1;
    else
        if (charecter == ' ') length = 0;
        else length = 1;
    position = length;

}


void ReadKeyboard::putchar(char thing)
{
    if (length < maxLength)
    {
        for (int i = 28; i>position;i--)
        {
            textstring[i]=textstring[i-1];
        }
        textstring[position] = thing;
        length++;
        position++;
    }
}


void ReadKeyboard::removeChar()
{
    for (int i = position;i<28;i++)
    {
        textstring[i]=textstring[i+1];
    }
    textstring[28]=' ';
    if (length>0)length--;
}

bool ReadKeyboard::ReadKey(SDLKey keyPressed)
{
    if (keyPressed == SDLK_SPACE)
    {
        ReadKeyboard::putchar(' ');
        return true;
    }
    if (keyPressed == SDLK_DELETE)
    {
        if ((length>0)&& (position<length))ReadKeyboard::removeChar();
        return true;
    }
    if (keyPressed == SDLK_BACKSPACE)
    {
        if (position>0)
        {
            position--;
            ReadKeyboard::removeChar();
            return true;
        }
        return false;
    }
    Uint8* keys;
    keys = SDL_GetKeyState(NULL);
    if (keyPressed == SDLK_HOME)
    {
        position=0;
        return true;
    }
    if (keyPressed == SDLK_END)
    {
        position=length;
        return true;
    }
    if ((keyPressed == SDLK_LEFT) && (position>0))
    {
        position--;
        return true;
    }
    if ((keyPressed == SDLK_RIGHT) && (position<length))
    {
        position++;
        return true;
    }
    char charToPut;
    switch (keyPressed)
    {
    case SDLK_a:
        charToPut = 'a';
        break;
    case SDLK_b:
        charToPut = 'b';
        break;
    case SDLK_c:
        charToPut = 'c';
        break;
    case SDLK_d:
        charToPut = 'd';
        break;
    case SDLK_e:
        charToPut = 'e';
        break;
    case SDLK_f:
        charToPut = 'f';
        break;
    case SDLK_g:
        charToPut = 'g';
        break;
    case SDLK_h:
        charToPut = 'h';
        break;
    case SDLK_i:
        charToPut = 'i';
        break;
    case SDLK_j:
        charToPut = 'j';
        break;
    case SDLK_k:
        charToPut = 'k';
        break;
    case SDLK_l:
        charToPut = 'l';
        break;
    case SDLK_m:
        charToPut = 'm';
        break;
    case SDLK_n:
        charToPut = 'n';
        break;
    case SDLK_o:
        charToPut = 'o';
        break;
    case SDLK_p:
        charToPut = 'p';
        break;
    case SDLK_q:
        charToPut = 'q';
        break;
    case SDLK_r:
        charToPut = 'r';
        break;
    case SDLK_s:
        charToPut = 's';
        break;
    case SDLK_t:
        charToPut = 't';
        break;
    case SDLK_u:
        charToPut = 'u';
        break;
    case SDLK_v:
        charToPut = 'v';
        break;
    case SDLK_w:
        charToPut = 'w';
        break;
    case SDLK_x:
        charToPut = 'x';
        break;
    case SDLK_y:
        charToPut = 'y';
        break;
    case SDLK_z:
        charToPut = 'z';
        break;
    case SDLK_0:
        charToPut = '0';
        break;
    case SDLK_1:
        charToPut = '1';
        break;
    case SDLK_2:
        charToPut = '2';
        break;
    case SDLK_3:
        charToPut = '3';
        break;
    case SDLK_4:
        charToPut = '4';
        break;
    case SDLK_5:
        charToPut = '5';
        break;
    case SDLK_6:
        charToPut = '6';
        break;
    case SDLK_7:
        charToPut = '7';
        break;
    case SDLK_8:
        charToPut = '8';
        break;
    case SDLK_9:
        charToPut = '9';
        break;
    case SDLK_KP0:
        charToPut = '0';
        break;
    case SDLK_KP1:
        charToPut = '1';
        break;
    case SDLK_KP2:
        charToPut = '2';
        break;
    case SDLK_KP3:
        charToPut = '3';
        break;
    case SDLK_KP4:
        charToPut = '4';
        break;
    case SDLK_KP5:
        charToPut = '5';
        break;
    case SDLK_KP6:
        charToPut = '6';
        break;
    case SDLK_KP7:
        charToPut = '7';
        break;
    case SDLK_KP8:
        charToPut = '8';
        break;
    case SDLK_KP9:
        charToPut = '9';
        break;
    case SDLK_KP_PERIOD:
    case SDLK_PERIOD:
        charToPut='.';
        break;
    default:
        return false;
    }
    if ((keys[SDLK_LSHIFT]) || (keys[SDLK_RSHIFT]))
    {
        switch (charToPut)
        {
        case 'a':
            charToPut = 'A';
            break;
        case 'b':
            charToPut = 'B';
            break;
        case 'c':
            charToPut = 'C';
            break;
        case 'd':
            charToPut = 'D';
            break;
        case 'e':
            charToPut = 'E';
            break;
        case 'f':
            charToPut = 'F';
            break;
        case 'g':
            charToPut = 'G';
            break;
        case 'h':
            charToPut = 'H';
            break;
        case 'i':
            charToPut = 'I';
            break;
        case 'j':
            charToPut = 'J';
            break;
        case 'k':
            charToPut = 'K';
            break;
        case 'l':
            charToPut = 'L';
            break;
        case 'm':
            charToPut = 'M';
            break;
        case 'n':
            charToPut = 'N';
            break;
        case 'o':
            charToPut = 'O';
            break;
        case 'p':
            charToPut = 'P';
            break;
        case 'q':
            charToPut = 'Q';
            break;
        case 'r':
            charToPut = 'R';
            break;
        case 's':
            charToPut = 'S';
            break;
        case 't':
            charToPut = 'T';
            break;
        case 'u':
            charToPut = 'U';
            break;
        case 'v':
            charToPut = 'V';
            break;
        case 'w':
            charToPut = 'W';
            break;
        case 'x':
            charToPut = 'X';
            break;
        case 'y':
            charToPut = 'Y';
            break;
        case 'z':
            charToPut = 'Z';
            break;

        default:
            charToPut = charToPut;
        }
    }
    ReadKeyboard::putchar(charToPut);
    return true;
}

char* ReadKeyboard::GetString()
{
    textstring[29]='\0';
    return &textstring[0];
}
