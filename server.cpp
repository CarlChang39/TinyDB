#include "server.h"

void Server::start() {
	loadAccountInfo();

	while (true) {
		// 打印提示信息
		cout << "TinyDB > ";

		// 获取用户输入
		string input;
		getline(cin, input);
		int length = input.length();

		// 创建用户（数据库）
		if (input.starts_with("CREATE USER")) {
			dealCreateUser(input);
		}
		// 删除用户（数据库）
		else if (input.starts_with("DROP USER")) {
			dealDropUser(input);
		}
		// 显示当前存在的用户
		else if (input == "SHOW USERS") {
			dealShowUsers();
		}
		// 启动指定用户（数据库）
		else if (input.starts_with("USE USER")) {
			dealUseUser(input);
		} 

		// 查看当前数据库中所有的表
		else if (input == "SHOW TABLES") {
			dealShowTables();
		}
		// 查看当前数据库中所有的视图
		else if (input == "SHOW VIEWS") {
			dealShowViews();
		}
		// 建表
		else if (input.starts_with("CREATE TABLE")) {
			dealCreateTable(input);
		}
		// 删除表
		else if (input.starts_with("DROP TABLE")) {
			dealDropTable(input);
		}
		// 打印表的属性
		else if (input.starts_with("SHOW TABLE")) {
			if (input.ends_with("COLUMNS")) {
				dealShowColumns(input);
			}
			else if (input.ends_with("PRIMARYKEY")) {
				dealShowPrimaryKey(input);
			}
		}
		// 插入数据
		else if (input.starts_with("INSERT INTO TABLE")) {
			dealInsert(input);
		}
		// 查看数据
		else if (input.starts_with("SELECT")) {
			dealSelect(input);
		}
		// 删除数据
		else if (input.starts_with("DELETE FROM TABLE")) {
			dealDelete(input);
		}

		// 退出数据库
		else if (input == "EXIT") {
			cout << "Bye!" << endl;
			break;
		}
		// 非法输入
		else {
			cerr << "Unrecognized command. Please check your input." << endl;
		}
	}
}

// 处理新建用户请求
bool Server::dealCreateUser(const string& input) {
	vector<string> words;
	parseSentence(input, words);

	if (words.size() == 5 && words[3] == "PASSWORD") {
		string username = words[2];
		if (accountInfo.count(username)) {
			// 用户已存在
			cerr << "Create user error: User already exist." << endl;
			return false;
		} 
		
		// 保存用户名密码信息
		string password = words[4];
		accountInfo[username] = password;
		saveAccountInfo();
		cout << "User created successfully." << endl;

		// 新建数据库路径
		string path = "database\\" + username;

		// 将 std::string 转换为宽字符字符串
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, NULL, 0);
		wstring wideFolderPath(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, &wideFolderPath[0], size_needed);
		// 使用CreateDirectory函数创建目录
		if (CreateDirectory(wideFolderPath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
			cout << "User database created successfully." << endl;
		}
		else {
			cerr << "Create user database error: Error creating folder." << endl;
			return false;
		}
	}
	else {
		// SQL语句格式不正确
		cerr << "Create user error: Invalid syntex." << endl;
		return false;
	}
	
	return true;
}

