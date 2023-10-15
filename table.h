#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <string>
#include <assert.h>
#include "bplustree.h"

using namespace std;

// 数据库表中的数据类型
enum DataType {
	TYPE_INT = 0,
	TYPE_FLOAT,
	TYPE_STRING
};

class Table {
public:
	string tableName;
	BPlusTree* tree;
	vector<int> dataType;
	vector<string> column;
	int numCols;

	Table(vector<int>&, vector<string>&, int treeMaxCapacity = 4);
	~Table();

	void insert(int, vector<string>&);
	void update(int, vector<string>&);
	void remove(int);
};

#endif	// TABLE_H