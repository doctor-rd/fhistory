#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>

#include "history.h"

using namespace std;

void dumpEntry( Entry &e ) {
	char *tmp = ctime( &e.date );
	cout << e.md5Hex() << "  ";
	if( tmp == NULL )
		cout << "invalid\n";
	else
		cout << tmp;
}

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
		cout << "history:\n";
		for( Entry &i : es )
			dumpEntry( i );
		Entry e = getCurrent( fd );
		bool nocommit = false;
		if( es.empty() )
			nocommit = true;
		else if( memcmp( &e, &es.back(), sizeof( Entry ) ) != 0 )
			nocommit = true;
		if( nocommit ) {
			cout << "\nnot committed:\n";
			dumpEntry( e );
		}
	}
	catch( string c ) {
		cerr << c;
		return 1;
	}
	return 0;
}