// 处理删除用户请求
bool Server::dealDropUser(const string& input) {
	vector<string> words;
	parseSentence(input, words);

	if (words.size() == 5 && words[3] == "PASSWORD") {
		string username = words[2];
		if (!accountInfo.count(username)) {
			// 用户不存在
			cerr << "Drop user error: User doesn't exist." << endl;
			return false;
		}

		string password = words[4];
		if (password != accountInfo[username]) {
			// 密码不正确
			cerr << "Drop user error: Wrong password." << endl;
			return false;
		}
		// 删除用户
		accountInfo.erase(username);
		saveAccountInfo();
		cout << "Drop user successfully." << endl;

		// 删除数据库
		string path = "database\\" + username;
		// 将 std::string 转换为宽字符字符串
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, NULL, 0);
		wstring wideFolderPath(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, &wideFolderPath[0], size_needed);

		SHFILEOPSTRUCT fileOp;
		ZeroMemory(&fileOp, sizeof(SHFILEOPSTRUCT));
		fileOp.wFunc = FO_DELETE;
		fileOp.pFrom = wideFolderPath.c_str();
		fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI | FOF_WANTNUKEWARNING;

		if (SHFileOperation(&fileOp) == 0) {
			cout << "User database removed successfully." << endl;
		}
		else {
			cerr << "Drop user error: Error removing user database." << endl;
			return false;
		}

		// 如果删除的是当前用户，释放database并指向nullptr
		if (database != nullptr && username == database->owner) {
			delete database;
			database = nullptr;
		}
	}
	else {
		// SQL语句格式不正确
		cerr << "Drop user error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// 执行启用指定用户（数据库）
bool Server::dealUseUser(const string& input) {
	vector<string> words;
	parseSentence(input, words);

	if (words.size() == 5 && words[3] == "PASSWORD") {
		string username = words[2];

		if (!accountInfo.count(username)) {
			// 用户不存在
			cerr << "Use user error: User doesn't exist." << endl;
			return false;
		}

		if (database != nullptr && username == database->owner) {
			// 要启用的用户就是当前用户
			cerr << "Use user error: You are already using this user." << endl;
			return false;
		}

		string password = words[4];
		if (password != accountInfo[username]) {
			// 密码不正确
			cerr << "Use user error: Wrong password." << endl;
			return false;
		}

		// 切换数据库
		delete database;
		database = new Database(username);
		cout << "Using user " << username << endl;
	}
	else {
		// SQL语句格式不正确
		cerr << "Use user error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// 建表
bool Server::dealCreateTable(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Create table error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);
	int size = words.size();
	
	if (size < 6) {
		cerr << "Create table error: Wrong syntex." << endl;
		return false;
	}
	
	if (words[size - 2] == ("PRIMARYKEY")) {

		string tablename = words[2]+".table";
		if (database->tableNames.count(tablename)) {
			// 表名和已有的表重复
			cerr << "Create table error: Table name already exists." << endl;
			return false;
		}

		// 解析主键
		string primaryKey = words[size - 1];

		// 解析用户想要定义的dataType和column，同时设置属性到存储下标的映射
		vector<string> dataTypeString;
		vector<string> column;
		unordered_map<string, int> column2index;
		for (int i = 3; i < size - 2; i++) {
			size_t pos = words[i].find(':');
			if (pos == string::npos) {
				// 属性定义的格式不正确
				cerr << "Create table error: Invalid column definition." << endl;
				return false;
			}
			
			string col = words[i].substr(0, pos);
			if (col.length() == 0) {
				// 属性名称不能为空
				cerr << "Create table error: Column value can't be NULL." << endl;
				return false;
			}
			
			string dts = words[i].substr(pos+1, words[i].length()-1-pos);
			if (dts != "INT" && dts != "STRING" && dts != "FLOAT") {
				cerr << "Create table error: Invalid data type." << endl;
				return false;
			}
			dataTypeString.push_back(dts);
			column.push_back(col);
			column2index[col] = i - 3;
		}

		// 规定：数据库表主键放在第一位
		auto it = find(column.begin(), column.end(), primaryKey);
		if (it == column.end()) {
			// 没有找到定义主键的属性，语句错误
			cerr << "Create table error: Wrong primary key name." << endl;
			return false;
		}
		// 从迭代器转换为下标
		size_t index = distance(column.begin(), it);
		string tmpCol = column[0];
		swap(column[index], column[0]);
		swap(dataTypeString[index], dataTypeString[0]);
		column2index[tmpCol] = index;
		column2index[primaryKey] = 0;

		// 创建表文件并写入表头信息
		ofstream file("database\\" + database->owner + "\\" + tablename);

		if (file.is_open()) {
			for (const auto& dts : dataTypeString) {
				file << dts << " ";
			}
			file << "\n";
			for (const auto& col : column) {
				file << col << " ";
			}
			file << "\n";
			for (const auto& col : column) {
				file << column2index[col] << " ";
			}
			file << "\n";
			file.close();

			cout << "Table created successfully" << endl;

			database->tableNames.insert(tablename);
		}
		else {
			cerr << "Create table error: Unable to open table file." << endl;
			return false;
		}
	}
	else {
		// 必须指定主键（且必须为int类型）
		cerr << "Create table error: Primary key must be assigned (int type only)." << endl;
		return false;
	}
	return true;
}

// 删除表
bool Server::dealDropTable(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Drop table error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);

	if (words.size() == 3) {
		string tablename = words[2];
		if (!database->tableNames.count(tablename + ".table")) {
			// 表名不存在
			cerr << "Drop table error: Wrong table name." << endl;
			return false;
		}
		if (database->openedTables.count(tablename)) {
			// 如果还未关闭，保存
			database->saveTable(tablename);
		}

		// 删除表（使用remove函数）
		if (remove(("database\\" + database->owner + "\\" + tablename + ".table").c_str()) != 0) {	
			cerr << "Drop table error: Can't delete table." << endl;	
			return false;
		}
		else {
			database->tableNames.erase(tablename + ".table");
			cout << "Table successfully deleted" << endl;
			return true;
		}
	}
	else {
		cerr << "Drop table error: Wrong syntex." << endl;
		return false;
	}
}

// 打印表的属性
bool Server::dealShowColumns(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Show columns error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);

	if (words.size() != 4) {
		cerr << "Show columns error: Invalid syntex." << endl;
		return false;
	}

	string tablename = words[2];
	if (!database->loadTable(tablename)) {
		cerr << "Show columns error: Failed loading table." << endl;
		return false;
	};

	Table* table = database->openedTables[tablename + ".table"];
	for (const auto& col : table->column) {
		cout << col << " ";
	}
	cout << endl;
	for (const auto& type : table->dataType) {
		string typeString = "";
		if (type == TYPE_INT) {
			typeString = "INT";
		}
		else if (type == TYPE_FLOAT) {
			typeString = "FLOAT";
		}
		else if (type == TYPE_STRING) {
			typeString = "STRING";
		}
		cout << typeString << " ";
	}
	cout << endl;

	// 释放空间
	delete table;

	// 从openedTables里删除该表
	database->openedTables.erase(tablename + ".table");

	return true;
}

