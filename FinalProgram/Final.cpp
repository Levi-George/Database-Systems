/*
Program Name: Final Project
Author: Levi George
Date Last Updated: 12/13/2019
Program Purpose: To allow a user to run a series of operations on a database
Program Notes:

You might notice that the code has a few "variableName--;" statements in it, this is to make up 
for a difference between the counter starting at i = 1, and the database entries beginning at 0

Table Of Contents
Line 23 ... Declarations, Libraries, namespace, and function prototypes
Line 71 ... Main
Line 148 ... Addition functtions
Line 537 ... Delete functions
Line 707 ... Update functions
Line 1042 ... Transaction 
Line 1209 ... View Data functions
Line 1449 ... validate function
Line 1472 ... Rollback function
*/

#include <iostream>
#include <string>
#include <iomanip>
#include "sqlite3.h"

using namespace std;

//menu based functions
void printMainMenu();
int mainMenu();
void printAddMenu();
int addMenu();
void printDeleteMenu();
int deleteMenu();
void printUpdateMenu();
int updateMenu();

//addition functions
void addPath(sqlite3 *);
void addSubject(sqlite3 *);
void addHerd(sqlite3 *);

//delete functions
void deletePath(sqlite3 *);
void deleteSubject(sqlite3 *);
void deleteHerd(sqlite3 *);

//update functions
void updatePath(sqlite3 *);
void updateSubject(sqlite3 *);
void updateCastle(sqlite3 *);
void updateHerd(sqlite3 *);

//transaction and retrieval
void transaction(sqlite3 *);

//view data functions
void viewData(sqlite3 *);
void viewCountry(sqlite3 *);
void viewCastle(sqlite3 *);
void viewSubject(sqlite3 *);

//validate input
int validate(int, int);

//will be used to rollback failed transactions
void ROLLBACK(sqlite3 *);

int main()
{
	int choice, rc;//declares user choice and return code (rc) variables
	
	sqlite3 *mydb;//declares database
	
	rc = sqlite3_open_v2("FinalHolds.db", &mydb, SQLITE_OPEN_READWRITE, "win32");//opens database
	
	if(rc)
	{
		cout << "\nDatabase failed to open\n";
		cout << "Closing program\n";
		return 0;
	}
	else
	{	
		cout << "\nDatabase opened successfully\n";
	}
	
	
	//creates program menu
	cout << "Welcome to the Final" << endl;
	choice = mainMenu();
	while (true)
	{
		//runs functions based on user choice
		switch (choice) 
		{
			case 1: addPath(mydb); break;
			case 2: deletePath(mydb); break;
			case 3: updatePath(mydb); break;
			case 4: transaction(mydb); break;
			case 5: viewData(mydb); break;
			case -1: return 0;
			default: cout << "That is not a valid choice." << endl;
		}
		cout << "\n\n";
		choice = mainMenu();
	}
	
	
	//closes function
	rc = sqlite3_close(mydb);
	
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
	
}
int mainMenu()
{
	int choice = 0;//holds user decision
	
	printMainMenu();//calls function
	cin >> choice;//gets input
	
	//validates input
	while ((!cin || choice < 1 || choice > 5) && choice  != -1)
	{		
		if (!cin)
		{
			cin.clear();
			cin.ignore(1000,'/n');
		}
		cout << "That is not a valid choice." << endl << endl;
		printMainMenu();
		cin >> choice;
	} 
	return choice;
}
//outputs menu
void printMainMenu() 
{
	cout << "\nPlease choose an option (enter -1 to quit):  " << endl;
	cout << "1. Add entries to tables" << endl;
	cout << "2. Delete entries from tables" << endl;
	cout << "3. Update entries from tables" << endl;
	cout << "4. Perform a Trade" << endl;
	cout << "5. View information" << endl;
	cout << "Enter Choice: ";
}

//functional add menu
void addPath(sqlite3 * mydb)
{
	int choice = 0;
	cout << "\n\nWhat would you like to do?" << endl;
	choice = addMenu();
	while (true)
	{
		//runs functions based on user choice
		switch (choice) 
		{
			case 1: addSubject(mydb); break;
			case 2: addHerd(mydb); break;
			case -1: return ;
			default: cout << "That is not a valid choice." << endl;
		}
		cout << "\n\n";
		choice = addMenu();
	}
}
//additional menu protects from bad input and calls functional add menu
int addMenu()
{
	int choice = 0;//holds user decision
	
	printAddMenu();//calls function
	cin >> choice;//gets input
	
	//validates input
	while ((!cin || choice < 1 || choice > 2) && choice  != -1)
	{		
		if (!cin)
		{
			cin.clear();
			cin.ignore(1000,'/n');
		}
		cout << "That is not a valid choice." << endl << endl;
		printAddMenu();
		cin >> choice;
	} 
	return choice;
}

