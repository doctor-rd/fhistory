#include <string.h>

#include "entdb.h"

EntryDB::Item* EntryDB::getItem( Item *p, Entry &e ) {
	for( Item &i : items )
		if( i.parent == p )
			if( memcmp( &i.entry, &e, sizeof( Entry ) ) == 0 )
				return &i;
	items.emplace_back();
	Item &n = items.back();
	n.entry = e;
	n.parent = p;
	return &n;
}
