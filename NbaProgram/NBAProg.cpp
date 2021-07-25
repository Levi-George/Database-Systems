/*
Program Name: NBAProg
Author: Levi George
Date Last Updated: 11/30/2019
Program Purpose: To do stuff with a database
*/

#include <iostream>
#include <string>
#include <iomanip>
#include "sqlite3.h"

using namespace std;

//function prototypes
void printMainMenu();
void addPlayer(sqlite3 *);
void addGame(sqlite3 *);
void viewDivision(sqlite3 *);
void viewTeam(sqlite3 *);
void viewGame(sqlite3 *);
int mainMenu();


int main()
{
	int choice, rc;//declares user choice and return code (rc) variables
	
	sqlite3 *mydb;//declares database
	
	rc = sqlite3_open_v2("NBA.db", &mydb, SQLITE_OPEN_READWRITE, "win32");//opens database
	
	//creates program menu
	cout << "Welcome to the NBA" << endl;
	choice = mainMenu();
	while (true)
	{
		//runs functions based on user choice
		switch (choice) 
		{
			case 1: addPlayer(mydb); break;
			case 2: addGame(mydb); break;
			case 3: viewDivision(mydb); break;
			case 4: viewTeam(mydb); break;
			case 5: viewGame(mydb); break;
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

//this function displays options available to the user
void printMainMenu() 
{
	cout << "Please choose an option (enter -1 to quit):  " << endl;
	cout << "1. Add a Player" << endl;
	cout << "2. Add a Game" << endl;
	cout << "3. View Teams By Division" << endl;
	cout << "4. View Team Roster" << endl;
	cout << "5. View Existing Game" << endl;
	cout << "Enter Choice: ";
}

//this function allows the user to choose what option they want to run
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

//adds player to database
void addPlayer(sqlite3 *db)
{	
	
	//holds users entries
	string sqlFNAME, sqlLNAME, sqlNUM;
	int sqlTEAM = 0;

	//queries that will run to get needed information or to insert users selected information
	string sqlState2 = "SELECT TEAMS.TEAM_NAM FROM TEAMS ORDER BY TEAM_NUM;";
	string sqlState1 = "INSERT INTO PLAYER VALUES(";
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//checks sql for errors; continues if the sql is valid, otherwise it quits
	if (sqlite3_prepare_v2(db, sqlState2.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//clears buffer
		cin.clear();
		cin.ignore(1000, '\n');
		
		//gets input
		cout << "What is the player's first name?\n";
		getline(cin, sqlFNAME);
		cout << "What is the player's last name?\n";
		getline(cin, sqlLNAME);
		cout << "What is the player's number?\n";
		cin >> sqlNUM;
		cout << "What team does the player belong to?" << endl;
		
		
		
		int i = 1; //declares base value for counter
		cout << left;//formatting
		while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
		{
			
			//outputs text of the Customer Code column for each row
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
		}
		do
		{
			
			//validates user input, ensuring they don't break the stream and that they enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '/n');
			}
			cin >> sqlTEAM;
			if (!cin || sqlTEAM < 1 || sqlTEAM > i)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin || sqlTEAM < 1 || sqlTEAM > i);
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		
		//finishes sql statement, adding user input to values
		sqlState1 = "INSERT INTO PLAYER VALUES(\"" + sqlFNAME + "\", \"" + sqlLNAME + "\", " + sqlNUM + ", " + to_string(sqlTEAM-1) + ");";
		
		//prepares the insert statment
		if (sqlite3_prepare_v2(db, sqlState1.c_str(), -1, &prepState, NULL) != SQLITE_OK)
		{
			
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			return;//finishes function
		}
		else
		{
			//runs statement
			sqlite3_step(prepState);
			cout << "Addition Successful!\n";
		}	
		
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		
	}
	
	
}

//this function adds a game to the database
void addGame(sqlite3 *db)
{	
	//sql statements to be ran in order to add new information or get needed information
	string sqlState3 = "SELECT * FROM GAME ORDER BY GAME_NUM;";
	string sqlState2 = "SELECT TEAM_NAM FROM TEAMS GROUP BY TEAM_NUM";
	string sqlState1 = "INSERT INTO GAME VALUES(";
	
	//holds user input or output from program
	int home, visit;
	int score1 = 0, score2 = 0; 
	int numOfCols = 0;
	string date;
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//checks sql query number three, if it is invalid then the function finishes
	if(sqlite3_prepare_v2(db, sqlState3.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
		{
			//increments for the number of rows in the sql command return information
			numOfCols++;
			cout << numOfCols << endl;
		}
		
	}
	
	sqlite3_finalize(prepState);//clears resource leaks
	
	//checks sql query number two, if it is invalid then the function finishes
	if(sqlite3_prepare_v2(db, sqlState2.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		cout << "Who is the home team?\n";//prompts user
		
		int i = 1; //declares base value 
		cout << left;
		while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
		{
			//outputs text of the Customer Code column for each row
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
		}
		do
		{
			
			//validates user input, ensuring they don't break the stream and that they enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> home;
			if (!cin || home < 1 || home > i)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin || home < 1 || home > i);
		
		home--;//there is a one off logic error in the code without this.
		
		cout << "Who is the visiting team?\n";
		
		i = 1;//resets counter
		while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
		{
			//outputs text of the Customer Code column for each row
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
		}
		do
		{
			
			//validates user input, ensuring they don't break the stream and that they enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> visit;
			if (!cin || visit < 1 || visit > i || visit == home)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin || visit < 1 || visit > i || visit == home);
		
		visit--;//there is a one off logic error in the code without this.
		
		//prompts user for input
		cout << "What was the home team's score?\n";
		cin >> score1;
		
		//validates input
		while(score1 < 0 || !cin)
		{
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
		}
		
		//prompts for input
		cout << "What was the visiting team's score?\n";
		cin >> score2;
		
		//validates input
		while(score2 < 0 || !cin)
		{
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
		}
		
		cin.clear();
		cin.ignore(1000, '\n');
		//prompts for input
		cout << "When did the team play? YYYY-MM-DD : HH\n";
		getline(cin, date);
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		
		//creates sql statement and appends user data to it
		string sqlState1 = "INSERT INTO GAME VALUES(";
		sqlState1 += to_string(numOfCols) + ", ";
		sqlState1 += to_string(home) + ", " + to_string(visit) + ", \"" + date + "\", \""  + to_string(score1) + " : " + to_string(score2) + "\");"; 
		
		//prepares sql statement and runs it if it is valid
		if(sqlite3_prepare_v2(db, sqlState1.c_str(), -1, &prepState, NULL) != SQLITE_OK)
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			return;//finishes function
		}
		else
		{
			sqlite3_step(prepState);//runs the sql statement
		}			
		
		
	}
	
	
}

//this function will return the teams in a division
void viewDivision(sqlite3 *db)
{	
	//sql statements to be used for this process
	string sqlState2 = "SELECT DIV_NAM FROM DIVISION GROUP BY DIV_NUM";
	string sqlState1 = "SELECT TEAM_NAM FROM TEAMS WHERE TEAM_DIV = ";
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	int choice = 0;
	
	//checks if sql statement 2 is valid, continues as needed if so, otherwise it will display an error message
	if(sqlite3_prepare_v2(db, sqlState2.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//prompts user
		cout << "What Division would you like to view?\n";
		
		int i = 1; //declares base value 
		cout << left;
		while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
		{
			//outputs text of the Customer Code column for each row
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
		}
		do
		{
			
			//validates user input, ensuring they don't break the stream and that they enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin || choice < 1 || choice > i);
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		
		sqlState1 += to_string(choice-1);//finishes sql statement 1 by appending user input
		
		//checks to see if sql state 1 is valid, if not it will display an error message, otherwise it continues
		if(sqlite3_prepare_v2(db, sqlState1.c_str(), -1, &prepState, NULL) != SQLITE_OK)
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			return;//finishes function
		}
		else
		{
			i = 0;//sets counter to 0
			while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
			{
				
				//outputs text of the name of the team for each row
				cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
				cout << endl;
				i++;
			}
		}
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		
		
	}
	
	
	
}

//this function will display the members of a team
void viewTeam(sqlite3 *db)
{	
	//These are sql statements which will be used for this function
	string sqlState2 = "SELECT TEAM_NAM FROM TEAMS GROUP BY TEAM_NUM";
	string sqlState1 = "SELECT PLY_FNAME,PLY_LNAME,PLY_NUM FROM PLAYER WHERE PLY_TEAM = ";
	
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	int choice = 0;
	
	//checks if sql statement 2 is valid, continues as needed if so, otherwise it will display an error message
	if(sqlite3_prepare_v2(db, sqlState2.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//prompts for input
		cout << "What Team would you like to view?\n";
		
		int i = 1; //declares base value 
		cout << left;
		while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
		{
			//outputs name of each team
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " "; 
			cout << endl;
			i++;
		}
		do
		{
			
			//validates user input, ensuring they don't break the stream and that they enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin || choice < 1 || choice > i);
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		
		sqlState1 += to_string(choice-1);//finishes the sql statement
		
		//checks if sql statement 1 is valid, continues as needed if so, otherwise it will display an error message
		if(sqlite3_prepare_v2(db, sqlState1.c_str(), -1, &prepState, NULL) != SQLITE_OK)
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			return;//finishes function
		}
		else
		{
			i = 0;
			while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
			{
				
				//outputs the member information for each row
				cout << i << ". " << sqlite3_column_text(prepState, 0) << " " << sqlite3_column_text(prepState, 1);
				cout << " " << sqlite3_column_text(prepState, 2);
				cout << endl;
				i++;
			}
		}
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		
		
	}
	
}

//displays the games in the database
void viewGame(sqlite3 *db)
{	
	
	//these strings hold sql statements to be used in the function
	string sqlState3 = "SELECT TEAM_NAM FROM TEAMS JOIN GAME ON TEAMS.TEAM_NUM = GAME.GAME_VISIT WHERE TEAMS.TEAM_NUM = GAME.GAME_VISIT;";
	string sqlState2 = "SELECT TEAM_NAM FROM TEAMS JOIN GAME ON TEAMS.TEAM_NUM = GAME.GAME_HOME WHERE TEAMS.TEAM_NUM = GAME.GAME_HOME;";
	string sqlState1 = "SELECT GAME_SCORE, GAME_DATE FROM GAME WHERE GAME_NUM = ";
	
	sqlite3_stmt *prepState;//These will hold a modified version of the query after use
	sqlite3_stmt *prepState2;
	sqlite3_stmt *prepState3;
	
	string errorStatement;//this will hold a error message from the database
	int choice = 0;//holds the users option
	
	//checks if sql statement  is valid, continues as needed if so, otherwise it will display an error message
	if(sqlite3_prepare_v2(db, sqlState2.c_str(), -1, &prepState, NULL) != SQLITE_OK || sqlite3_prepare_v2(db, sqlState3.c_str(), -1, &prepState2, NULL) != SQLITE_OK )
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		cout << "What Game would you like to view?\n";
		
		int i = 1; //declares base value 
		cout << left;
		while (sqlite3_step(prepState) == SQLITE_ROW && sqlite3_step(prepState2) == SQLITE_ROW)//While the step function returns a row 
		{
			//outputs text of the Customer Code column for each row
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " vs. " << sqlite3_column_text(prepState2, 0); 
			cout << endl;
			i++;
		}
		do
		{
			
			//validates user input, ensuring they don't break the stream and that they enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin || choice < 1 || choice > i);
		
		sqlState1 += to_string(choice-1) + ";";//appends user option to end of sql statement
		
		cout << "\n\n";
		
		//checks if sql statement 1 is valid, continues as needed if so, otherwise it will display an error message
		if(sqlite3_prepare_v2(db, sqlState1.c_str(), -1, &prepState3, NULL) != SQLITE_OK )
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState3);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			return;//finishes function
		}
		else
		{
			i = 0;
			while (sqlite3_step(prepState3) == SQLITE_ROW)//While the step function returns a row 
			{
				
				//outputs the data for the game selected
				cout << "\nFinal Score: " << sqlite3_column_text(prepState3, 0) << "\nPlayed on: " << sqlite3_column_text(prepState3, 1);
				cout << endl;
				i++;
			}
		}
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		sqlite3_finalize(prepState2);//closes possible resource leaks from prepared Statement
		sqlite3_finalize(prepState3);//closes possible resource leaks from prepared Statement
		
	}
	
}