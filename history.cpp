#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iomanip>

#include "history.h"

#define HIST_MAXLEN ( 1024 / sizeof( Entry ) )
#define HIST_NAME "user.history"

std::string Entry::md5Hex() {
	std::ostringstream ss;
	ss  << std::hex << std::setfill( '0' );
	for( int i = 0; i < MD5_DIGEST_LENGTH; i++ )
		ss << std::setw( 2 ) << (int) md5[i];
	return ss.str();
}

std::vector<Entry> getHistory( int fd ) {
	std::vector<Entry> res;
	res.resize( HIST_MAXLEN );
	int buflen = fgetxattr( fd, HIST_NAME, res.data(), sizeof( Entry ) * HIST_MAXLEN );
	if( buflen == -1 )
		throw std::string( "no history available\n" );
	res.resize( buflen / sizeof( Entry ) );
	return res;
}

void initHistory( int fd ) {
	if( fsetxattr( fd, HIST_NAME, NULL, 0, XATTR_CREATE ) == -1 )
		throw std::string( "init failed\n" );
}

void setHistory( int fd, std::vector<Entry> &in ) {
	int len = in.size();
	if( len > HIST_MAXLEN )
		throw std::string( "history size exceeds limit\n" );
	if( fsetxattr( fd, HIST_NAME, in.data(), sizeof( Entry ) * len, XATTR_REPLACE ) == -1 )
		throw std::string( "set failed\n" );
}

Entry getCurrent( int fd ) {
	Entry res;

	struct stat s;
	fstat( fd, &s );
	if( !S_ISREG( s.st_mode ) )
		throw std::string( "not a regular file\n" );
	res.date = s.st_mtime;

	MD5_CTX c;
	MD5_Init( &c );
	if( lseek( fd, 0, SEEK_SET ) == -1 )
		throw std::string( strerror( errno ) ) + std::string( "\n" );
	while( true ) {
		unsigned char buf[4096];
		int len = read( fd, buf, 4096 );
		if( len == -1 )
			throw std::string( strerror( errno ) ) + std::string( "\n" );
		if( len == 0 )
			break;
		MD5_Update( &c, buf, len );
	}
	MD5_Final( res.md5, &c );

	return res;
}

bool operator<=( std::vector<Entry> &l, std::vector<Entry> &r ) {
	int len = l.size();
	if( len <= r.size() )
		return memcmp( l.data(), r.data(), len * sizeof( Entry ) ) == 0;
	return false;
}
