#ifndef SERVER_H
#define SERVER_H

#include <fstream>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <string>
#include <windows.h>
#include <shellapi.h>
#include <cstdio>
#include <filesystem>
#include <unordered_map>

#include "database.h"
#include "config.h"

using namespace std;

class Server {
	unordered_map<string, string> accountInfo;
public:
	Database* database = nullptr;

	Server();
	~Server();

	void start();
	void printPrompt();
	void parseSentence(const string&, vector<string>&);
	string md5(const string&);

	bool loadAccountInfo();
	bool saveAccountInfo();

	/* CREATE USER username PASSWORD password */
	bool dealCreateUser(const string&);					// �����û�
	/* DROP USER username PASSWORD password */
	bool dealDropUser(const string&);					// ɾ���û�
	/* USE USER database PASSWORD password */
	bool dealUseUser(const string&);					// ����ָ���û������ݿ⣩
	/* SHOW USERS */
	bool dealShowUsers();								// ��ʾ��ǰ���ڵ��û�

	/* SHOW TABLES */
	bool dealShowTables();								// �鿴��ǰ���ݿ������еı�
	/* SHOW VIEWS */		
	bool dealShowViews();								// �鿴��ǰ���ݿ������е���ͼ
	/* CREATE TABLE tablename id:INT name:STRING money:FLOAT PRIMARYKEY id */
	bool dealCreateTable(const string&);				// ����
	/* DROP TABLE tablename */
	bool dealDropTable(const string&);					// ɾ����
	/* SHOW TABLE tablename COLUMNS */
	bool dealShowColumns(const string&);				// ��ӡ�������
	/* SHOW TABLE tablename PRIMARYKEY */
	bool dealShowPrimaryKey(const string&);				// ��ӡ�������

	/* INSERT INTO TABLE tablename id:1 name:Carl money:12.5 */
	bool dealInsert(const string&);						// ��������
	/* SELECT ... FROM TABLE tablename (WHERE ...) */
	bool dealSelect(const string&);						// �鿴����
	/* DELETE FROM TABLE tablename WHERE id:1 */
	bool dealDelete(const string&);						// ɾ������

	/* SAVE TABLE tablename PATH path AS txt/csv */
	bool dealSaveTable(const string&);					// �������Ϣ��ָ����ַ

};

#endif	// SERVER_H