// 打印表的主键
bool Server::dealShowPrimaryKey(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Show primary key error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);

	if (words.size() != 4) {
		cerr << "Show primary key error: Invalid syntex." << endl;
		return false;
	}

	string tablename = words[2];
	if (!database->loadTable(tablename)) {
		cerr << "Show primary key error: Failed loading table." << endl;
		return false;
	};

	Table* table = database->openedTables[tablename + ".table"];
	
	cout << table->column[0] << endl;

	// 释放空间
	delete table;

	// 从openedTables里删除该表
	database->openedTables.erase(tablename + ".table");

	return true;
}

// 插入数据
bool Server::dealInsert(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Insert into table error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);
	int size = words.size();

	if (size < 5) {
		cerr << "Insert into table error: Wrong syntex." << endl;
		return false;
	}

	string tablename = words[3];
	if (!database->loadTable(tablename)) {
		cerr << "Insert into table error: Failed loading table." << endl;
		return false;
	}

	Table* table = database->openedTables[tablename + ".table"];
	if (table->numCols != size - 4) {
		// 插入数量和表中属性数量不等
		cerr << "Insert into table error: Wrong insert value amount." << endl;
		return false;
	}

	vector<string> row(table->numCols, "");
	for (int i = 4; i < size; i++) {
		size_t pos = words[i].find(':');
		if (pos == string::npos) {
			// 属性赋值的格式不正确
			cerr << "Insert into table error: Invalid assignment." << endl;
			return false;
		}

		string col = words[i].substr(0, pos);
		if (col.length() == 0) {
			// 属性名称不能为空
			cerr << "Insert into table error: Column value can't be NULL." << endl;
			return false;
		}

		string value = words[i].substr(pos + 1, words[i].length() - 1 - pos);
		if (value.length() == 0) {
			// 属性值不能为空
			cerr << "Insert into table error: Insert value can't be NULL." << endl;
			return false;
		}
		
		/* TODO: 检查对应数据格式是否正确 */

		row[table->column2index[col]] = value;
	}

	if (!table->insert(stoi(row[0]), row)) {
		// 主键值重复
		cerr << "Insert into table error: Duplicated primary key." << endl;
		return false;
	}

	// 保存修改
	if (database->saveTable(tablename)) {
		cout << "Insert into table successfully." << endl;
	}

	return true;
}