//outputs addition menu
void printAddMenu()
{
	cout << "\nPlease choose an option (enter -1 to quit):  " << endl;
	cout << "1. Add Subject" << endl;
	cout << "2. Add Herd" << endl;
	cout << "Enter Choice: ";
}
//Adds a subject
//Complete
void addSubject(sqlite3 * db)
{
	//self-descriptive sql strings
	string sqlSelectCastle, sqlSelectRegion, sqlSelectProf, sqlCountSubs;
	string sqlInsertSubject;
	string castleNumbers;

	//sql statement
	sqlSelectRegion = "SELECT REG_NAM FROM REGION GROUP BY REG_NUM";

		
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//Will be used to hold info for final sqlInsertSubject sql
	int choice = 0;
	int custID = 0;
	int reg, cast, prof;
	string region;
	string professionName;
	
	//sql statement
	sqlCountSubs = "SELECT COUNT(SUB_NUM) FROM SUBJECT;";
	
	//checks if sql for region is valid, continues as needed if so, otherwise it will display an error message
	if(sqlite3_prepare_v2(db, sqlCountSubs.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//runs sql CountSubs, gets value of column, clears memory leaks
		sqlite3_step(prepState);
		custID = atoi(reinterpret_cast<const char*>(sqlite3_column_text(prepState, 0)));
		sqlite3_finalize(prepState);
		
	}
	
	
	//checks if sql for region is valid, continues as needed if so, otherwise it will display an error message
	if(sqlite3_prepare_v2(db, sqlSelectRegion.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//counter
		int i = 1;
		
		//prompts user
		cout << "What region does the subject belong to?\n";
		
		//prints options
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << "\n";
			i++;
		}
	
		//accepts input
		cin >> reg;
		
		//validates input
		reg = validate(reg, i);
		
		//closes resource leaks
		sqlite3_finalize(prepState);
	}
	
	reg--;//Makes up for the i = 1 discrepancy, since database entries start at 0
	
	//If nomadic, then user can live in any castle, otherwise they can only live in specific castles at first
	if(reg == 6)
	{
		sqlSelectCastle = "SELECT CSTL_NAM, CSTL_NUM FROM CASTLE GROUP BY CSTL_NUM;";
	}
	else
	{
		sqlSelectCastle = "SELECT CSTL_NAM, CSTL_NUM FROM CASTLE WHERE CSTL_CNTRY = " + to_string(reg) + "; ";
	}
	
	//checks if sql for region is valid, continues as needed if so, otherwise it will display an error message
	if(sqlite3_prepare_v2(db, sqlSelectCastle.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		int i = 1;//counter
		
		//prompting for information
		cout << "What castle is the subject in currently?\n";
		
		//outputs information from sql
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << "\n";
			castleNumbers.append(reinterpret_cast<const char*>(sqlite3_column_text(prepState, 1)));
			i++;
		}
		
		
		//obtains input
		cin >> cast;
		
		//validates input and clear resource leaks
		cast = validate(cast, i);
		sqlite3_finalize(prepState);
	}
	
	cast--;//sql database starts at zero, this clears discrepancy since the counter starts at 1
	
	//sql statement
	sqlSelectProf = "SELECT PROF_NAME FROM PROFESSION GROUP BY PROF_NUM;";
	
	//runs sql if valid, otherwise exits function
	if(sqlite3_prepare_v2(db, sqlSelectProf.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		int i = 1;//counter
		
		cout << "What is the subject's current role?\n";//output
			
		//outputs sql results
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << "\n";
			i++;
		}
	
		//gets user input
		cin >> prof;
		
		//validates and closes resource leak
		prof = validate(prof, i);
		sqlite3_finalize(prepState);
	}
	
	prof--;//database - counter discrepancy clear

	//holds user input
	string fname, lname;
	int age = 0;
		
	//prompts for input
	cout << "How old is the subject? (subjects may not be older than 100)\n";
	cin >> age;
		
	//validates age
	age = validate(age, 100);
	
	//clears buffer
	cin.clear();
	cin.ignore(1000, '\n');
	
	//gets subject name
	cout << "What is the subject's name?\n";
	getline(cin, fname);
	cout << "What is the subject's last name?\n";
	getline(cin, lname);
	
	//builds insert statement
	sqlInsertSubject = "INSERT INTO SUBJECT VALUES(" + to_string(custID) + ", \"" + fname + "\", \"" + lname + "\", " + to_string(age);
	sqlInsertSubject += ", " + to_string(prof) + ", " + castleNumbers.at(cast) + ", " + to_string(reg) + ", " + to_string(reg) + ");";
	//runs statement if valid
	if(sqlite3_prepare_v2(db, sqlInsertSubject.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);
	}
	
}
//adds herd entry to herd table
void addHerd(sqlite3 * db)
{
	//sql statements used for function
	string sqlCountHerd, sqlInsertHerd;
	string sqlGetRegion;
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//builds sql statements
	sqlCountHerd = "SELECT COUNT(HERD_NUM) FROM HERD;";
	sqlInsertHerd = "INSERT INTO HERD VALUES(\""; //will need to add herd number, size, region, and species
	sqlGetRegion = "SELECT REG_NAM FROM REGION WHERE NOT REG_NUM = 5;";
	string sqlGetTera = "SELECT REG_TERRAIN FROM REGION WHERE REG_NUM = ";
	
	//used to hold user input
	int reg = 0, herdnum = 0, herdsize = 0;
	string terrain[3] = {"Forest", "Mountainous", "Seaside"};//used in naming species
	string chosen;
		
	//runs sql
	if(sqlite3_prepare_v2(db, sqlCountHerd.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{	//run sql, get value from result, clears resources
		sqlite3_step(prepState);
		herdnum = atoi(reinterpret_cast<const char*>(sqlite3_column_text(prepState, 0)));
		sqlite3_finalize(prepState);
	}
	
	//prompts user
	cout << "Where was the herd sighted?\n";
	
	//runs sql if valid
	if(sqlite3_prepare_v2(db, sqlGetRegion.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		int i = 1;//counter

		//outputs sql results
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << "\n";
			i++;
		}
		
		//gets input from user
		cin >> reg;
		
		//validates, clears resource leaks, and fixes discrepancy
		reg = validate(reg, i);
		sqlite3_finalize(prepState);
		reg--;
	}
	
	//prompts user for output and gets, validates user input
	cout << "How big is the herd?";
	cout << "\n[1] Small\n[2] Medium\n[3] Populous\n";
	cin >> herdsize;
	herdsize = validate(herdsize, 3);
	
	//changes value based on user input
	switch(herdsize)
	{
		case 1:
			herdsize = 35;
			break;
		case 2:
			herdsize = 65;
			break;
		case 3:
			herdsize = 120;
			break;
	}
	
	//sql to get terrain value of region
	sqlGetTera += to_string(reg) + ";";
	
	//runs sql if valid
	if(sqlite3_prepare_v2(db, sqlGetTera.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//runs sql, gets return value,
		sqlite3_step(prepState);
		chosen = reinterpret_cast<const char*>(sqlite3_column_text(prepState, 0));
		sqlite3_finalize(prepState);
	}
	
	//matches chosen species with terrain
	for(unsigned int i = 0; i < 3; i++)
	{
		if(chosen == terrain[i])
		{
			switch(i)
			{
				case 0: chosen = "Boar"; i = 3; break;
				case 1: chosen = "Goat"; i = 3; break;
				case 2: chosen = "Tuna"; i = 3; break;
			}
		}
	}
	
	//finishes sql
	sqlInsertHerd += to_string(herdnum) + "\", " + to_string(herdsize) + ", " + to_string(reg) + ", \"" + chosen + "\");";
	
	//runs sql if valid
	if(sqlite3_prepare_v2(db, sqlInsertHerd.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);
	}
	
}



//Calls functions for deleting table entries
void deletePath(sqlite3 * mydb)
{
	int choice = 0;
	
	cout << "\n\nWhat would you like to do?" << endl;
	choice = deleteMenu();
	while (true)
	{
		//runs functions based on user choice
		switch (choice) 
		{
			case 1: deleteSubject(mydb); break;
			case 2: deleteHerd(mydb); break;
			case -1: return ;
			default: cout << "That is not a valid choice." << endl;
		}
		cout << "\n\n";
		choice = deleteMenu();
	}
}
//validates user input for delete menu
int deleteMenu()
{
	int choice = 0;//holds user decision
	
	printDeleteMenu();//calls function
	cin >> choice;//gets input
	
	//validates input
	while ((!cin || choice < 1 || choice > 2) && choice  != -1)
	{		
		if (!cin)
		{
			cin.clear();
			cin.ignore(1000,'/n');
		}
		cout << "That is not a valid choice." << endl << endl;
		printDeleteMenu();
		cin >> choice;
	} 
	return choice;
}
//prints menu
void printDeleteMenu()
{
	cout << "Please choose an option (enter -1 to quit):  " << endl;
	cout << "1. Delete Subject" << endl;
	cout << "2. Delete Herd" << endl;
	cout << "Enter Choice: ";
	
}

//deletes subjects from database
void deleteSubject(sqlite3 * db)
{
	//sql statements
	string sqlDelete = "DELETE FROM SUBJECT WHERE SUB_NUM = ";
	string sqlSelect = "SELECT SUB_FNAME, SUB_LNAME FROM SUBJECT GROUP BY SUB_NUM;";
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	int choice = 0, i = 0;
	
	//runs, displays user input
	if(sqlite3_prepare_v2(db, sqlSelect.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		int i = 1;//counter

		//prompt for user input
		cout << "What is the subject's do you wish to delete?\n";
		
		//outputs sql result
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " " << sqlite3_column_text(prepState, 1) << "\n";
			i++;
		}
	
		//gets input, validates, clears resources, fixes database-counter discrepancy
		cin >> choice;
		choice = validate(choice, i);
		sqlite3_finalize(prepState);
		choice--;
	}
	
	//appends information to delete statement
	sqlDelete += to_string(choice) + ";";
	
	//runs delete sql if valid
	if(sqlite3_prepare_v2(db, sqlDelete.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//code to run sql
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);
	}
	
}
//deletes herd entries from database
void deleteHerd(sqlite3 * db)
{	
	//sql statements
	string sqlDelete = "DELETE FROM HERD WHERE HERD_NUM = ";
	string sqlSelect = "SELECT HERD_SPEC, REG_NAM FROM HERD JOIN REGION ON HERD_REG = REG_NUM GROUP BY HERD_NUM;";
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	int choice = 0, i = 0;//holds user choice, counter
	
	if(sqlite3_prepare_v2(db, sqlSelect.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{	//prompts for output 	
		cout << "What is the herd do you wish to delete?\n";
		
		//outputs sql results
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " " << sqlite3_column_text(prepState, 1) << "\n";
			i++;
		}
	
		//gets input, validates input, clears leaks, fixes discrepancy
		cin >> choice;
		choice = validate(choice, i);
		sqlite3_finalize(prepState);
		choice--;
	}
	
	//appends information to sql delete statement
	sqlDelete += to_string(choice) + ";";
	
	//runs sql delete statmement if valid
	if(sqlite3_prepare_v2(db, sqlDelete.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{	
		//runs sql
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);
	}
	
	
}


//calls update functions
void updatePath(sqlite3 * mydb)
{
	int choice = 0;
	cout << "\n\nWhat would you like to do?" << endl;
	choice = updateMenu();
	while (true)
	{
		//runs functions based on user choice
		switch (choice) 
		{
			case 1: updateSubject(mydb); break;
			case 2: updateCastle(mydb); break;
			case 3: updateHerd(mydb); break;
			case -1: return ;
			default: cout << "That is not a valid choice." << endl;
		}
		cout << "\n\n";
		choice = updateMenu();
	}
}
//validates user input for update functions
int updateMenu()
{
	int choice = 0;//holds user decision
	
	printUpdateMenu();//calls function
	cin >> choice;//gets input
	
	//validates input
	while ((!cin || choice < 1 || choice > 3) && choice  != -1)
	{		
		if (!cin)
		{
			cin.clear();
			cin.ignore(1000,'/n');
		}
		cout << "That is not a valid choice." << endl << endl;
		printUpdateMenu();
		cin >> choice;
	} 
	return choice;
}
//outputs menu 
void printUpdateMenu()
{
	cout << "Please choose an option (enter -1 to quit):  " << endl;
	cout << "1. Update Subject" << endl;
	cout << "2. Update Castle" << endl;
	cout << "3. Update Herd" << endl;
	cout << "Enter Choice: ";
}

//updates subject entries
//unfinished - fails halfway through
void updateSubject(sqlite3 * db)
{
	//sql statements
	string sqlSelectSub, sqlUpdateSub;
	string sqlSelectCastle, sqlGetRegion;
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	
	int sub = 0, i = 1;//holds user chosen sub number, counter
	int castle;//holds user chosen castle
	
	cout << "What subject are you changing?\n";//prompts for input
	
	sqlSelectSub = "SELECT SUB_FNAME, SUB_LNAME FROM SUBJECT GROUP BY SUB_NUM;";//sql sstatement to get subject base info
	
	//runs sql as long as it is valid
	if (sqlite3_prepare_v2(db, sqlSelectSub.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//outputs sql results
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " " << sqlite3_column_text(prepState, 1);
			cout << "\n";
			i++;
		}
		sqlite3_finalize(prepState);//clears resource leaks
	}
	
	//gets input, validates same input, fixes discrepancy, prompts for next input, writes Select statement
	cin >> sub;
	sub = validate(sub, i);
	sub--;
	cout << "Where does the Subject live now?\n";
	sqlSelectCastle = "SELECT CSTL_NAM FROM CASTLE GROUP BY CSTL_NUM";
	
	//runs sql, if valid
	if (sqlite3_prepare_v2(db, sqlSelectCastle.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		i = 1;
		//outputs sql results
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0);
			cout << "\n";
			i++;
		}
		sqlite3_finalize(prepState);
	}
	
	//accepts input, validates input, fixes discrepancy, writes sql for castle region selection, declares item to hold value
	cin >> castle;
	castle = validate(castle, i);
	castle--;
	sqlGetRegion = "SELECT CSTL_REG FROM CASTLE WHERE CSTL_NUM = " + to_string(castle) + ";";
	int region = 0;
	
	//runs sql if valid
	if (sqlite3_prepare_v2(db, sqlGetRegion.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//gets sql output
		sqlite3_step(prepState);
		region = atoi(reinterpret_cast<const char*>(sqlite3_column_text(prepState, 0)));//fails here
		sqlite3_finalize(prepState);
	}
	
	//builds sql 
	sqlUpdateSub = "UPDATE SUBJECT SET SUB_CSTL = " + to_string(castle) + ", SUB_REGION = " + to_string(region) + " WHERE SUB_NUM = " + to_string(sub) + ";";
	
	//runs sql if valid
	if (sqlite3_prepare_v2(db, sqlUpdateSub.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//runs sql
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);
	}
	
	
}
//I couldn't think of a practical way to implement this function given my current constraints,
//so I update the castle populations with this function.
void updateCastle(sqlite3 * db)
{
	//notifies user of operation
	cout << "\nUpdating castle populations... \n";
	//declares strings to hold sql and populates them
	string sqlGetCastleNum = "SELECT COUNT(CSTL_NUM) FROM CASTLE;";
	string sqlUpdateCastleNum = "UPDATE CASTLE SET CSTL_SUB = "; //count of castle subjects
	string sqlGetSubCnt = "SELECT COUNT(SUB_NUM) FROM SUBJECT WHERE SUBJECT.SUB_CSTL = ";
	string originalGetSubCnt = sqlGetSubCnt;
	string originalCastleNum = sqlUpdateCastleNum;
	int count = 0;//counter value
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//runs sql statement if valid
	if (sqlite3_prepare_v2(db, sqlGetCastleNum.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return ;//finishes function
	}
	else
	{
		//runs sql statements, gets information from it, frees memory
		sqlite3_step(prepState);
		count = atoi(reinterpret_cast<const char*>(sqlite3_column_text(prepState, 0)));
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
	}
	
	//runs sql multiple times to check the populations of each castle
	for(unsigned int i = 0; i < count; i++)
	{
		sqlGetSubCnt = originalGetSubCnt;//returns sql to original statement
		sqlUpdateCastleNum = originalCastleNum;
		
		int numOfSubs = 0;//declares variable to hold number of subjects
		sqlGetSubCnt += to_string(i) + ";";//appends i onto sql for each run of sql
		if (sqlite3_prepare_v2(db, sqlGetSubCnt.c_str(), -1, &prepState, NULL) != SQLITE_OK)
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			return ;//finishes function
		}
		else
		{
			//runs sql statement, gets return value
			sqlite3_step(prepState);
			numOfSubs = atoi(reinterpret_cast<const char*>(sqlite3_column_text(prepState, 0)));
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		}
		
		//appends new data onto sqlUpdate statement
		sqlUpdateCastleNum += to_string(numOfSubs) + " WHERE CSTL_NUM = " + to_string(i) + ";";
		
		//runs sql
		if (sqlite3_prepare_v2(db, sqlUpdateCastleNum.c_str(), -1, &prepState, NULL) != SQLITE_OK)
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			return ;//finishes function
		}
		else
		{
			sqlite3_step(prepState); //runs sql statement
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		}
	}
	
	
	cout << "Update Complete.\n";//notifies user
}

//updates herd from database
void updateHerd(sqlite3 * db)
{	
	//sql statements
	string sqlGetHerds = "SELECT HERD_SIZE, REG_NAM, HERD_SPEC FROM HERD JOIN REGION ON HERD_REG = REG_NUM;";
	string sqlGetHerdSize = "SELECT HERD_SIZE FROM HERD WHERE HERD_NUM = ";
	string updateHerd = "UPDATE HERD SET HERD_SIZE = ";
	string deleteHerd = "DELETE FROM HERD WHERE HERD_NUM = ";
	string sqlChosenFunc;
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//holds user info
	int size = 0, choice = 0;
	int herdsize = 0;
	int i = 0;//counter
	
	//runs sql to get herd information if valid
	if (sqlite3_prepare_v2(db, sqlGetHerds.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return ;//finishes function
	}
	else
	{
		i = 1;
		//outputs sql return data
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << "Herd Species and Location: " << sqlite3_column_text(prepState, 2) << " " << sqlite3_column_text(prepState, 1) << "\n";
			cout << "Herd Size: " << sqlite3_column_text(prepState, 0) << "\n\n";
			i++;
		}
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		
	}
	
	//gets information from user, validates it, fixes discrepancy, appends to get herd statement
	cout << "Which herd are you updating?";
	cin >> choice;
	choice = validate(choice, i);
	choice--;
	cout << "By how much has the herd been reduced?\n";
	cin >> size;
	size = validate(size, INT_MAX);
	sqlGetHerdSize += to_string(choice) + ";";
	
	//runs sql if valid
	if (sqlite3_prepare_v2(db, sqlGetHerdSize.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return ;//finishes function
	}
	else
	{
		//runs sql
		sqlite3_step(prepState);
		herdsize = atoi(reinterpret_cast<const char*>(sqlite3_column_text(prepState, 0)));//gets result of sql
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
	}
	
	//if the herd size should be zero, then the herd is instead deleted otherwise it will update
	if(size > herdsize)
	{
		sqlChosenFunc += deleteHerd;
		sqlChosenFunc += to_string(choice) + ";";
	}
	else
	{
		sqlChosenFunc += updateHerd;
		sqlChosenFunc += " " + to_string(herdsize - size) + " WHERE HERD_NUM = " + to_string(choice) +";";
	}
	
	//runs sql if it is valid
	if (sqlite3_prepare_v2(db, sqlChosenFunc.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return ;//finishes function
	}
	else
	{
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
	}
	
}

//sql transaction
void transaction(sqlite3 * db)
{
	int rc, sendChoice, recChoice, choice, quan, quan2;//holds user input and results
	
	//sql queries needed for transaction
	string sqlSelectCntry = "SELECT CNTRY_NAM FROM COUNTRY ORDER BY CNTRY_NUM;";
	string sqlCountTrade = "SELECT * FROM TRADES;"; 
	string sqlUpdateSenCountry = "UPDATE COUNTRY SET CNTRY_GOLD = CNTRY_GOLD";
	string sqlUpdateRecCountry = "UPDATE COUNTRY SET CNTRY_GOLD = CNTRY_GOLD";
	string sqlInsertSenCountry = "INSERT INTO TRADES VALUES(";
	string sqlInsertRecCountry = "INSERT INTO TRADES VALUES(";
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//checks sql for errors; continues if the sql is valid, otherwise it quits
	if (sqlite3_prepare_v2(db, sqlCountTrade.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//increments based on the number of trades
		int i = 1;
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			i++;
		}
		
		//gives the two new trades values for their number
		sqlInsertSenCountry += to_string(i-1) + ", ";
		sqlInsertRecCountry += to_string(i) + ", ";
		
	}
	
	
	cout << "\n\n";//formatting
	
	//checks sql for errors; continues if the sql is valid, otherwise it quits
	if (sqlite3_prepare_v2(db, sqlSelectCntry.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		cout << "What country is sending?\n";//prompts for user input
		
		int i = 1;
		//outputs user information
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
			
		}
		
		
		cin >> sendChoice;//gets input
		//validates input
		sendChoice = validate(sendChoice, i);
		//appends choice to sql
		sqlInsertSenCountry += to_string(sendChoice-1) + ", ";
		//prompts for information
		cout << "What country is receiving?\n";
		cin >> recChoice;
		
		//validates, since a country cannot trade with itself.
		do
		{
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '/n');
			}
			if (!cin || recChoice < 1 || recChoice > i || recChoice == sendChoice)
			{	
				cout << "That is not a valid choice! Try Again!" << endl;
				cin >> recChoice;
			}
		} while (!cin);
		
		//appends information from sql
		sqlInsertSenCountry += to_string(recChoice-1) + ", ";
		sqlInsertRecCountry += to_string(recChoice-1) + ", " + to_string(sendChoice-1) + ", ";
		
		//prompts for information, gets input, validates input
		cout << "What is being sent?\n[1] Food\n[2] Gold\n";
		cin >> choice;
		choice = validate(choice, 2);
		
		//appends to sql based on user choice
		switch(choice)
		{
			case 1: 
				sqlInsertSenCountry += "\"Food \", ";
				sqlInsertRecCountry += "\"Gold \", ";
				break;
			case 2:
				sqlInsertSenCountry += "\"Gold \", ";
				sqlInsertRecCountry += "\"Food \", ";
				break;
		}
		
		//gets input
		cout << "How much was sent?\n";
		cin >> quan;
		
		//validates
		quan = validate(quan, INT_MAX);
		
		//changes sql based on input
		switch(choice)
		{
			case 1: 
				quan2 = quan * 2;
				sqlInsertSenCountry += to_string(quan) + ", date('now'));";
				sqlInsertRecCountry += to_string(quan2) + ", date('now'));";
				sqlUpdateSenCountry += " - " + to_string(quan) + ", CNTRY_FOOD = CNTRY_FOOD +" + to_string(quan2) + " WHERE CNTRY_NUM = " + to_string(sendChoice) + ";";
				sqlUpdateRecCountry += " - " + to_string(quan2) + ", CNTRY_FOOD = CNTRY_FOOD +" + to_string(quan) + " WHERE CNTRY_NUM = " + to_string(recChoice) + ";";
				break;
			case 2:
				quan2 = quan / 2;
				sqlInsertSenCountry += to_string(quan) + ", date('now'));";
				sqlInsertRecCountry += to_string(quan2) + ", date('now'));";
				sqlUpdateSenCountry += " - " + to_string(quan) + ", CNTRY_FOOD = CNTRY_FOOD +" + to_string(quan2) + " WHERE CNTRY_NUM = " + to_string(sendChoice) + ";";
				sqlUpdateRecCountry += " - " + to_string(quan2) + ", CNTRY_FOOD = CNTRY_FOOD +" + to_string(quan) + " WHERE CNTRY_NUM = " + to_string(recChoice) + ";";
				break;
		}
		
	}
	
	//declares two transaction statements for beginning and ending the transaction
	string sqlBEGIN = "BEGIN TRANSACTION;\n";
	string sqlCOMMIT = "COMMIT;";
	
	//holds every sql statement built for transaction
	string sql[6] = {sqlBEGIN, sqlInsertSenCountry, sqlInsertRecCountry, sqlUpdateSenCountry, sqlUpdateRecCountry, sqlCOMMIT};
	
	//runs every statement in array of sql statements
	for(unsigned int i = 0; i < 6; i++)
	{
		if (sqlite3_prepare_v2(db, sql[i].c_str(), -1, &prepState, NULL) != SQLITE_OK)
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			ROLLBACK(db);
			return;//finishes function
		}
		else
		{
			sqlite3_step(prepState);//runs sql
			sqlite3_finalize(prepState);//closes resource leaks
		}	
	}
	
}//end sql transaction


//Viewing Data calls functions for viewing data
void viewData(sqlite3 * db)
{
	int choice;//holds choice

	//outputs options
	cout << "\nWhat would you like to view?\n";
	cout << "[1] Countries\n[2] Castles\n[3] Subjects\n";
	cin >> choice;
	
	//calls functions based on user decision
	switch(choice)
	{
		case 1:
			viewCountry(db);
			break;
		case 2:
			viewCastle(db);
			break;
		case 3:
			viewSubject(db);
			break;
		default:
			cout << "Bad input, please retry\n"; 
	}
	
}//end view data

//views countries from database
void viewCountry(sqlite3 * db)
{
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	//sql for queries
	//sqlOne gets country names to display to user, sqlTwo gets the country information that the user designated
	string sqlOne = "SELECT CNTRY_NAM FROM COUNTRY ORDER BY CNTRY_NUM;";
	string sqlTwo = "SELECT CNTRY_NAM, SUB_FNAME, SUB_LNAME, CNTRY_GOLD, CNTRY_FOOD, REG_NAM, CSTL_NAM FROM COUNTRY "; 
	sqlTwo += "JOIN SUBJECT ON CNTRY_RULE = SUB_NUM ";
	sqlTwo += "JOIN REGION ON CNTRY_REG = REG_NUM ";
	sqlTwo += "JOIN CASTLE ON CNTRY_CSTL = CSTL_NUM ";
	sqlTwo += "WHERE CNTRY_NUM = ";
	
	int choice;//holds user choice
	int i = 1;//counter
	
	//runs sql to oput castle selections
	if (sqlite3_prepare_v2(db, sqlOne.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return ;//finishes function
	}
	else
	{
		
		//outputs results
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
		}
		
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		
		
	}
	
	//prompts user for input, gets input, and validates it, finally appends it to sqlTwo
	cout << "What country do you wish to view?\n";
	cin >> choice;
	choice = validate(choice, i);
	sqlTwo += to_string(choice-1) + ";";
	
	//runs sql to ouput information
	if (sqlite3_prepare_v2(db, sqlTwo.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return ;//finishes function
	}
	else
	{
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << sqlite3_column_text(prepState, 0) << ":\n";
			cout << "Country's Ruler: " << sqlite3_column_text(prepState, 1) << " " << sqlite3_column_text(prepState, 2) << "\n";
			cout << "Country Region: " << sqlite3_column_text(prepState, 5) << "\n";
			cout << "Country Castle: " << sqlite3_column_text(prepState, 6) << "\n";
			cout << "Country's Gold: " << sqlite3_column_text(prepState, 3) << "\n";
			cout << "Country's Food: " << sqlite3_column_text(prepState, 4);
			cout << endl;
			
		}
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
	}
	
}

//displays chosen subject information
void viewSubject(sqlite3 * db)
{
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//prepares sql statements, sqlOne gets basic subject information, sqlTwo gets chosen user information
	string sqlOne = "SELECT SUB_FNAME, SUB_LNAME FROM SUBJECT ORDER BY SUB_NUM;";
	string sqlTwo = "SELECT SUB_FNAME, SUB_LNAME, CSTL_NAM, REG_NAM, PROF_NAME ";
	sqlTwo += "FROM SUBJECT JOIN PROFESSION ON SUB_PROF = PROF_NUM ";
	sqlTwo += "JOIN REGION ON SUB_REGION = REG_NUM JOIN CASTLE ON SUB_CSTL = CSTL_NUM ";
	sqlTwo += "WHERE SUB_NUM = ";
	
	int choice;//user choice
	int i = 1;//counter
	
	//runs sqlOne to display user information
	if (sqlite3_prepare_v2(db, sqlOne.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return ;//finishes function
	}
	else
	{
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
		}
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks	
	}
	
	//prompts for input, gets input, validates it, appends it to sql
	cout << "What subject do you wish to view?\n";
	cin >> choice;
	choice = validate(choice, i);
	choice--;
	sqlTwo += to_string(choice) + ";";
	
	//runs sql to display full subject information
	if (sqlite3_prepare_v2(db, sqlTwo.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << sqlite3_column_text(prepState, 0);
			cout << " " << sqlite3_column_text(prepState, 1) << "\n";
			cout << "Subject Castle: " << sqlite3_column_text(prepState, 2) << "\n";
			cout << "Subject Region: " << sqlite3_column_text(prepState, 3) << "\n";
			cout << "Subject Profession: " << sqlite3_column_text(prepState, 4) << "\n";
			
		}
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
	}
}

//allows user to display castle information
void viewCastle(sqlite3 * db)
{
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	string sqlOne = "SELECT CSTL_NAM FROM CASTLE GROUP BY CSTL_NUM;";	
	string sqlTwo = "SELECT SUB_FNAME, SUB_LNAME, CSTL_NAM, REG_NAM, CSTL_SUB, CNTRY_NAM ";
	sqlTwo += "FROM CASTLE ";
	sqlTwo += "JOIN COUNTRY ON CSTL_CNTRY = CNTRY_NUM ";
	sqlTwo += "JOIN REGION ON CSTL_REG = REG_NUM ";
	sqlTwo += "JOIN SUBJECT ON CSTL_LRD = SUB_NUM ";
	sqlTwo += "WHERE CSTL_NUM = ";
	
	int choice;//holds user choice
	int i = 1;//counter
	
	//runs sql to display castles to user
	if (sqlite3_prepare_v2(db, sqlOne.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return ;//finishes function
	}
	else
	{
		//outputs castle information
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
		}
		
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
	}
	
	//prompts for input, gets input, validates, fixes discrepancy, sqlTwo has user choice appended
	cout << "What castle would you like to view?\n";
	cin >> choice;
	choice = validate(choice, i);
	choice--;
	sqlTwo += to_string(choice) + ";";
	//cout << "\n" << sqlTwo << "\n";
	
	//runs sqlTwo to display full castle information that was chosen by the user
	if (sqlite3_prepare_v2(db, sqlTwo.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//displays information from sql as long as it returns information
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			cout << "\n" << sqlite3_column_text(prepState, 2);
			cout << "\nCastle Country: " << sqlite3_column_text(prepState, 5);
			cout << "\nCastle Region: " << sqlite3_column_text(prepState, 3) << "\n";
			cout << "Total Subjects: " << sqlite3_column_text(prepState, 4) << "\n";
			cout << "Castle Lord: " << sqlite3_column_text(prepState, 0) << " " << sqlite3_column_text(prepState, 1) << "\n";

		}
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
	}
	
}


//Status: Complete
//This function validates integer inputs
int validate(int resp, int i)
{
	do
	{
		//catches stream breaks
		if (!cin) 
		{
			cin.clear();
			cin.ignore(1000, '/n');
		}
		
		if (!cin || resp < 1 || resp > i)//displays message
		{
			cin >> resp;//gets input
			cout << "That is not a valid choice! Try Again!" << endl;
		}
		
	} while (!cin || resp < 1 || resp > i);
	
	return resp;
}

//runs rollback sql to revert any changes that can't be saved
void ROLLBACK(sqlite3 * db)
{
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	string sqlROLLBACK = "ROLLBACK;";
	
	if (sqlite3_prepare_v2(db, sqlROLLBACK.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
	}
	
}



