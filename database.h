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
	unordered_set<string> tableNames;				// ��ǰ���ڵı����ƣ�����׺��
	unordered_set<string> viewNames;				// ��ǰ���ڵ���ͼ���ƣ�����׺��

	unordered_map<string, Table*> openedTables;		// ��ǰ�򿪲����ı����ͼ

	Database(const string&);
	~Database();

	bool loadTable(const string&);					// �򿪱�����openTables�У�
	bool saveTable(const string&);					// �����
};

#endif	// DATABASE_H