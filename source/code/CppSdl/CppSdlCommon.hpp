/* 
 * File:   CppSdlCommon.hpp
 * Author: poul
 *
 * Created on 7. december 2010, 20:12
 */

#ifndef _CPPSDLCOMMON_HPP
#define	_CPPSDLCOMMON_HPP

namespace CppSdl {
class CppSdlCommon {
public:
    CppSdlCommon* getInstance();
    void setTime(long time);
    long getTime() const;
protected:
    CppSdlCommon();

private:
    static CppSdlCommon *instance;
    long time;
};
}//namespace

#endif	/* _CPPSDLCOMMON_HPP */

