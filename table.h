#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <string>
#include <assert.h>
#include <unordered_map>
#include "bplustree.h"
#include "config.h"

using namespace std;

class Table {
public:
    /* 
        Primary key Ĭ��Ϊ�±�Ϊ0��Ԫ�أ�dataType��column��
        ���ݿ����ÿ�У�vector<string>���ͣ���������Ԫ�أ�
        Ҳ����˵����Ԫ����btree����µ�key��value�ж�����
    */
	string tableName;
	BPlusTree* tree = nullptr;
	vector<DataType> dataType;
	vector<string> column;
    unordered_map<string, int> column2index;    // ��¼���Ե����λ�ã��±꣩
	int numCols;
    bool isView;                                // �Ƿ�����ͼ

    Table(vector<DataType>&, vector<string>&, unordered_map<string, int>&, vector<vector<string>>&, int treeMaxCapacity = 4, bool isView_ = false);
    Table(vector<DataType>&, vector<string>&, int treeMaxCapacity = 4, bool isView_ = false);
    ~Table();

	bool insert(int, vector<string>&);
	void update(int, vector<string>&);
	void remove(int);
    bool get(int);              

    void getRows(vector<vector<string>>&);      // ��ȡ�������м�¼
};

#endif	// TABLE_H


//int main() {
    //vector<DataType> dataType = { TYPE_STRING, TYPE_INT };
    //vector<string> column = { "name", "age" };
    //Table table(dataType, column, 2);

    //vector<int> insert_list = { 5, 15, 25, 35, 45, 55, 40, 30, 20 };
    //for (const int i : insert_list) {
    //    vector<string> s(2);
    //    s[0] = to_string(i);
    //    s[1] = 'h';
    //    table.insert(i, s);
    //}

    //table.tree->print();

    //vector<vector<string>> rows;
    //table.getRows(rows);

    //for (auto& row : rows) {
    //    for (auto& item : row) {
    //        cout << item << ' ';
    //    }
    //    cout << endl;
    //}

    //return 0;
//}