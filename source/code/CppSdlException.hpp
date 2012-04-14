/* 
 * File:   CppSdlException.hpp
 * Author: poul
 *
 * Created on 7. november 2010, 13:19
 */

#ifndef _CPPSDLEXCEPTION_HPP
#define	_CPPSDLEXCEPTION_HPP

#include <string>

#define CPPSDL_ERROR_DATA 1
#define CPPSDL_ERROR_INVALID 2
#define CPPSDL_ERROR_MISSINGFILE 3
#define CPPSDL_ERROR_NULLPOINTER 4

enum Subsystem {OTHER,IMAGE,TILE,SPRITE,LAYER,SCREEN };


namespace CppSdl {
class CppSdlException : std::exception{
public:
    //CppSdlException();
    /**
     * Constructor for making a fully CppSdl compliant expection
     * @param subsystem Can be: OTHER,IMAGE,TILE,SPRITE,LAYER,SCREEN
     * @param errorNumber 1=Data error, 2=Invalid, 3=Missingfile, 4=null pointer
     * @param msg Human readable error message.
     */
    CppSdlException(Subsystem subsystem, long errorNumber, std::string msg);
    CppSdlException(const CppSdlException& orig);
    virtual ~CppSdlException() throw();
    virtual const char* what() const throw();
    long GetErrorNumber();
    Subsystem GetSubSystem();
private:
    std::string _message;
    long _errorNumber;
    Subsystem _subsystem;
};


}
#endif	/* _CPPSDLEXCEPTION_HPP */

