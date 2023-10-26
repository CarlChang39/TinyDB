#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

#include "bplustree.h"
#include "table.h"

using namespace std;

class Database {
public:
	string owner;
	unordered_set<string> tableNames;				// 当前存在的表名称（带后缀）
	unordered_set<string> viewNames;				// 当前存在的视图名称（带后缀）

	unordered_map<string, Table*> openedTables;		// 当前打开操作的表或视图

	Database(const string&);
	~Database();

	bool loadTable(const string&);					// 打开表（放入openTables中）
	bool saveTable(const string&);					// 保存表
};

#endif	// DATABASE_H