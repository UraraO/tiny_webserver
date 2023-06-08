

//#include <cassert>
//#include <cerrno>
//#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
//#include <sys/stat.h>


#include "file_util.h"

// constructor
append_file::append_file(string filename) :
	m_fp(fopen(filename.c_str(), "a+")) {
	setbuffer(m_fp, m_buffer, sizeof(m_buffer));
}

// destructor
append_file::~append_file() {
	fclose(m_fp);
}

void append_file::append(const char *logline, const size_t length) {
	auto n = write(logline, length);
	auto remain = length - n;
	auto x = n;
	while(n > 0) {
		x = write(logline + n, remain);
		if(x == 0) {
			int err = ferror(m_fp);
			if (err) fprintf(stderr, "AppendFile::append() failed !\n");
			break;
		}
		n += x;
		remain = length - n;
	}
}

void append_file::flush() {
	fflush(m_fp);
}

size_t append_file::write(const char *logline, const size_t length) {
	return fwrite_unlocked(logline, 1, length, m_fp);
}























