#include "cursor.h"

Cursor::Cursor(BPlusTree& tree) {
	if (tree.numRows == 0) {
		cout << "Cursor Error: Tree is empty!" << endl;
		exit(0);
	}
	// 初始化光标指向表中第一条记录
	node = tree.findFrontLeaf();
	index = 0;
	key = node->keys[index];
	numCols = node->values[index].size();
	value.resize(numCols, "");
	for (int i = 0; i < numCols; i++) {
		value[i] = node->values[index][i];
	}
	isLastRecord = tree.numRows == 1 ? true : false;
}

void Cursor::print() {
	cout << key << ": [";
	int i;
	for (i = 0; i < numCols-1; i++) {
		cout << value[i] << ", ";
	}
	cout << value[i] << "]" << endl;
}

void Cursor::checkIfLast() {
	if (index == node->keys.size()-1 && node->next == nullptr) {
		isLastRecord = true;
	}
}

void Cursor::advance() {
	checkIfLast();
	if (isLastRecord == true) {
		cout << "Cursor Error: cursor already point to the last record!" << endl;
		exit(0);
	}
	if (index == node->keys.size() - 1) {
		node = node->next;
		index = 0;
		key = node->keys[index];
		for (int i = 0; i < numCols; i++) {
			value[i] = node->values[index][i];
		}
	}
	else {
		index++;
		key = node->keys[index];
		for (int i = 0; i < numCols; i++) {
			value[i] = node->values[index][i];
		}
	}
}