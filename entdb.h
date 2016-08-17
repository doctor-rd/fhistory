#ifndef ENTDB_H
#define ENTDB_H

#include <string>
#include <list>

#include "history.h"

class EntryDB {
public:
	struct Item {
		Entry entry;
		Item *parent;
		std::list<std::string> filenames;
	};
	Item* getItem( Item *p, Entry &e );
	std::list<Item> items;
};

#endif
