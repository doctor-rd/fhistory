#ifndef HISTORY_H
#define HISTORY_H

#include <time.h>
#include <openssl/md5.h>
#include <string>
#include <vector>

struct Entry {
	unsigned char md5[MD5_DIGEST_LENGTH];
	time_t date;
	std::string md5Hex();
};

std::vector<Entry> getHistory( int fd );
void initHistory( int fd );
void setHistory( int fd, std::vector<Entry> &in );
Entry getCurrent( int fd );
bool operator<=( std::vector<Entry> &l, std::vector<Entry> &r );

#endif
