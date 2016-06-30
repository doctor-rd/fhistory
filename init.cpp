#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

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
		initHistory( fd );
	}
	catch( string c ) {
		cerr << c;
		return 1;
	}
	return 0;
}
