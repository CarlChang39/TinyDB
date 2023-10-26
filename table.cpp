#include "table.h"

Table::Table(vector<DataType>& dataType_, vector<string>& column_, unordered_map<string, int>& column2index_, vector<vector<string>>& rows, int treeMaxCapacity, bool isView_) {
	assert(dataType_.size() == column_.size());
	assert(!dataType_.empty());
	for (const auto& dt : dataType_) {
		dataType.push_back(dt);
	}
	for (const string& str : column_) {
		column.push_back(str);
	}
	for (auto& pair : column2index_) {
		column2index[pair.first] = pair.second;
	}
	numCols = column.size();
	isView = isView_;
	
	// 插入数据
	tree = new BPlusTree(treeMaxCapacity);
	
	for (auto& row : rows) {
		tree->insert(stoi(row[0]), row);
	}
}

Table::Table(vector<DataType>& dataType_, vector<string>& column_, int treeMaxCapacity, bool isView_) {
	assert(dataType_.size() == column_.size());
	assert(!dataType_.empty());
	for (const auto& dt : dataType_) {
		dataType.push_back(dt);
	}
	for (const string& str : column_) {
		column.push_back(str);
	}
	numCols = column.size();
	isView = isView_;

	// 插入数据
	tree = new BPlusTree(treeMaxCapacity);
}

Table::~Table() {
	if (tree != nullptr) {
		delete tree;
	}
}

bool Table::insert(int key, vector<string>& value) {
	return tree->insert(key, value);
}

void Table::update(int key, vector<string>& value) {
	tree->update(key, value);
}

void Table::remove(int key) {
	tree->remove(key);
}

bool Table::get(int key) {
	vector<string> result = tree->get(key);
	if (result.empty()) {
		// 查询结果为空，说明key不存在
		return false;
	}
	return true;
}

void Table::getRows(vector<vector<string>>& rows) {
	Node* frontLeaf = tree->findFrontLeaf();
	while (frontLeaf != nullptr) {
		for (auto& value : frontLeaf->values) {
			rows.push_back(value);
		}
		frontLeaf = frontLeaf->next;
	}
	return;
}