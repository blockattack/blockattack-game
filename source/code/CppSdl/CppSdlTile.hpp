/* 
 * File:   CppSdlTile.hpp
 * Author: poul
 *
 * Created on 7. december 2010, 19:55
 */

#ifndef _CPPSDLTILE_HPP
#define	_CPPSDLTILE_HPP

#include "CppSdlFrame.hpp"


namespace CppSdl {
enum CollideMode {OFF,BOX,PIXEL};

class CppSdlTile {
public:
    CppSdlTile();
    CppSdlTile(const CppSdlTile& orig);
    virtual ~CppSdlTile();

    void addFrame(CppSdlImageHolder image, long milliseconds);
    void addFrame(CppSdlFrame frame);
    void clearFrames();

    void draw();
    
    void setCollideMode(CollideMode _collideMode);
    CollideMode getCollideMode() const;
private:
    CollideMode _collideMode;
    std::vector<CppSdlFrame> _frames;
};
} //namespace

#endif	/* _CPPSDLTILE_HPP */

