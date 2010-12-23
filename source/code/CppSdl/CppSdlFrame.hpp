/* 
 * File:   CppSdlFrame.hpp
 * Author: poul
 *
 * Created on 7. december 2010, 20:27
 */

#ifndef _CPPSDLFRAME_HPP
#define	_CPPSDLFRAME_HPP

#include "CppSdlImageHolder.hpp"


namespace CppSdl {
class CppSdlFrame {
public:
    CppSdlFrame();
    CppSdlFrame(const CppSdlFrame& orig);
    virtual ~CppSdlFrame();
    void SetMilliseconds(long _milliseconds);
    long GetMilliseconds() const;
    void SetImage(CppSdlImageHolder _image);
    CppSdlImageHolder GetImage() const;
private:
    CppSdlImageHolder _image;
    long _milliseconds;
};

} // namespace

#endif	/* _CPPSDLFRAME_HPP */

