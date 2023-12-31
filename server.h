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
#include "OpenCVWindowExt.h"
#include "config.h"
#include "md5.h"
#include "hex.h"
#include "sha.h"
#include "filters.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utils/logger.hpp"

using namespace std;
using namespace CryptoPP;
using namespace cv;

class Server {
	unordered_map<string, string> accountInfo;
public:
	Database* database = nullptr;

	Server();
	~Server();

	void start();
	void printPrompt();
	void parseSentence(const string&, vector<string>&);
	void parseSentence(const string&, vector<string>&, const char&);
	string md5(const string&);
	string sha256(const string&);
	
	bool loadAccountInfo();
	bool saveAccountInfo();

	/* CREATE USER username PASSWORD password */
	bool dealCreateUser(const string&);					// 创建用户
	/* DROP USER username PASSWORD password */
	bool dealDropUser(const string&);					// 删除用户
	/* USE USER database PASSWORD password */
	bool dealUseUser(const string&);					// 启用指定用户（数据库）
	/* SHOW USERS */
	bool dealShowUsers();								// 显示当前存在的用户

	/* SHOW TABLES */
	bool dealShowTables();								// 查看当前数据库中所有的表
	/* SHOW VIEWS */		
	bool dealShowViews();								// 查看当前数据库中所有的视图
	/* CREATE TABLE tablename id:INT name:STRING money:FLOAT PRIMARYKEY id */
	bool dealCreateTable(const string&);				// 建表
	/* DROP TABLE tablename */
	bool dealDropTable(const string&);					// 删除表
	/* SHOW TABLE tablename COLUMNS */
	bool dealShowColumns(const string&);				// 打印表的属性
	/* SHOW TABLE tablename PRIMARYKEY */
	bool dealShowPrimaryKey(const string&);				// 打印表的主键

	/* INSERT INTO TABLE tablename id:1 name:Carl money:12.5 */
	bool dealInsert(const string&);						// 插入数据
	/* SELECT ... FROM TABLE tablename (WHERE ...) */
	bool dealSelect(const string&);						// 查看数据
	/* DELETE FROM TABLE tablename WHERE id:1 */
	bool dealDelete(const string&);						// 删除数据

	/* SAVE TABLE tablename PATH path AS txt/csv */
	bool dealSaveTable(const string&);					// 保存表信息到指定地址
	/* IMPORT TABLE tablename PATH path BY txt/csv */
	bool dealImportTable(const string& input);			// 导入表信息
	/* SHOW PICTURE pictureName */
	bool dealShowPicture(const string&);				// 查看图片
	/* SHOW VIDEO videoName */
	bool dealShowVideo(const string&);					// 查看流媒体
	/* HELP*/
	bool dealHelp();						// 帮助
};

#endif	// SERVER_H