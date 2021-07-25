/*
Program Name: openFacts
Author: Levi George
Date Last Updated: 11/8/2019
Program Purpose: Open a database and run an SQL query on it.
*/

#include "sqlite3.h"
#include <iostream>
#include <string>

using namespace std;

//call back function to be sent to the sqlite3_exec function
int callback(void *data, int numOfCols, char **ColumnValue, char **ColName);

int main()
{
	sqlite3 *Facts; //database handle
	int rc; //means return code
	char * messageError;//this will hold the error message
	const char* data = "Callback function called";//this will be passed to a function.
	
	//This function call opens the database
	rc = sqlite3_open_v2("factdb.db", &Facts, SQLITE_OPEN_READWRITE, "win32");
	
	//runs based on the return code of the open function
	if(rc)
	{	//displays error message if the rc contains anything
		cout << "Failure to Open Database: " << sqlite3_errmsg(Facts) << endl;
		return 0;
	}
	else//if the rc contains nothing then this will display a message 
	{//indicating the success of the open function
	
		cout << "Database Opened." << endl;
	}
	
	//this will open the database and run the query on it, the callback will be used to 
	//print information from the query
	rc = sqlite3_exec(Facts, "SELECT * FROM CHECKOUT;",callback,(void*) data, &messageError);
	
	//This will run based on the return code of the execution function, it tells the user whether or not it was successful
	if(rc != SQLITE_OK)
	{
		//tells the user if the execution function was unsuccessful
		cout << "There was an error retrieving data from the database " << messageError << endl;
		sqlite3_free(messageError);
	}
	else
	{
		//tells the user if the function was successful
		cout << "Called Select" << endl;
	}
	
	//closes function
	rc = sqlite3_close(Facts);
	
	//this if statement will run to ensure that the user knows whether or not the function
	//to close the database was successful
	if(rc != SQLITE_OK)
	{
		cout << "Closure of database has failed\n";
	}
	else
	{
		cout << "Successfully closed Database\n";
	}
	
	return 0;
}

//this function will be passed to the exec function and will print the information of 
//the rows that are returned
int callback(void *data, int numOfCols, char **ColumnValue, char **ColName)
{
	//runs based on the number of columns in the table
	for (int i = 0; i < numOfCols; i++)
	{
		//I found out the hard way that if a row has a null then it will 
		//fail to print, because I'm guessing that it will only initialize the length to the number of non-null
		//columns, thus by trying to grab from the sixth item in that array we break the program due to an out-of-range error
		
		//if any column is null, outputs null otherwise it will output regularly
		if(ColumnValue[i] == NULL)
		{
			cout << ColName[i] << ": NULL" << endl;
		}
		else
		{
			cout << ColName[i] << ": " << ColumnValue[i] << endl;
		}
	}
	cout << endl;
	
	return 0;
}