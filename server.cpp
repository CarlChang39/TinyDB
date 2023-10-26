#include "server.h"

void Server::start() {
	loadAccountInfo();

	while (true) {
		// ��ӡ��ʾ��Ϣ
		cout << "TinyDB > ";

		// ��ȡ�û�����
		string input;
		getline(cin, input);
		int length = input.length();

		// �����û������ݿ⣩
		if (input.starts_with("CREATE USER")) {
			dealCreateUser(input);
		}
		// ɾ���û������ݿ⣩
		else if (input.starts_with("DROP USER")) {
			dealDropUser(input);
		}
		// ��ʾ��ǰ���ڵ��û�
		else if (input == "SHOW USERS") {
			dealShowUsers();
		}
		// ����ָ���û������ݿ⣩
		else if (input.starts_with("USE USER")) {
			dealUseUser(input);
		} 

		// �鿴��ǰ���ݿ������еı�
		else if (input == "SHOW TABLES") {
			dealShowTables();
		}
		// �鿴��ǰ���ݿ������е���ͼ
		else if (input == "SHOW VIEWS") {
			dealShowViews();
		}
		// ����
		else if (input.starts_with("CREATE TABLE")) {
			dealCreateTable(input);
		}
		// ɾ����
		else if (input.starts_with("DROP TABLE")) {
			dealDropTable(input);
		}
		// ��ӡ�������
		else if (input.starts_with("SHOW TABLE")) {
			if (input.ends_with("COLUMNS")) {
				dealShowColumns(input);
			}
			else if (input.ends_with("PRIMARYKEY")) {
				dealShowPrimaryKey(input);
			}
		}
		// ��������
		else if (input.starts_with("INSERT INTO TABLE")) {
			dealInsert(input);
		}
		// �鿴����
		else if (input.starts_with("SELECT")) {
			dealSelect(input);
		}
		// ɾ������
		else if (input.starts_with("DELETE FROM TABLE")) {
			dealDelete(input);
		}

		// �˳����ݿ�
		else if (input == "EXIT") {
			cout << "Bye!" << endl;
			break;
		}
		// �Ƿ�����
		else {
			cerr << "Unrecognized command. Please check your input." << endl;
		}
	}
}

// �����½��û�����
bool Server::dealCreateUser(const string& input) {
	vector<string> words;
	parseSentence(input, words);

	if (words.size() == 5 && words[3] == "PASSWORD") {
		string username = words[2];
		if (accountInfo.count(username)) {
			// �û��Ѵ���
			cerr << "Create user error: User already exist." << endl;
			return false;
		} 
		
		// �����û���������Ϣ
		string password = words[4];
		accountInfo[username] = password;
		saveAccountInfo();
		cout << "User created successfully." << endl;

		// �½����ݿ�·��
		string path = "database\\" + username;

		// �� std::string ת��Ϊ���ַ��ַ���
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, NULL, 0);
		wstring wideFolderPath(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, &wideFolderPath[0], size_needed);
		// ʹ��CreateDirectory��������Ŀ¼
		if (CreateDirectory(wideFolderPath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
			cout << "User database created successfully." << endl;
		}
		else {
			cerr << "Create user database error: Error creating folder." << endl;
			return false;
		}
	}
	else {
		// SQL����ʽ����ȷ
		cerr << "Create user error: Invalid syntex." << endl;
		return false;
	}
	
	return true;
}

