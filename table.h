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
        Primary key 默认为下标为0的元素（dataType，column）
        数据库表中每行（vector<string>类型）包含主键元素，
        也就是说主键元素在btree层次下的key和value中都包含
    */
	string tableName;
	BPlusTree* tree = nullptr;
	vector<DataType> dataType;
	vector<string> column;
    unordered_map<string, int> column2index;    // 记录属性的相对位置（下标）
	int numCols;
    bool isView;                                // 是否是视图

    Table(vector<DataType>&, vector<string>&, unordered_map<string, int>&, vector<vector<string>>&, int treeMaxCapacity = 4, bool isView_ = false);
    Table(vector<DataType>&, vector<string>&, int treeMaxCapacity = 4, bool isView_ = false);
    ~Table();

	bool insert(int, vector<string>&);
	void update(int, vector<string>&);
	void remove(int);
    bool get(int);              

    void getRows(vector<vector<string>>&);      // 获取表中所有记录
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