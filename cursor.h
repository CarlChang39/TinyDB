#ifndef CURSOR_H
#define CURSOR_H

#include <string>
#include "bplustree.h"

using namespace std;

class Row {
public:
	// ����
	string name;
	// ����
	int age;
	// �ɼ�
	int score;
};

class Cursor {
private:
	int index;				// ��ǰ��¼�������е�����
	vector<string> value;	// ��ǰ��¼��value
	Node* node;				// ��ǰ��¼���ڵĽ��
	int numCols;			// һ����¼����������������
public:
	int key;				// ��ǰ��¼��key
	bool isLastRecord;		// �Ƿ񵽴����һ����¼

	Cursor(BPlusTree& tree);

	void print();			
	void checkIfLast();		// ��鵱ǰ�Ƿ�ָ�����һ����¼			
	void advance();			// ��������ƶ�һ����¼
};

#endif	// CURSOR_H


//Cursor* cursor = new Cursor(tree);
//do {
//	cursor->print();
//	cursor->advance();
//} while (!cursor->isLastRecord);