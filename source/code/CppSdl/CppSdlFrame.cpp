/* 
 * File:   CppSdlFrame.cpp
 * Author: poul
 * 
 * Created on 7. december 2010, 20:27
 */

#include "CppSdlFrame.hpp"

namespace CppSdl {
CppSdlFrame::CppSdlFrame() {
}

CppSdlFrame::CppSdlFrame(const CppSdlFrame& orig) {
}

CppSdlFrame::~CppSdlFrame() {
}
void CppSdlFrame::SetMilliseconds(long _milliseconds) {
    this->_milliseconds = _milliseconds;
}
long CppSdlFrame::GetMilliseconds() const {
    return _milliseconds;
}
void CppSdlFrame::SetImage(CppSdlImageHolder _image) {
    this->_image = _image;
}
CppSdlImageHolder CppSdlFrame::GetImage() const {
    return _image;
}

}//namespace
