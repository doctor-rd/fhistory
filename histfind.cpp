#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>

#include "history.h"

using namespace std;

list<vector<Entry>> after;
list<vector<Entry>> before;

void usage( char* name ) {
	printf( "Usage: %s [OPTIONS] [[--] COMMAND]\n"
	"Results maybe appended as arguments to COMMAND.\n"
	"\t-h\n\t\tThis\n"
	"\t-a FILENAME\n\t\tRestrict to files after FILENAME.\n"
	"\t-b FILENAME\n\t\tRestrict to files before FILENAME.\n"
	"\t-r FILENAME\n\t\tRestrict to files related to FILENAME.\n"
	, name );
}

bool matches( vector<Entry> &h ) {
	int l = h.size();
	for( vector<Entry> &i : after ) {
		if( l <= i.size() )
			return false;
		if( !( i <= h ) )
			return false;
	}
	for( vector<Entry> &i : before ) {
		if( l >= i.size() )
			return false;
		if( !( h <= i ) )
			return false;
	}
	return true;
}

int main( int argc, char* const argv[] ) {
	int opt;

	while( ( opt = getopt( argc, argv, "ha:b:r:" ) ) != -1 ) {
		int fd;
		switch( opt ) {
		case 'h':
			usage( argv[0] );
			return 1;
		case 'a':
		case 'b':
		case 'r':
			fd = open( optarg, 0 );
			if( fd == -1 ) {
				perror ( optarg );
				return 1;
			}
			try {
				auto h = getHistory( fd );
				switch( opt ) {
				case 'a':
					after.push_back( h );
					break;
				case 'b':
					before.push_back( h );
					break;
				case 'r':
					if( h.size() == 0 ) {
						cerr << optarg << ": history empty\n";
						return 1;
					}
					h.resize( 1 );
					after.push_back( h );
					break;
				}
			}
			catch( string c ) {
				cerr << optarg << ": " << c;
				return 1;
			}
			close( fd );
			break;
		default:
			return 1;
		}
	}
	DIR *d = opendir( "." );
	if( d == NULL ) {
		perror( "cwd" );
		return 1;
	}
	list<string> result;
	struct dirent *de;
	while( ( de = readdir( d ) ) != NULL ) {
		char *name = de->d_name;
		if( *name == '.' )
			continue;
		int fd = openat( dirfd( d ), name, 0 );
		if( fd == -1 )
			continue;
		struct stat s;
		fstat( fd, &s );
		if( S_ISREG( s.st_mode ) ) {
			try {
				auto h = getHistory( fd );
				if( matches( h ) )
					result.push_back( string( name ) );
			}
			catch( string c ) {
			}
		}
		close( fd );
	}
	closedir( d );
	if( result.size() == 0 )
		return 0;
	if( optind < argc ) {
		vector<const char*> args;
		for( ; optind < argc; optind++ )
			args.push_back( argv[optind] );
		for( string &i : result )
			args.push_back( i.c_str() );
		args.push_back( NULL );
		execvp( args[0], (char* const*) args.data() );
		perror( args[0] );
		return 1;
	}
	else
		for( string &i : result )
			cout << i << endl;
	return 0;
}
