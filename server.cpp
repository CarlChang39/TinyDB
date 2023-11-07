#include "server.h"

void Server::start() {
	// 加载用户名-密码文件
	loadAccountInfo();
	string buffer = "";
	//不输出error之外的信息
	utils::logging::setLogLevel(utils::logging::LOG_LEVEL_ERROR);//静默 utils::logging::LOG_LEVEL_SILENT

	while (true) {
		// 打印提示信息
		printPrompt();

		// 获取用户输入
		string input;
		getline(cin, input);
		size_t length = input.length();

		if (!input.ends_with(";")){
			// 存入缓存
			buffer = buffer + input;
			// 限制长度
			if (buffer.size() > 1000) {
				cerr << "Command is too long and must end with ; Please check your input." << endl;
				buffer = "";
			}
			continue;
		}
		else {
			// 存入缓存
			input = buffer + input.substr(0, length-1);
			// 清空缓存
			buffer = "";
		}

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
		// 保存数据
		else if (input.starts_with("SAVE TABLE")) {
			dealSaveTable(input);
		}
		// 导入数据
		else if (input.starts_with("IMPORT TABLE")) {
			dealImportTable(input);
		}
		// 查看图片
		else if (input.starts_with("SHOW PICTURE")) {
			dealShowPicture(input);
		}
		// 查看流媒体
		else if (input.starts_with("SHOW VIDEO")) {
			dealShowVideo(input);
		}
		else if (input == "HELP") {
			dealHelp();
		}
		// 退出数据库
		else if (input == "EXIT") {
			std::cout << "Bye!" << endl;
			break;
		}
		// 非法输入
		else {
			cerr << "Unrecognized command. Please check your input." << endl;
		}
	}
}

