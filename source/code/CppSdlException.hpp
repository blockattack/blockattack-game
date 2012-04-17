/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2012 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
http://blockattack.sf.net
===========================================================================
*/

#ifndef _CPPSDLEXCEPTION_HPP
#define	_CPPSDLEXCEPTION_HPP

#include <string>

#define CPPSDL_ERROR_DATA 1
#define CPPSDL_ERROR_INVALID 2
#define CPPSDL_ERROR_MISSINGFILE 3
#define CPPSDL_ERROR_NULLPOINTER 4

enum Subsystem {OTHER,IMAGE,TILE,SPRITE,LAYER,SCREEN };


namespace CppSdl
{
class CppSdlException : std::exception
{
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

