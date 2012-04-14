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
    /**
     * Creates a nulled ImageHolder (without data)
     */
    CppSdlImageHolder();
    /**
     * Creates an image from a file
     * @param filename to be loaded
     */
    CppSdlImageHolder(std::string filename);
    /**
     * Creates a copy. The new copy shares raw data with the original but is otherwise independent.
     * @param orig
     */
    CppSdlImageHolder(const CppSdlImageHolder& orig);
    CppSdlImageHolder(char *rawdata, int datasize);
    virtual ~CppSdlImageHolder();
    /**
     * This gives access direct access to the internal data.
     * Careful, the data might be shared between multiple ImageHolders, 
     * chaing one will change them all.
     * @return A pointer
     */
    SDL_Surface *GetRawDataInsecure();
    CppSdlImageHolder & operator=(const CppSdlImageHolder &rhs);
    void Cut(Uint32 x,Uint32 y,Sint32 w,Sint32 h);
    /**
     * The width of the image
     * 0 if the image is nulled
     * @return The width of the image in pizels
     */
    Uint32 GetWidth();
    /**
     * The height of the image
     * 0 if the image is nulled
     * @return  The height of the image
     */
    Uint32 GetHeight();
    /**
     * Draws an image to surface at a given position. 
     * If the image is nulled nothing is drawn
     * @param target Destination surface
     * @param x horizontal placement on destination surface
     * @param y vertical placement on destination surface
     */
    void PaintTo(SDL_Surface *target, int x, int y);
    void OptimizeForBlit(bool allowAlpha = true);
    /**
     * Tests if there are data in the object
     * @return true if where are data in the object
     */
    bool IsNull();
    /**
     * Removes the data in the object.
     * Once all ImageHolders pointing to the same data has been freed the internal data will be freed too
     */
    void MakeNull();
private:
    void Initialized(); //throws an exception if *data is null
    int *counter;
    SDL_Rect area;
    SDL_Surface *data;
};

}
#endif	/* _CPPSDLIMAGEHOLDER_HPP */

