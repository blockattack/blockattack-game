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
	char* m_data = new char[m_size+1];
	int length_read = PHYSFS_read (myfile, m_data, 1, m_size);
	if (length_read != (int)m_size) {
		delete [] m_data;
		m_data = nullptr;
		PHYSFS_close(myfile);
		cerr << "Error: Curropt data file: " << filename << "\n";
		return ret;
	}
	PHYSFS_close(myfile);
	m_data[m_size] = 0; //ensure that we are 0 terminated
	ret = m_data;
	delete [] m_data;
	return ret;
}

void WriteFileContent(const char* filename, const std::string& content) {
	PHYSFS_file* myfile = PHYSFS_openWrite(filename);
	if (!myfile) {
		cerr << "Failed to open file for writing, " << PHYSFS_getLastError() << "\n";
		return;
	}
	PHYSFS_write(myfile, content.c_str(), sizeof(char), content.length());
	PHYSFS_close(myfile);
}

}
