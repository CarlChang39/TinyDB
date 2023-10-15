#include "bplustree.h"
#include "cursor.h"
#include "database.h"

int main() {
    vector<int> dataType = { TYPE_STRING, TYPE_INT };
    vector<string> column = { "name", "age" };
    Table table(dataType, column, 2);

    vector<int> insert_list = { 5, 15, 25, 35, 45, 55, 40, 30, 20 };
    for (const int i : insert_list) {
        vector<string> s(2);
        s[0] = to_string(i);
        s[1] = 'h';
        table.insert(i, s);
    }

    table.tree->print();
    
    vector<int> remove_list = { 40, 5, 45, 35, 25, 55 };
    for (const int i : remove_list) {
        table.remove(i);
    }

    table.tree->print();

    return 0;
}