// ����ɾ���û�����
bool Server::dealDropUser(const string& input) {
	vector<string> words;
	parseSentence(input, words);

	if (words.size() == 5 && words[3] == "PASSWORD") {
		string username = words[2];
		if (!accountInfo.count(username)) {
			// �û�������
			cerr << "Drop user error: User doesn't exist." << endl;
			return false;
		}

		string password = words[4];
		if (password != accountInfo[username]) {
			// ���벻��ȷ
			cerr << "Drop user error: Wrong password." << endl;
			return false;
		}
		// ɾ���û�
		accountInfo.erase(username);
		saveAccountInfo();
		cout << "Drop user successfully." << endl;

		// ɾ�����ݿ�
		string path = "database\\" + username;
		// �� std::string ת��Ϊ���ַ��ַ���
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

		// ���ɾ�����ǵ�ǰ�û����ͷ�database��ָ��nullptr
		if (database != nullptr && username == database->owner) {
			delete database;
			database = nullptr;
		}
	}
	else {
		// SQL����ʽ����ȷ
		cerr << "Drop user error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// ִ������ָ���û������ݿ⣩
bool Server::dealUseUser(const string& input) {
	vector<string> words;
	parseSentence(input, words);

	if (words.size() == 5 && words[3] == "PASSWORD") {
		string username = words[2];

		if (!accountInfo.count(username)) {
			// �û�������
			cerr << "Use user error: User doesn't exist." << endl;
			return false;
		}

		if (database != nullptr && username == database->owner) {
			// Ҫ���õ��û����ǵ�ǰ�û�
			cerr << "Use user error: You are already using this user." << endl;
			return false;
		}

		string password = words[4];
		if (password != accountInfo[username]) {
			// ���벻��ȷ
			cerr << "Use user error: Wrong password." << endl;
			return false;
		}

		// �л����ݿ�
		delete database;
		database = new Database(username);
		cout << "Using user " << username << endl;
	}
	else {
		// SQL����ʽ����ȷ
		cerr << "Use user error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// ����
bool Server::dealCreateTable(const string& input) {
	if (database == nullptr) {
		// δѡ�����ݿ�
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
			// ���������еı��ظ�
			cerr << "Create table error: Table name already exists." << endl;
			return false;
		}

		// ��������
		string primaryKey = words[size - 1];

		// �����û���Ҫ�����dataType��column��ͬʱ�������Ե��洢�±��ӳ��
		vector<string> dataTypeString;
		vector<string> column;
		unordered_map<string, int> column2index;
		for (int i = 3; i < size - 2; i++) {
			size_t pos = words[i].find(':');
			if (pos == string::npos) {
				// ���Զ���ĸ�ʽ����ȷ
				cerr << "Create table error: Invalid column definition." << endl;
				return false;
			}
			
			string col = words[i].substr(0, pos);
			if (col.length() == 0) {
				// �������Ʋ���Ϊ��
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

		// �涨�����ݿ���������ڵ�һλ
		auto it = find(column.begin(), column.end(), primaryKey);
		if (it == column.end()) {
			// û���ҵ��������������ԣ�������
			cerr << "Create table error: Wrong primary key name." << endl;
			return false;
		}
		// �ӵ�����ת��Ϊ�±�
		size_t index = distance(column.begin(), it);
		string tmpCol = column[0];
		swap(column[index], column[0]);
		swap(dataTypeString[index], dataTypeString[0]);
		column2index[tmpCol] = index;
		column2index[primaryKey] = 0;

		// �������ļ���д���ͷ��Ϣ
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
		// ����ָ���������ұ���Ϊint���ͣ�
		cerr << "Create table error: Primary key must be assigned (int type only)." << endl;
		return false;
	}
	return true;
}

// ɾ����
bool Server::dealDropTable(const string& input) {
	if (database == nullptr) {
		// δѡ�����ݿ�
		cerr << "Drop table error: You have to select a database first." << endl;
		return false;
	}

	vector<string> words;
	parseSentence(input, words);

	if (words.size() == 3) {
		string tablename = words[2];
		if (!database->tableNames.count(tablename + ".table")) {
			// ����������
			cerr << "Drop table error: Wrong table name." << endl;
			return false;
		}
		if (database->openedTables.count(tablename)) {
			// �����δ�رգ�����
			database->saveTable(tablename);
		}

		// ɾ����ʹ��remove������
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

// ��ӡ�������
bool Server::dealShowColumns(const string& input) {
	if (database == nullptr) {
		// δѡ�����ݿ�
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

	// �ͷſռ�
	delete table;

	// ��openedTables��ɾ���ñ�
	database->openedTables.erase(tablename + ".table");

	return true;
}

// ��ӡ�������
bool Server::dealShowPrimaryKey(const string& input) {
	if (database == nullptr) {
		// δѡ�����ݿ�
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

	// �ͷſռ�
	delete table;

	// ��openedTables��ɾ���ñ�
	database->openedTables.erase(tablename + ".table");

	return true;
}

// ��������
bool Server::dealInsert(const string& input) {
	if (database == nullptr) {
		// δѡ�����ݿ�
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
		// ���������ͱ���������������
		cerr << "Insert into table error: Wrong insert value amount." << endl;
		return false;
	}

	vector<string> row(table->numCols, "");
	for (int i = 4; i < size; i++) {
		size_t pos = words[i].find(':');
		if (pos == string::npos) {
			// ���Ը�ֵ�ĸ�ʽ����ȷ
			cerr << "Insert into table error: Invalid assignment." << endl;
			return false;
		}

		string col = words[i].substr(0, pos);
		if (col.length() == 0) {
			// �������Ʋ���Ϊ��
			cerr << "Insert into table error: Column value can't be NULL." << endl;
			return false;
		}

		string value = words[i].substr(pos + 1, words[i].length() - 1 - pos);
		if (value.length() == 0) {
			// ����ֵ����Ϊ��
			cerr << "Insert into table error: Insert value can't be NULL." << endl;
			return false;
		}
		
		/* TODO: ����Ӧ���ݸ�ʽ�Ƿ���ȷ */

		row[table->column2index[col]] = value;
	}

	if (!table->insert(stoi(row[0]), row)) {
		// ����ֵ�ظ�
		cerr << "Insert into table error: Duplicated primary key." << endl;
		return false;
	}

	// �����޸�
	if (database->saveTable(tablename)) {
		cout << "Insert into table successfully." << endl;
	}

	return true;
}

bool Server::dealSelect(const string& input) {
	if (database == nullptr) {
		// δѡ�����ݿ�
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
			// �鿴���е�ȫ������
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

		// �ͷſռ�
		delete table;

		// ��openedTables��ɾ���ñ�
		database->openedTables.erase(tablename + ".table");
	}
	else {
		cerr << "Select error: Invalid syntex." << endl;
		return false;
	}

	return true;
}

// ɾ������
bool Server::dealDelete(const string& input) {
	if (database == nullptr) {
		// δѡ�����ݿ�
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

		// ��������Ƿ���ȷ
		size_t pkPos = condition.find(':');
		if (pkPos == string::npos) {
			// ���Ը�ֵ�ĸ�ʽ����ȷ
			cerr << "Delete error: Invalid condition." << endl;
			return false;
		}

		string pkCol = condition.substr(0, pkPos);
		if (pkCol.length() == 0) {
			// key����Ϊ��
			cerr << "Delete error: Condition key can't be NULL." << endl;
			return false;
		}
		if (pkCol != table->column[0]) {
			// key��������������
			cerr << "Delete error: Condition key must be primary key." << endl;
			return false;
		}

		string pkValue = condition.substr(pkPos + 1, condition.length() - 1 - pkPos);
		if (pkValue.length() == 0) {
			// ����ֵ����Ϊ��
			cerr << "Delete error: Primary key value can't be NULL." << endl;
			return false;
		}

		int pk = stoi(pkValue);
		if (!table->get(pk)) {
			// ����ֵ������
			cerr << "Delete error: Primary key value doesn't exist." << endl;
			return false;
		}
		// ɾ��
		table->remove(pk);

		// �����޸�
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

// ��ʾ��ǰ���ڵ��û�
bool Server::dealShowUsers() {
	for (const auto& account : accountInfo) {
		cout << account.first << endl;
	}
	return true;
}

// �鿴��ǰ���ݿ������еı�
bool Server::dealShowTables() {
	if (database == nullptr) {
		// δѡ�����ݿ�
		cerr << "Show tables error: You have to select a database first." << endl;
		return false;
	}
	for (const string& tableName : database->tableNames) {
		// ȥ����׺".table"
		cout << tableName.substr(0, tableName.length() - 6) << endl;
	}
	return true;
}

// �鿴��ǰ���ݿ������е���ͼ
bool Server::dealShowViews() {
	if (database == nullptr) {
		// δѡ�����ݿ�
		cerr << "Show views error: You have to select a database first." << endl;
		return false;
	}
	for (const string& viewName : database->viewNames) {
		// ȥ����׺".view"
		cout << viewName.substr(0, viewName.length() - 5) << endl;
	}
	return true;
}

// �����˺�������Ϣ
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

// �����˺�������Ϣ
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

// ���ո�ָ����
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