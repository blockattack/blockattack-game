/* 
 * File:   CppSdlImageHolder.cpp
 * Author: poul
 * 
 * Created on 29. september 2010, 19:46
 */

#include "CppSdlImageHolder.hpp"
#include "SDL_image.h"

namespace CppSdl {

CppSdlImageHolder::CppSdlImageHolder(std::string filename) {
    data = IMG_Load(filename.c_str());
    if(!data)
    {
        //Here we should throw an exception
        CppSdlException e(IMAGE,CPPSDL_ERROR_MISSINGFILE,"Could not read file \""+filename+"\"");
        throw e;
    }
    SDL_GetClipRect(data,&area);

    counter=1;
}

CppSdlImageHolder::CppSdlImageHolder(const CppSdlImageHolder& orig) {
    //Just take the data from the original. This is technically wrong but adds a little performance.
    data = orig.data;
    area = orig.area;
    counter++;
}

CppSdlImageHolder::CppSdlImageHolder(char* rawdata, int datasize) {
    SDL_RWops *rw = SDL_RWFromMem (rawdata, datasize);

    //The above might fail an return null.
    if(!rw)
    {
        CppSdlException e(IMAGE,CPPSDL_ERROR_NULLPOINTER, "Could not read raw data");
        throw e;
    }

    SDL_Surface* data = IMG_Load_RW(rw,false); //the second argument tells the function to three RWops

    if(!data)
    {
        CppSdlException e(IMAGE,CPPSDL_ERROR_DATA,"Could not read raw data");
        throw e;
    }
    
    SDL_GetClipRect(data,&area);
    counter = 1;
}

CppSdlImageHolder::~CppSdlImageHolder() {
    counter--;
    if(counter == 0)
    {
        SDL_FreeSurface(data);
        data = NULL;
    }
}

SDL_Surface* CppSdlImageHolder::GetRawDataInsecure()
{
    return data;
}

Uint32 CppSdlImageHolder::GetWidth()
{
    return area.w;
}

Uint32 CppSdlImageHolder::GetHeight()
{
    return area.h;
}

void CppSdlImageHolder::Cut(Uint32 x, Uint32 y, Sint32 w = -1, Sint32 h = -1)
{
    if(w<0)
    {
        w = GetWidth() - x;
    }
    if(h<0)
    {
        h = GetHeight() - y;
    }
    if(x+w>GetWidth())
    {
        //throw exception
    }
    if(y+h>GetHeight())
    {
        //throw exception
    }
    if(x<0 || x>GetWidth())
    {
        //throw exception
    }
    if(y<0 || y>GetHeight())
    {
        //throw exception
    }
    area.x += x;
    area.y += y;
    area.w = w;
    area.h = h;
}

}