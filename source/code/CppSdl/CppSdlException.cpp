/* 
 * File:   CppSdlException.cpp
 * Author: poul
 * 
 * Created on 7. november 2010, 13:19
 */

#include "CppSdlException.hpp"

namespace CppSdl {

CppSdlException::CppSdlException() {
    message = "An CppSdlException";
}

CppSdlException::CppSdlException(std::string msg) {
    message = msg;
}

CppSdlException::CppSdlException(const CppSdlException& orig) {
}

//CppSdlException::~CppSdlException() {
//}

const char* CppSdlException::what() const throw() {
    return message.c_str();
}

}