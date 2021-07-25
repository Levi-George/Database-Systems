/*
Program Name:
Author:
Date Last Updated:
Program Purpose:
*/

#include "sqlite3.h"
#include <iostream>
#include <string>

using namespace std;

static int callback(void *data, int argc, char **argv, char ** azColName);

int main()
{
	sqlite3 *ConstructCo; //database handle
	int rc; //means return code
	char * messageError;
	int rows, cols;
	char ** result;
	const char* data = "Callback function called";
	
	
	rc = sqlite3_open_v2("ConstructCo.db", &ConstructCo, SQLITE_OPEN_READWRITE, "win32");
	
	if(rc)
	{
		cout << "Error opening database: " << sqlite3_errmsg(ConstructCo) << endl;
		return 0;
	}
	else
	{
		cout << "Database Opened Successfully" << endl;
	}
	
	rc = sqlite3_exec(ConstructCo, "Select * from employee;", callback,(void*) data, &messageError);
	
	if(rc != SQLITE_OK)
	{
		cout << "There was an error retrieving data from the database " << messageError << endl;
		sqlite3_free(messageError);
	}
	else
	{
		cout << "Called Select" << endl;
	}
	
	
	return 0;
}

int callback(void *data, int argc, char **argv, char **azColName)
{
	int i;
	static int count = 0;
	for (i = 0; i < argc; i++)
	{
		cout << azColName[i] << ": " << argv[i] << endl;
	}
	cout << endl;
	
	return 0;
}