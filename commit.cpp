#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <vector>

#include "history.h"

using namespace std;

int main( int argc, char* const argv[] ) {
	int fd = 0;
	if( argc >= 2 ) {
		fd = open( argv[1], 0 );
		if( fd == -1 ) {
			perror( "" );
			return 1;
		}
	}
	try {
		vector<Entry> es = getHistory( fd );
		Entry e = getCurrent( fd );
		bool nocommit = false;
		if( es.empty() )
			nocommit = true;
		else if( memcmp( &e, &es.back(), sizeof( Entry ) ) != 0 )
			nocommit = true;
		if( nocommit ) {
			es.push_back( e );
			setHistory( fd, es );
			struct stat s;
			fstat( fd, &s );
			if( s.st_nlink == 0 )
				cout << "file deleted or replaced" << endl;
		}
		else
			cout << "already up to date\n";
	}
	catch( string c ) {
		cerr << c;
		return 1;
	}
	return 0;
}
