#include "table.h"

Table::Table(vector<int>& dataType_, vector<string>& column_, int treeMaxCapacity) {
	assert(dataType_.size() == column_.size());
	assert(!dataType_.empty());
	for (const int dt : dataType_) {
		dataType.push_back(dt);
	}
	for (const string& str : column_) {
		column.push_back(str);
	}
	numCols = column.size();
	tree = new BPlusTree(treeMaxCapacity);
}

Table::~Table() {
	delete tree;
}

void Table::insert(int key, vector<string>& value) {
	tree->insert(key, value);
}

void Table::update(int key, vector<string>& value) {
	tree->update(key, value);
}

void Table::remove(int key) {
	tree->remove(key);
}
