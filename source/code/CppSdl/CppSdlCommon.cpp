/* 
 * File:   CppSdlCommon.cpp
 * Author: poul
 * 
 * Created on 7. december 2010, 20:12
 */

#include "CppSdlCommon.hpp"


namespace CppSdl {

CppSdlCommon* CppSdlCommon::instance = NULL;

CppSdlCommon* CppSdlCommon::getInstance() {
    if(!instance) {
        instance = new CppSdlCommon();
        return instance;
    } else
        return instance;
}
void CppSdlCommon::setTime(long time) {
    this->time = time;
}
long CppSdlCommon::getTime() const {
    return time;
}

CppSdlCommon::CppSdlCommon() {
}


} //namespace