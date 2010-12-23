/* 
 * File:   CppSdlTile.cpp
 * Author: poul
 * 
 * Created on 7. december 2010, 19:55
 */

#include <vector>

#include "CppSdlTile.hpp"

namespace CppSdl {
CppSdlTile::CppSdlTile() {
    _collideMode = OFF;
    _frames.clear();
}

CppSdlTile::CppSdlTile(const CppSdlTile& orig) {
    _collideMode = orig->_collideMode;
    _frames = orig->_frames;
}

CppSdlTile::~CppSdlTile() {
}

void CppSdlTile::setCollideMode(CollideMode _collideMode) {
    this->_collideMode = _collideMode;
}

CollideMode CppSdlTile::getCollideMode() const {
    return _collideMode;
}

void CppSdlTile::addFrame(CppSdlFrame frame) {
    _frames.push_back(frame);
}

void CppSdlTile::addFrame(CppSdlImageHolder image, long milliseconds) {
    CppSdlFrame f;
    f.SetImage(image);
    f.SetMilliseconds(milliseconds);
    addFrame(f);
}

void CppSdlTile::clearFrames() {
    _frames.clear();
}


} //namespace