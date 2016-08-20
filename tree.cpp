#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <iomanip>

#include "entdb.h"
#include "history.h"
#include "timelen.h"

using namespace std;

time_t now;
EntryDB db;
int maxdepth = 0;

void dumpTree( EntryDB::Item *start = 0, string prefix = "", string headprefix = "" ) {
	for( EntryDB::Item &i : db.items )
		if( i.parent == start ) {
			ostringstream entss;
			entss << headprefix << i.entry.md5Hex();
			cout << left << setw( 32 + maxdepth * 4 ) << entss.str() << setw( 16 );
			ostringstream agess;
			timelen_human_readable( agess, now - i.entry.date );
			agess << " ago";
			cout << agess.str() << setw( 0 );
			for( string &f : i.filenames )
				cout << ' ' << f;
			cout << endl;
			dumpTree( &i, prefix + "|   ", prefix + "+---" );
		}
}

int main( int argc, char* const argv[] ) {
	struct timespec tp;
	clock_gettime( CLOCK_REALTIME, &tp );
	now = tp.tv_sec;
	for( int i = 1; i < argc; i++ ) {
		char *filename = argv[i];
		int fd = open( filename, 0 );
		if( fd == -1 ) {
			cerr << filename << ": " << strerror( errno ) << endl;
			continue;
		}
		try {
			vector<Entry> es = getHistory( fd );
			if( es.size() > maxdepth )
				maxdepth = es.size();
			EntryDB::Item *last = 0;
			for( int i = 0; i < es.size(); i++ ) {
				EntryDB::Item *n = db.getItem( last, es[i] );
				if( i == es.size() - 1 )
					n->filenames.push_back( filename );
				last = n;
			}
		}
		catch( string c ) {
			cerr << filename << ": " << c;
		}
		close( fd );
	}
	dumpTree();
	return 0;
}
