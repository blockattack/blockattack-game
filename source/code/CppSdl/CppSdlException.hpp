/* 
 * File:   CppSdlException.hpp
 * Author: poul
 *
 * Created on 7. november 2010, 13:19
 */

#ifndef _CPPSDLEXCEPTION_HPP
#define	_CPPSDLEXCEPTION_HPP

#include <string>


namespace CppSdl {
class CppSdlException : std::exception{
public:
    CppSdlException();
    CppSdlException(std::string msg);
    CppSdlException(const CppSdlException& orig);
    virtual ~CppSdlException() throw();
    virtual const char* what() const throw();
private:
    std::string message;
};


}
#endif	/* _CPPSDLEXCEPTION_HPP */

