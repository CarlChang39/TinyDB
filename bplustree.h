#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <tuple>
#include <algorithm>

using namespace std;

class Node {
public:
    // key从1开始递增
    vector<int> keys;
    Node* parent;

    vector<Node*> children;
    vector<vector<string>> values;
    Node* next;
    Node* prev;

    bool isLeaf;

    Node(Node* parent = nullptr, bool isLeaf = false, Node* prev_ = nullptr, 
         Node* next_ = nullptr);

    int indexOfChild(int);
    int indexOfKey(int);
    Node* getChild(int); 
    Node* getLeftChild();                               // 获取最左侧的子结点
    Node* getRightChild();                              // 获取最右侧的子结点
    void setChild(int, vector<Node*>);
    tuple<int, Node*, Node*> splitInternal();
    vector<string> get(int);
    bool insert(int, vector<string>&);
    bool update(int, vector<string>&);
    tuple<int, Node*, Node*> splitLeaf();
};

class BPlusTree {
public:
    Node* root;
    int maxCapacity;
    int minCapacity;
    int depth;
    int numRows;                                        // 数据库记录条数

    BPlusTree(int _maxCapacity = 4);

    Node* findLeaf(int);
    Node* findFrontLeaf();                              // 获取起始叶子结点
    Node* findBackLeaf();                               // 获取终止叶子结点
    vector<string> get(int);
    void insert(int, vector<string>&);
    void update(int, vector<string>&);
    //void select(int, int);
    void insertNode(tuple<int, Node*, Node*>);
    void removeFromLeaf(int key, Node*);
    void removeFromInternal(int key, Node*);
    void borrowKeyFromRightLeaf(Node*, Node*);
    void borrowKeyFromLeftLeaf(Node*, Node*);
    void mergeNodeWithRightLeaf(Node*, Node*);
    void mergeNodeWithLeftLeaf(Node*, Node*);
    void borrowKeyFromRightInternal(int, Node*, Node*);
    void borrowKeyFromLeftInternal(int, Node*, Node*);
    void mergeNodeWithRightInternal(int, Node*, Node*);
    void mergeNodeWithLeftInternal(int, Node*, Node*);
    void remove(int, Node* node = nullptr);
    void print(Node* node = nullptr, string _prefix = "", bool _last = true);
};

#endif  // BPLUSTREE_H


//int main() {
//    BPlusTree tree(2);
//
//    vector<int> insert_list = { 5, 15, 25, 35, 45, 55, 40, 30, 20 };
//    for (const int i : insert_list) {
//        tree.insert(i, i);
//    }
//
//    vector<int> remove_list = { 40, 5, 45, 35, 25, 55 };
//    for (const int i : remove_list) {
//        tree.remove(i);
//    }
//
//    tree.print();
//
//    return 0;
//}