/* 
 * File:   CppSdlException.cpp
 * Author: poul
 * 
 * Created on 7. november 2010, 13:19
 */

#include "CppSdlException.hpp"

namespace CppSdl {

CppSdlException::CppSdlException(Subsystem subsystem,long errorNumber, std::string msg) {
    _errorNumber = errorNumber;
    _message = msg;
    _subsystem = subsystem;
}

CppSdlException::CppSdlException(const CppSdlException& orig) {
}

CppSdlException::~CppSdlException() throw() {
}

const char* CppSdlException::what() const throw() {
    return _message.c_str();
}

long CppSdlException::GetErrorNumber() {
    return _errorNumber;
}

Subsystem CppSdlException::GetSubSystem() {
    return _subsystem;
}

}
