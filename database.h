#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>

#include "bplustree.h"
#include "table.h"

using namespace std;

class Database {
public:
	vector<Table*> tables;

	Database(string path);
	~Database();

	void start();
};

#endif	// DATABASE_H