void Server::printPrompt() {
	if (database != nullptr) {
		cout << database->owner << " > ";
	}
	else {
		cout << "TinyDB > ";
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
		string password = sha256(words[4]);
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

		string password = sha256(words[4]);
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

		string password = sha256(words[4]);
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
	size_t size = words.size();
	
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
		unordered_map<string, size_t> column2index;
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
	size_t size = words.size();

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
	size_t size = words.size();

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
	size_t size = words.size();

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

// 保存表信息到指定地址
bool Server::dealSaveTable(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Save table error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);
	size_t size = words.size();

	if (size == 7 && words[3] == "PATH" && words[5] == "AS") {
		string tablename = words[2];
		string path = words[4];
		string format = words[6];

		if (!filesystem::exists(path)) {
			cerr << "Save table error: Save path doesn't exist." << endl;
			return false;
		}
		if (format != "txt" && format != "csv") {
			cerr << "Save table error: Table can only be saved as txt or csv." << endl;
			return false;
		}
		if (!database->loadTable(tablename)) {
			cerr << "Save table error: Failed loading table." << endl;
			return false;
		}

		Table* table = database->openedTables[tablename + ".table"];

		vector<vector<string>> rows;
		table->getRows(rows);

		// 保存到目标路径
		if (!path.ends_with("\\")) {
			path = path + "\\";
		}
		ofstream outputFile(path + tablename + "." + format);
		char delimiter;
		if (format == "txt")
			delimiter = ' ';
		else
			delimiter = ',';
		if (outputFile.is_open()) {
			for (size_t i = 0; i < table->column.size(); i++) {
				outputFile << table->column[i];
				if (i < table->column.size() - 1) {
					outputFile << delimiter;
				}
			}
			outputFile << "\n";
			for (const vector<string>& row : rows) {
				for (size_t i = 0; i < row.size(); i++) {
					outputFile << row[i];
					if (i < row.size() - 1) {
						outputFile << delimiter;
					}
				}
				outputFile << "\n";
			}

			outputFile.close();

			cout << "Table saved to path " << path + tablename + "." + format << " successfully." << endl;

			// 释放空间
			delete table;

			// 从openedTables里删除该表
			database->openedTables.erase(tablename + ".table");
		}
		else {
			cerr << "Save table error: Unable to open table file." << endl;

			// 释放空间
			delete table;

			// 从openedTables里删除该表
			database->openedTables.erase(tablename + ".table");

			return false;
		}
	}
	else {
		cerr << "Save table error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// 导出数据
bool Server::dealImportTable(const string& input){
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Save table error: You have to select a database first." << endl;
		return false;
	}
	vector<string> words;
	parseSentence(input, words);
	size_t size = words.size();

	if (size == 7 && words[3] == "PATH" && words[5] == "BY") {
		string tablename = words[2];
		string path = words[4];
		string format = words[6];

		if (!filesystem::exists(path)) {
			cerr << "Import table error: Import path doesn't exist." << endl;
			return false;
		}
		if (format != "txt" && format != "csv") {
			cerr << "Import table error: Table can only be import by txt or csv." << endl;
			return false;
		}
		// 按行将文本读取到lines中
		std::vector<std::string> lines; // 存储每行的字符串
		std::ifstream inputFile(path);
		if (inputFile.is_open()) {
			std::string line;
			while (std::getline(inputFile, line)) {
				lines.push_back(line); // 将每行添加到字符串向量中
			}
			inputFile.close(); // 关闭文件
		}
		else {
			std::cerr << "Import table error: File can't be opened" << std::endl;
			return false;
		}

		// 分隔符
		char delimiter;
		if (format == "txt")
			delimiter = ' ';
		else
			delimiter = ',';

		// 第一行为列名
		string column;
		std::vector<std::string> columns;
		parseSentence(lines[0], columns, delimiter);
		size_t num_col = columns.size();
		//遍历每一行,从第二行开始
		for (size_t i = 1; i < lines.size(); i++) {
			//分割
			std::vector<std::string> row;
			parseSentence(lines[i], row, delimiter);
			if (row.size() != num_col) {
				std::cerr << "Import table error: Wrong number of columns" << std::endl;
				return false;
			}
			//格式INSERT INTO TABLE t_nzx id:1 name:NZX;
			string sql = "INSERT INTO TABLE " + tablename;
			for (size_t j = 0; j < num_col; j++) {
				sql += " " + columns[j] + ":" + row[j];
			}
			dealInsert(sql);
		}
		std::cout << "Import TABLE " + tablename + " from " + path + " format " + format + " sucessfully." << endl;
	}
	else {
		cerr << "Save table error: Invalid syntex." << endl;
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

// 查看图片
bool Server::dealShowPicture(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Show picture error: You have to select a database first." << endl;
		return false;
	}
	
	vector<string> words;
	parseSentence(input, words);
	size_t size = words.size();

	if (size == 3) {
		string filename = words[2];
		string filePath = "database\\" + database->owner + "\\" + filename;

		if (!filesystem::exists(filePath)) {
			cerr << "Show picture error: File doesn't exist." << endl;
			return false;
		}
		// 创建类对象 COpenCVWindowExt
		COpenCVWindowExt img_window("img");
		img_window.ImRead(filePath);
		waitKey(0);
		//按任意键关闭窗口
		destroyAllWindows();
	}
	else {
		cerr << "Show picture error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// 查看流媒体
bool Server::dealShowVideo(const string& input) {
	if (database == nullptr) {
		// 未选定数据库
		cerr << "Show video error: You have to select a database first." << endl;
		return false;
	}
	
	vector<string> words;
	parseSentence(input, words);
	size_t size = words.size();

	if (size == 3) {
		string filename = words[2];
		string filePath = "database\\" + database->owner + "\\" + filename;

		// 打开视频
		VideoCapture cap(filePath);
		// 检查视频是否成功打开
		if (!cap.isOpened()) {
			cerr << "Show video error: Could not open or find the video" << endl;
			return false;
		}

		// 读取和显示视频帧
		Mat frame;
		bool isPaused = false;
		while (true) {
			if (!isPaused)
			{
				cap.read(frame);
				if (frame.empty()) {
					break;
				}
				imshow("Video", frame);
			}
				
			// 按下空格暂停
			auto key = waitKey(30);
			if (key == 32)
				isPaused = !isPaused;
			// 按下ESC键退出
			else if (key == 27)
				break;
		}

		// 关闭视频流
		cap.release();
		destroyAllWindows();
	}
	else {
		cerr << "Show video error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

//显示帮助
bool Server::dealHelp() {
	std::cout << "Welcome to use our database system!" << endl;
	std::cout << "You can use the following commands to operate the database system." << endl;
	std::cout << "1. CREATE USER username PASSWORD password;" << endl;
	std::cout << "2. DROP USER username PASSWORD password;" << endl;
	std::cout << "3. USE USER username PASSWORD password;" << endl;
	std::cout << "4. CREATE TABLE tablename col1:dataType col2:dataType ... PRIMARYKEY primaryKey;" << endl;
	std::cout << "5. DROP TABLE tablename;" << endl;
	std::cout << "6. SHOW TABLE tablename COLUMNS;" << endl;
	std::cout << "7. SHOW TABLE tablename PRIMARYKEY;" << endl;
	std::cout << "8. INSERT INTO TABLE tablename col1:value col2:value ...;" << endl;
	std::cout << "9. SELECT ... FROM TABLE tablename (WHERE ...);" << endl;
	std::cout << "10. DELETE FROM TABLE tablename WHERE primaryKey:value;" << endl;
	std::cout << "11. SAVE TABLE tablename PATH path AS format;" << endl;
	std::cout << "12. IMPORT TABLE tablename PATH path BY format;" << endl;
	std::cout << "13. SHOW USERS;" << endl;
	std::cout << "14. SHOW TABLES;" << endl;
	std::cout << "15. SHOW VIEWS;" << endl;
	std::cout << "16. SHOW PICTURE filename;" << endl;
	std::cout << "17. SHOW VIDEO filename;" << endl;
	std::cout << "18. EXIT;" << endl;
	std::cout << "19. HELP;" << endl;
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

// 按分隔符分割句子
void Server::parseSentence(const string& input, vector<string>& words, const char& delimiter) {
	istringstream ss(input);
	string word;
	while (std::getline(ss, word, delimiter)) {
		words.push_back(word);
	}
}

string Server::md5(const string& password) {
	std::string digest;
	CryptoPP::Weak1::MD5 md5;
	CryptoPP::HashFilter hashfilter(md5);
	hashfilter.Attach(new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest), false));
	hashfilter.Put(reinterpret_cast<const unsigned char*>(password.c_str()), password.length());
	hashfilter.MessageEnd();
	return digest;
}

// 将输入的字符串通过SHA-256算法加密
string Server::sha256(const string& password) {
	std::string digest;
	// 创建SHA-256哈希对象
	CryptoPP::SHA256 sha256;
	// 计算哈希值
	CryptoPP::byte hash[CryptoPP::SHA256::DIGESTSIZE];
	sha256.CalculateDigest(hash, reinterpret_cast<CryptoPP::byte*>(const_cast<char*>(password.c_str())), password.length());
	// 使用HexEncoder过滤器将哈希值转换为十六进制字符串
	CryptoPP::HexEncoder encoder;
	encoder.Attach(new CryptoPP::StringSink(digest));
	encoder.Put(hash, sizeof(hash));
	encoder.MessageEnd();
	return digest;
}

Server::Server() {
	wstring databasePath = L"database";
	if (!filesystem::exists(databasePath)) {
		if (CreateDirectory(databasePath.c_str(), NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError()) {
			//cout << "Database home path created successfully." << endl;
		}
		else {
			cerr << "Server init error: Can't create database home path." << endl;
		}
	}
}

Server::~Server() {
	// cout << "Server destructor called." << endl;

	if (database != nullptr) {
		delete database;
	}
}