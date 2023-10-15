#ifndef CURSOR_H
#define CURSOR_H

#include <string>
#include "bplustree.h"

using namespace std;

class Row {
public:
	// 姓名
	string name;
	// 年龄
	int age;
	// 成绩
	int score;
};

class Cursor {
private:
	int index;				// 当前记录在其结点中的索引
	vector<string> value;	// 当前记录的value
	Node* node;				// 当前记录所在的结点
	int numCols;			// 一条记录的列数（属性数）
public:
	int key;				// 当前记录的key
	bool isLastRecord;		// 是否到达最后一条记录

	Cursor(BPlusTree& tree);

	void print();			
	void checkIfLast();		// 检查当前是否指向最后一条记录			
	void advance();			// 光标向下移动一条记录
};

#endif	// CURSOR_H


//Cursor* cursor = new Cursor(tree);
//do {
//	cursor->print();
//	cursor->advance();
//} while (!cursor->isLastRecord);