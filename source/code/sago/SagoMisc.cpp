/*
Copyright (c) 2015 Poul Sander

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SagoMisc.hpp"
#include <physfs.h>
#include <iostream>
#include <iconv.h>
#include <string.h>
#include <memory>

#if PHYSFS_VER_MAJOR < 3
#define PHYSFS_readBytes(X,Y,Z) PHYSFS_read(X,Y,1,Z)
#define PHYSFS_writeBytes(X,Y,Z) PHYSFS_write(X,Y,1,Z)
#endif

using std::string;
using std::cerr;
using std::vector;

namespace sago {


std::vector<std::string> GetFileList(const char* dir) {
	vector<string> ret;
	char** rc = PHYSFS_enumerateFiles(dir);
	for (char** i = rc; *i != NULL; i++) {
		ret.push_back(*i);
	}
	PHYSFS_freeList(rc);
	return ret;
}

bool FileExists(const char* filename) {
	return PHYSFS_exists(filename);
}

std::string GetFileContent(const char* filename) {
	string ret;
	if (!PHYSFS_exists(filename)) {
		cerr << "GetFileContent - File does not exists: " << filename << "\n";
		return ret;
	}
	PHYSFS_file* myfile = PHYSFS_openRead(filename);
	unsigned int m_size = PHYSFS_fileLength(myfile);
	std::unique_ptr<char[]> m_data(new char[m_size]);
	int length_read = PHYSFS_readBytes (myfile, m_data.get(), m_size);
	if (length_read != (int)m_size) {
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file: " << filename << "\n";
		return ret;
	}
	PHYSFS_close(myfile);
	//Now create a std::string 
	ret = string(m_data.get(), m_data.get()+m_size);
	return ret;
}

static void CreatePathToFile(const std::string& path) {
	size_t end_of_path = path.find_last_of("/");
	if (end_of_path == std::string::npos) {
		//No path
		return;
	}
	std::string path2dir = path.substr(0, end_of_path);
	PHYSFS_mkdir(path2dir.c_str());
}

void WriteFileContent(const char* filename, const std::string& content) {
	CreatePathToFile(filename);
	PHYSFS_file* myfile = PHYSFS_openWrite(filename);
	if (!myfile) {
#if PHYSFS_VER_MAJOR > 2
		PHYSFS_ErrorCode code = PHYSFS_getLastErrorCode();
		std::cerr << "Failed to open file for writing, " << PHYSFS_getErrorByCode(code) << " (" << code << ")\n";
#else
		std::cerr << "Failed to open file for writing, " << PHYSFS_getLastError() << "\n";
#endif
		return;
	}
	PHYSFS_writeBytes(myfile, content.c_str(), sizeof(char)*content.length());
	PHYSFS_close(myfile);
}

long int StrToLong(const char* c_string) {
	auto ret = strtol(c_string, nullptr, 10);
	return ret;
}

}
