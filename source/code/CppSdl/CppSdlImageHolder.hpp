/* 
 * File:   CppSdlImageHolder.hpp
 * Author: poul
 *
 * Created on 29. september 2010, 19:46
 */

#ifndef _CPPSDLIMAGEHOLDER_HPP
#define	_CPPSDLIMAGEHOLDER_HPP

#include "SDL.h"
#include <string>
#include "CppSdlException.hpp"

namespace CppSdl {

class CppSdlImageHolder {
public:
    CppSdlImageHolder();
    CppSdlImageHolder(std::string filename);
    CppSdlImageHolder(const CppSdlImageHolder& orig);
    CppSdlImageHolder(char *rawdata, int datasize);
    virtual ~CppSdlImageHolder();
    SDL_Surface *GetRawDataInsecure();
    CppSdlImageHolder & operator=(const CppSdlImageHolder &rhs);
    void Cut(Uint32 x,Uint32 y,Sint32 w,Sint32 h);
    Uint32 GetWidth();
    Uint32 GetHeight();
    void PaintTo(SDL_Surface *target, int x, int y);
    void OptimizeForBlit(bool allowAlpha = true);
    bool IsNull();
    void MakeNull();
private:
    void Initialized(); //throws an exception if *data is null
    int *counter;
    SDL_Rect area;
    SDL_Surface *data;
};

}
#endif	/* _CPPSDLIMAGEHOLDER_HPP */

