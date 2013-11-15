/*
physfs_stream.hpp
    Copyright (C) 2009 Poul Sander
        Copied from Warzone 2100
        http://svn.gna.org/viewcvs/warzone/branches/sound/lib/sound/general/physfs_stream.hpp?rev=3623
	Copyright (C) 2007 Warzone Resurrection Project
	Copyright (C) 2007 Giel van Schijndel <me@mortis.eu>
	Originally part of SuperTux:
	http://svn.berlios.de/svnroot/repos/supertux/trunk/supertux/src/physfs/physfs_stream.cpp revision 4176
	Copyright (C) 2006 Matthias Braun <matze@braunis.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Poul Sander
    blockattack@poulsander.com
*/

#ifndef __INCLUDED_PHYSFS_STREAM_HPP__
#define __INCLUDED_PHYSFS_STREAM_HPP__

#define BUF_SIZE 1024

#include <physfs.h>
#include <streambuf>
#include <istream>
#include <ostream>
/*Compared to Warzone 2100 boost:array has been replaced by vector to limit
 the compile requirements*/
#include <vector>

/** This an implementation of "File-based streams" as described in the
 *  ISO/IEC 14882:1998(E) C++ Standard in chapter 27 "Input/output library",
 *  section 8 "File-based streams". This implementation however uses PhysicsFS
 *  instead of the C-library's FILE functions. Also this implementation lacks
 *  an implementation of basic_fstream because PhysicsFS does not support
 *  opening a file both for reading as well as writing.
 *
 *  This header contains similar classes and typedefs as <fstream> does. Both
 *  similar in names as well as operation, the main difference is that these
 *  classes do _not_ reside in namespace std.
 */
namespace PhysFS
{
/** Typedefinitions mandated by the C++ standard in section 27.8.1
 */
template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class physfs_filebuf;
typedef physfs_filebuf<char>        filebuf;
typedef physfs_filebuf<wchar_t>     wfilebuf;

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class physfs_ifstream;
typedef physfs_ifstream<char>       ifstream;
typedef physfs_ifstream<wchar_t>    wifstream;

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class physfs_ofstream;
typedef physfs_ofstream<char>       ofstream;
typedef physfs_ofstream<wchar_t>    wofstream;

/** This class implements a C++ streambuf object for physfs files.
 *  So that you can use normal istream and ostream operations on them.
 *  This class is similar to std::basic_filebuf as defined in
 *  section 27.8.1.1, except in name.
 */
template<typename _CharT, typename _Traits>
class physfs_filebuf : public std::basic_streambuf<_CharT, _Traits>
{
public:
	// Types:
	typedef _CharT                                  char_type;
	typedef _Traits                                 traits_type;
	typedef typename traits_type::int_type          int_type;
	typedef typename traits_type::pos_type          pos_type;
	typedef typename traits_type::off_type          off_type;

	typedef std::basic_streambuf<_CharT, _Traits>   __streambuf_type;
	typedef physfs_filebuf<_CharT, _Traits>         __filebuf_type;

	physfs_filebuf() :
		_file(0)
	{
		_buf.resize(BUF_SIZE);
	}

	virtual ~physfs_filebuf()
	{
		close();
	}

	bool is_open() const throw()
	{
		return (_file != 0);
	}

	__filebuf_type* open(const char* fileName, std::ios_base::openmode mode)
	{
		// Return failure if this file buf is open already
		if (is_open() != false)
			return 0;

		// Return failure if we are requested to open a file in an unsupported mode
		if (!(mode & std::ios_base::binary) ||
				(mode & std::ios_base::in &&
				 mode & std::ios_base::out))
			return 0;

		// Open the file
		if (mode & std::ios_base::out &&
				mode & std::ios_base::app)
		{
			_file = PHYSFS_openAppend(fileName);
			_writeStream = true;
		}
		else if (mode & std::ios_base::out)
		{
			_file = PHYSFS_openWrite(fileName);
			_writeStream = true;
		}
		else if (mode & std::ios_base::in)
		{
			_file = PHYSFS_openRead(fileName);
			_writeStream = false;
		}
		else
		{
			return 0;
		}

		if (!_file)
			return 0;

		if (mode & std::ios_base::ate &&
				mode & std::ios_base::in)
		{
			if (!PHYSFS_seek(_file, PHYSFS_fileLength(_file)))
			{
				close();
				return 0;
			}
		}

		return this;
	}

	__filebuf_type* close()
	{
		// Return failure if this file buf is closed already
		if (is_open() == false)
			return 0;

		sync();

		if (!PHYSFS_close(_file))
		{
			return 0;
		}

		_file = 0;

		return this;
	}

protected:
	// Read stuff:
	virtual int_type underflow()
	{
		if (!is_open() || _writeStream)
			return traits_type::eof();

		if(PHYSFS_eof(_file))
		{
			return traits_type::eof();
		}

		PHYSFS_sint64 objectsRead = PHYSFS_read(_file, &*_buf.begin(), sizeof(char_type), BUF_SIZE);

		if(objectsRead <= 0)
		{
			return traits_type::eof();
		}

		char_type* xend = (static_cast<size_t> (objectsRead) == BUF_SIZE) ? &*_buf.end() : &_buf[objectsRead];
		this->setg(&*_buf.begin(), &*_buf.begin(), xend);

		return traits_type::to_int_type(_buf.front());
	}

	virtual pos_type seekpos(pos_type pos, std::ios_base::openmode)
	{
		if (!is_open() || _writeStream)
			return pos_type(off_type(-1));

		if(PHYSFS_seek(_file, static_cast<PHYSFS_uint64> (pos)) == 0)
		{
			return pos_type(off_type(-1));
		}

		// the seek invalidated the buffer
		this->setg(&*_buf.begin(), &*_buf.begin(), &*_buf.begin());
		return pos;
	}

	virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode)
	{
		if (!is_open() || _writeStream)
			return pos_type(off_type(-1));

		off_type pos = off;

		switch (dir)
		{
		case std::ios_base::beg:
			break;

		case std::ios_base::cur:
		{
			PHYSFS_sint64 ptell = PHYSFS_tell(_file);
			pos_type buf_tell = static_cast<pos_type> (ptell) - static_cast<pos_type> (__streambuf_type::egptr() - __streambuf_type::gptr());
			if(off == 0)
			{
				return buf_tell;
			}

			pos += static_cast<off_type> (buf_tell);
			break;
		}

		case std::ios_base::end:
			pos = static_cast<off_type> (PHYSFS_fileLength(_file)) - pos;
			break;

		default:
			//assert(!"invalid seekdirection");
			return pos_type(off_type(-1));
		}

		return seekpos(static_cast<pos_type> (pos), mode);
	}

	// Write stuff:
	virtual int_type overflow(int_type c = traits_type::eof())
	{
		if (!is_open() || !_writeStream)
			return traits_type::eof();

		size_t size = __streambuf_type::pptr() - __streambuf_type::pbase();
		if(size == 0)
			return 0;

		PHYSFS_sint64 res = PHYSFS_write(_file, __streambuf_type::pbase(), sizeof(char_type), size);

		if(res <= 0)
			return traits_type::eof();

		if(!traits_type::eq_int_type(c, traits_type::eof()))
		{
			PHYSFS_sint64 res = PHYSFS_write(_file, &c, sizeof(char_type), 1);

			if(res <= 0)
				return traits_type::eof();
		}

		char_type* xend = (static_cast<size_t> (res) == BUF_SIZE) ? &*_buf.end() : &_buf[res];
		this->setp(&*_buf.begin(), xend);
		return 0;
	}

	virtual int sync()
	{
		if (!is_open() || !_writeStream)
			return -1;

		return overflow(traits_type::eof());
	}

	virtual std::streamsize showmanyc()
	{
		if (!is_open() || _writeStream)
			return -1;

		PHYSFS_sint64 fileSize = PHYSFS_fileLength(_file);

		return static_cast<int> (fileSize);
	}

private:
	PHYSFS_file* _file;
	bool _writeStream;
	vector<char_type> _buf;
};

/** This class implements a C++ istream object for physfs files.
 *  This class is similar to std::basic_ifstream as defined in
 *  section 27.8.1.5, except in name.
 */
template<typename _CharT, typename _Traits>
class physfs_ifstream : public std::basic_istream<_CharT, _Traits>
{
public:
	// Types:
	typedef _CharT                                  char_type;
	typedef _Traits                                 traits_type;
	typedef typename traits_type::int_type          int_type;
	typedef typename traits_type::pos_type          pos_type;
	typedef typename traits_type::off_type          off_type;

	typedef physfs_filebuf<_CharT, _Traits>         __filebuf_type;
	typedef std::basic_istream<_CharT, _Traits>     __istream_type;

	physfs_ifstream() throw() :
		__istream_type(&_sb)
	{
	}

	explicit physfs_ifstream(const char* fileName, std::ios_base::openmode mode = std::ios_base::binary) :
		__istream_type(&_sb)
	{
		open(fileName, mode);
	}

	const __filebuf_type* rdbuf() const throw()
	{
		return &_sb;
	}

	__filebuf_type* rdbuf() throw()
	{
		return &_sb;
	}

	bool is_open() const throw()
	{
		return rdbuf()->is_open();
	}

	void open(const char* fileName, std::ios_base::openmode mode = std::ios_base::binary)
	{
		if (!rdbuf()->open(fileName, mode | std::ios_base::in))
		{
			__istream_type::setstate(std::ios_base::failbit);
		}
	}

	void close()
	{
		if (!rdbuf()->close())
		{
			__istream_type::setstate(std::ios_base::failbit);
		}
	}

private:
	__filebuf_type _sb;
};

/** This class implements a C++ ostream object for physfs files.
 *  This class is similar to std::basic_ofstream as defined in
 *  section 27.8.1.8, except in name.
 */
template<typename _CharT, typename _Traits>
class physfs_ofstream : public std::basic_ostream<_CharT, _Traits>
{
public:
	// Types:
	typedef _CharT                                  char_type;
	typedef _Traits                                 traits_type;
	typedef typename traits_type::int_type          int_type;
	typedef typename traits_type::pos_type          pos_type;
	typedef typename traits_type::off_type          off_type;

	typedef physfs_filebuf<_CharT, _Traits>         __filebuf_type;
	typedef std::basic_ostream<_CharT, _Traits>     __ostream_type;

	physfs_ofstream() throw() :
		__ostream_type(&_sb)
	{
	}

	explicit physfs_ofstream(const char* fileName, std::ios_base::openmode mode = std::ios_base::binary) :
		__ostream_type(&_sb)
	{
		open(fileName, mode);
	}

	const __filebuf_type* rdbuf() const throw()
	{
		return &_sb;
	}

	__filebuf_type* rdbuf() throw()
	{
		return &_sb;
	}

	bool is_open() const throw()
	{
		return rdbuf()->is_open();
	}

	void open(const char* fileName, std::ios_base::openmode mode = std::ios_base::binary)
	{
		if (!rdbuf()->open(fileName, mode | std::ios_base::out))
		{
			__ostream_type::setstate(std::ios_base::failbit);
		}
	}

	void close()
	{
		if (!rdbuf()->close())
		{
			__ostream_type::setstate(std::ios_base::failbit);
		}
	}

private:
	__filebuf_type _sb;
};
}

#endif // __INCLUDED_PHYSFS_STREAM_HPP__
