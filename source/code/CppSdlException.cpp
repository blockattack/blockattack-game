/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2013 Poul Sander

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

#include "CppSdlException.hpp"

namespace CppSdl
{

CppSdlException::CppSdlException(Subsystem subsystem,long errorNumber, std::string msg)
{
	_errorNumber = errorNumber;
	_message = msg;
	_subsystem = subsystem;
}

CppSdlException::CppSdlException(const CppSdlException& orig)
{
}

CppSdlException::~CppSdlException() throw()
{
}

const char* CppSdlException::what() const throw()
{
	return _message.c_str();
}

long CppSdlException::GetErrorNumber()
{
	return _errorNumber;
}

Subsystem CppSdlException::GetSubSystem()
{
	return _subsystem;
}

}
