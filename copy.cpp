#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <vector>

#include "history.h"

using namespace std;

int main( int argc, char* const argv[] ) {
	if( argc < 3 ) {
		cerr << "too few arguments\n";
		return 1;
	}
	int infd = open( argv[1], 0 );
	if( infd == -1 ) {
		perror( "" );
		return 1;
	}
	int outfd = open( argv[2], 0 );
	if( outfd == -1 ) {
		perror( "" );
		return 1;
	}
	try {
		vector<Entry> es = getHistory( infd );
		initHistory( outfd );
		setHistory( outfd, es );
	}
	catch( string c ) {
		cerr << c;
		return 1;
	}
	return 0;
}