bool Server::dealSelect(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Select error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);
	int size = words.size();

	if (size >= 5 && words[2] == "FROM" && words[3] == "TABLE") {
		string target = words[1];
		string tablename = words[4];

		if (!database->loadTable(tablename)) {
			cerr << "Select error: Failed loading table." << endl;
			return false;
		}

		Table* table = database->openedTables[tablename + ".table"];

		for (const auto& col : table->column) {
			cout << col << " ";
		}
		cout << endl;
		if (target == "*") {
			// 查看表中的全部数据
			vector<vector<string>> rows;
			table->getRows(rows);
			for (auto& row : rows) {
				for (auto& item : row) {
					cout << item << " ";
				}
				cout << endl;
			}
		}
		else {
			cerr << "Select error: Wrong select target." << endl;
			return false;
		}

		// 释放空间
		delete table;

		// 从openedTables里删除该表
		database->openedTables.erase(tablename + ".table");
	}
	else {
		cerr << "Select error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// 删除数据
bool Server::dealDelete(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Delete error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);
	int size = words.size();

	if (size == 6 && words[size - 2] == "WHERE") {
		string tablename = words[3];
		string condition = words[5];

		if (!database->loadTable(tablename)) {
			cerr << "Delete error: Failed loading table." << endl;
			return false;
		}

		Table* table = database->openedTables[tablename + ".table"];

		// 检查条件是否正确
		size_t pkPos = condition.find(':');
		if (pkPos == string::npos) {
			// 属性赋值的格式不正确
			cerr << "Delete error: Invalid condition." << endl;
			return false;
		}

		string pkCol = condition.substr(0, pkPos);
		if (pkCol.length() == 0) {
			// key不能为空
			cerr << "Delete error: Condition key can't be NULL." << endl;
			return false;
		}
		if (pkCol != table->column[0]) {
			// key必须是主键属性
			cerr << "Delete error: Condition key must be primary key." << endl;
			return false;
		}

		string pkValue = condition.substr(pkPos + 1, condition.length() - 1 - pkPos);
		if (pkValue.length() == 0) {
			// 主键值不能为空
			cerr << "Delete error: Primary key value can't be NULL." << endl;
			return false;
		}

		int pk = stoi(pkValue);
		if (!table->get(pk)) {
			// 主键值不存在
			cerr << "Delete error: Primary key value doesn't exist." << endl;
			return false;
		}
		// 删除
		table->remove(pk);

		// 保存修改
		if (database->saveTable(tablename)) {
			cout << "Delete from table successfully." << endl;
		}
	}
	else {
		cerr << "Delete error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// 显示当前存在的用户
bool Server::dealShowUsers() {
	for (const auto& account : accountInfo) {
		cout << account.first << endl;
	}
	return true;
}

// 查看当前数据库中所有的表
bool Server::dealShowTables() {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Show tables error: You have to select a database first." << endl;
		return false;
	}
	for (const string& tableName : database->tableNames) {
		// 去除后缀".table"
		cout << tableName.substr(0, tableName.length() - 6) << endl;
	}
	return true;
}

// 查看当前数据库中所有的视图
bool Server::dealShowViews() {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Show views error: You have to select a database first." << endl;
		return false;
	}
	for (const string& viewName : database->viewNames) {
		// 去除后缀".view"
		cout << viewName.substr(0, viewName.length() - 5) << endl;
	}
	return true;
}

// 加载账号密码信息
bool Server::loadAccountInfo() {
	ifstream file("./account.txt");

	if (file.is_open()) {
		string username, password;
		while (file >> username >> password) {
			accountInfo[username] = password;
		}
		file.close();
		// std::cout << "Account loaded successfully." << endl;
		return true;
	}
	else {
		std::cerr << "Load account error: Unable to open file ./account.txt." << endl;
		return false;
	}
}

// 保存账号密码信息
bool Server::saveAccountInfo() {
	// cout << "Size of accountInfo: " << accountInfo.size() << endl;
	ofstream file("./account.txt");

	if (file.is_open()) {
		for (const auto& account : accountInfo) {
			file << account.first << " " << account.second << "\n";
		}
		file.close();
		// cout << "Account saved successfully" << endl;
		return true;
	}
	else {
		cerr << "Load account error: Unable to open file ./account.txt." << endl;
		return false;
	}
}

// 按空格分割句子
void Server::parseSentence(const string& input, vector<string>& words) {
	istringstream ss(input);
	
	string word;
	while (ss >> word) {
		words.push_back(word);
	}
}

Server::Server() {
}

Server::~Server() {
	// cout << "Server destructor called." << endl;

	if (database != nullptr) {
		delete database;
	}
}