#include "database.h"

Database::Database(const string& username) : owner(username) {

    string path = "database\\" + username;

    for (const auto& entry : filesystem::directory_iterator(path)) {
        string filename = entry.path().filename().string();
        if (filename.ends_with(".table")) {
            // 表文件
            tableNames.insert(filename);
        }
        else if (filename.ends_with(".view")) {
            // 视图文件
            viewNames.insert(filename);
        }
    }
}

// 打开表（放入openTables中）
bool Database::loadTable(const string& tablename) {
    string fullname = tablename.ends_with(".table") ? tablename : tablename + ".table";
    if (openedTables.count(fullname)) {
        // cerr << "Load table error: Table already open." << endl;
        return true;
    }
    else if (!tableNames.count(fullname)) {
        cerr << "Load table error: Wrong table name." << endl;
        return false;
    }

    // 读取数据库表文件
    string filePath = "database\\" + owner + "\\" + fullname;
    ifstream file(filePath);
    if (file.is_open()) {
        int i = 0;
        string line;
        vector<DataType> dataType;
        vector<string> column;
        unordered_map<string, int> column2index;
        vector<vector<string>> rows;
          
        while (getline(file, line)) {
            istringstream iss(line);
            string token;

            if (i == 0) {
                // 第一行，存储了表属性的数据类型
                while (iss >> token) {
                    //cout << "i == 0 " << token << endl;
                    DataType dt;
                    if (token == "INT") {
                        dt = TYPE_INT;
                    }
                    else if (token == "FLOAT") {
                        dt = TYPE_FLOAT;
                    }
                    else if (token == "STRING") {
                        dt = TYPE_STRING;
                    }
                    dataType.push_back(dt);
                }
            }
            else if (i == 1) {
                // 第二行，存储了表属性的值
                while (iss >> token) {
                    //cout << "i == 1 " << token << endl;
                    column.push_back(token);
                }
            }
            else if (i == 2) {
                // 第三行，存储了表属性和值的映射关系
                int j = 0;
                while (iss >> token) {
                    //cout << "i == 2 " << token << endl;
                    column2index[column[j]] = stoi(token);
                    j++;
                }
            }
            else {
                // 从第四行开始，存储数据库表中存储的行
                vector<string> row;
                while (iss >> token) {
                    row.push_back(token);
                }
                rows.push_back(row);
            }
            
            i++;
        }
        file.close();

        // 定义Table*
        Table* table = new Table(dataType, column, column2index, rows);
        openedTables.insert(make_pair(fullname, table));
    }
    else {
        cerr << "Load table error: Unable to open table file." << endl; 
        return false;
    }

    return true;
}


// 保存表
bool Database::saveTable(const string& tablename) {
    string fullname = tablename.ends_with(".table") ? tablename : tablename + ".table";
    if (!openedTables.count(fullname)) {
        // 表没有被打开
        cerr << "Save table error: Table " << tablename << "is not open." << endl;
        return false;
    }
    else if (!tableNames.count(fullname)) {
        // 不存在该表
        cerr << "Save table error: Wrong table name." << endl;
        return false;
    }

    // 打开数据库表文件写入
    ofstream file("database\\" + owner + "\\" + fullname);
    Table* table = openedTables[fullname];

    /* TODO: 修改下面的代码，首先要在Table类中定义获取表中所有行的方法（通过在btree中定义方法并调用） */
    vector<vector<string>> rows;
    table->getRows(rows);

    if (file.is_open()) {
        for (const auto& dt : table->dataType) {
            string dts;
            if (dt == TYPE_INT) {
                dts = "INT";
            }
            else if (dt == TYPE_FLOAT) {
                dts = "FLOAT";
            }
            else if (dt == TYPE_STRING) {
                dts = "STRING";
            }
            file << dts << " ";
        }
        file << "\n";
        for (const auto& col : table->column) {
            file << col << " ";
        }
        file << "\n";
        for (const auto& col : table->column) {
            file << table->column2index[col] << " ";
        }
        file << "\n";
        for (const auto& row : rows) {
            for (const auto& item : row) {
                file << item << " ";
            }
            file << "\n";
        }
        file.close();
        //cout << "Table saved successfully" << endl;

        // 释放空间
        delete openedTables[fullname];
        
        // 从openedTables里删除该表
        openedTables.erase(fullname);
    }
    else {
        cerr << "Save table error: Unable to open table file." << endl;
        return false;
    }

    return true;
}


Database::~Database() {
    for (auto& openedTable : openedTables) {
        saveTable(openedTable.first);
        delete openedTable.second;
    }
}