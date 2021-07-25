#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include "sqlite3.h"
/* Program name: NBA.cpp
*  Author: Dr. Sebastien Tides
*  Date last updated: 12/11/2019 d/m/y
*  Purpose: INSERT and SELECT NBA.db
*/
using namespace std;
//cl -EHsc NBA.cpp sqlite3.lib  ..EHsc for compiling to shut Windows Defender up
///*current bugs to fix: 
///1. 
///2. 
///3. 
void printMainMenu();
///the menu is serv'ed
void viewDivision(sqlite3 *);//select all teams where divi = E/W
void viewTeam(sqlite3 *);//select all team where index = choice
void viewGame(sqlite3 *);//select all game, print, detail selection
void newPlayer(sqlite3 *);//select a team, insert playerID, fname, lname and team into TEAM
void newGame(sqlite3 *);//select a team1, select a team2, insert home, away, date, time, home score, away score

string currentDate();//return current date in SQL format for INSERT
int mainMenu();

int main()
{
	int choice;//menu selection

	sqlite3 *mydb;//pionter for my SQL database object after opening

	int rc; //return value from the SQL execution attempt (1 or 0)

	rc = sqlite3_open_v2("NBA.db", &mydb, SQLITE_OPEN_READWRITE, "win32");//opens the database

	if (rc) {
		cout << "Error opening database: " << sqlite3_errmsg(mydb) << endl;
		return 0;
	}
	else {
		cout << "Database opened successfully!" << endl;
	}
	//^routine feedback.

	cout << "Welcome to the NBA" << endl;
	choice = mainMenu();

	while (true)
	{
		switch (choice)
		{
		case 1: viewDivision(mydb); break;//view division teams
		case 2: viewTeam(mydb); break;//view players on a teams by team selection
		case 3: viewGame(mydb); break;//view game by game selection
		case 4: newPlayer(mydb); break;//insert new player
		case 5: newGame(mydb); break;//insert new game record
		case -1: return 0;
		default: cout << "That is not a valid choice." << endl;
		}
		cout << "\n\n";

		choice = mainMenu();

	}

}

void printMainMenu()
{
	cout << "You can choose an option (enter -1 to quit):  " << endl;
	cout << "1. View a divisions teams" << endl;
	cout << "2. View a teams players" << endl;
	cout << "3. View a game" << endl;
	cout << "4. Enter new player" << endl;
	cout << "5. Enter new game" << endl;
	cout << "Enter Choice : ";

}

int mainMenu()
{
	int choice = 0;

	printMainMenu();
	cin >> choice;
	while ((!cin || choice < 1 || choice > 5) && choice != -1)
	{
		if (!cin)
		{
			cin.clear();
			cin.ignore(1000, '\n');
		}
		cout << "That is not a valid choice." << endl << endl;
		printMainMenu();
		cin >> choice;
	}

	return choice;
}

void viewDivision(sqlite3 * db)
{
	int choice;
	sqlite3_stmt *pRes;//variable type is SQL language, statement, an SQL object
	string m_strLastError;//takes the string for the SQL error code when passing SQL to DB object
	cout << "You would like to see the teams from which division (1-4)?"<<endl;
	cout << "1. North West" << endl;
	cout << "2. South East" << endl;
	cout << "3. Ivy League" << endl;
	cout << "4. Incorporated" << endl;

	cin >> choice;
	while (!cin || choice < 1 || choice > 4)
	{
		if (!cin)
		{
			cin.clear();
			cin.ignore(1000, '\n');
		}
		cout << "That is not a valid choice." << endl << endl;
		cin >> choice;
	}
	string cat = to_string(choice);
	string query = "SELECT * FROM TEAM WHERE TEAM.DIVI = '" + cat + "';";//queries can be built as selections are made for INSERT
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//check to see if SQL statement(query cast to c string) is valid and run it
	{
		m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(pRes);//deconstructor for SQL object
		cout << "There was an error: " << m_strLastError << endl;//prints the returned error code
		return;
	}
	else
	{
		///build database to test this code to print all three teams from selection
		int columnCount = sqlite3_column_count(pRes);//pRes appears to be the DB object
		int i = 1, choice;
		sqlite3_stmt *pRes2;//creating a new SQL object for the second query (query2)
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW)//while there is a row, take a step to the next row
		{
			cout << i << ". " << sqlite3_column_text(pRes, 0) <<"   "<< reinterpret_cast<const char*>(sqlite3_column_text(pRes, 1))<<"   "<< reinterpret_cast<const char*>(sqlite3_column_text(pRes, 2)) << "   " << reinterpret_cast<const char*>(sqlite3_column_text(pRes, 3)) << "   " << reinterpret_cast<const char*>(sqlite3_column_text(pRes, 4)) << "   " << reinterpret_cast<const char*>(sqlite3_column_text(pRes, 5)) << "   " << reinterpret_cast<const char*>(sqlite3_column_text(pRes, 6));
			cout << endl;
			i++;

		}
		sqlite3_finalize(pRes);//release the pRes
		///this is where I left off
		//start build of next query for next SQL
	}
}

void viewTeam(sqlite3 * db)//should not print a table for second query like viewInvoice
{
	
	sqlite3_stmt *pRes;
	string m_strLastError;
	string team_name;
	string query = "SELECT TEAM.TEAM FROM TEAM;";//just pull team name, 1 TABLE
	
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//check and run SQL
	{
		m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(pRes);
		cout << "There was an error: " << m_strLastError << endl;
		return;
	}
	else
	{
		cout << "Please choose the team roster you want to see:" << endl;//learn as you go, Seabass, learn as you go
		int columnCount = sqlite3_column_count(pRes);//THERE CAN BE ONLY ONE! ~Highlander
		int i = 1, choice;//
		sqlite3_stmt *pRes2;//pRes2 is the pointer to the data in second query
		cout << left;//
		while (sqlite3_step(pRes) == SQLITE_ROW)//drop that first QUERY on the screen
		{
			cout << i << ". " << sqlite3_column_text(pRes, 0);//
			cout << endl;//
			i++;//
		}
		do
		{
			if (!cin) //
			{
				cin.clear();//
				cin.ignore(1000, '\n');//
			}
			cin >> choice;//
			if (!cin || choice < 1 || choice > i)//why are not printing my shit
				cout << "That is not a valid choice! Try Again!" << endl;//will it let me try again?, nope, wtf
		} while (!cin);//

		sqlite3_reset(pRes);//
		for (int i = 0; i < choice; i++)//assign column variables based on choice
			sqlite3_step(pRes);//an "array" of pres from that row
		team_name = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0));//pRes, 1 takes the column by index of 1

		sqlite3_finalize(pRes);
		string query2 = "SELECT PLAYER.fn, PLAYER.ln FROM TEAM JOIN PLAYER ON TEAM.TEAM = PLAYER.TEAM WHERE PLAYER.TEAM = '"+team_name+"';";
		if (sqlite3_prepare_v2(db, query2.c_str(), -1, &pRes2, NULL) != SQLITE_OK)//check and run SQL
		{
			m_strLastError = sqlite3_errmsg(db);
			sqlite3_finalize(pRes2);
			cout << "There was an error: " << m_strLastError << endl;
			return;
		}
		else
		{
			int columnCount = sqlite3_column_count(pRes2);//THERE CAN BE ONLY ONE! ~Highlander
			int i = 1, choice;//
			cout << left;//
			while (sqlite3_step(pRes2) == SQLITE_ROW)//drop that first QUERY on the screen
			{
				cout << i << ". " << sqlite3_column_text(pRes2, 0)<<" "<<sqlite3_column_text(pRes2,1);//
				cout << endl;//
				i++;//
			}

			sqlite3_finalize(pRes2);


		}

	}
}

void viewGame(sqlite3 * db)
{
	
	sqlite3_stmt *pRes;
	string m_strLastError;
	string GMID, Hteam, Ateam, date, time, Hscr, Ascr;
	string query = "SELECT * FROM GAME;";//just pull everything for both calls since only 1 TABLE
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//check and run SQL
	{
		m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(pRes);
		cout << "There was an error: " << m_strLastError << endl;
		return;
	}
	else
	{
		cout << "Please choose the game you want to see:" << endl;//learn as you go, Seabass, learn as you go
		int columnCount = sqlite3_column_count(pRes);//THERE CAN BE ONLY ONE! ~Highlander
		int i = 1, choice;//
		cout << left;//
		while (sqlite3_step(pRes) == SQLITE_ROW)//drop that first QUERY on the screen
		{
			cout << sqlite3_column_text(pRes, 0) << ". " << sqlite3_column_text(pRes, 3) << " " << sqlite3_column_text(pRes, 1) << " vs. " << sqlite3_column_text(pRes, 2);//
			cout << endl;//
			i++;//
		}
		do
		{
			if (!cin) //
			{
				cin.clear();//
				cin.ignore(1000, '\n');//
			}
			cin >> choice;//
			if (!cin || choice < 1 || choice > i)//why are not printing my shit
				cout << "That is not a valid choice! Try Again!" << endl;//will it let me try again?, nope, wtf
		} while (!cin);//

		sqlite3_reset(pRes);//
		for (int i = 0; i < choice; i++)//assign column variables based on choice
			sqlite3_step(pRes);//an "array" of pres from that row
		GMID = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0));//pRes, 1 takes the column by index of 1
		Hteam = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 1));//when selecting all takes the column by index 2
		Ateam = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 2));
		date = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 3));
		time = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 4));
		Hscr = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 5));
		Ascr = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 6));

		sqlite3_finalize(pRes);

			cout << left;
			cout << "Home Score: " << Hscr << endl;
			cout << "Away Score: (" << Ascr << ") " <<endl;
			cout << "Play off: " << time << endl;			
	}
}

void newPlayer(sqlite3 * db)//passes whole database and then...
{

	sqlite3_stmt *pRes;//print CUSTOMER

	string m_strLastError;
	string fname, lname, team;///for the product

	string query = "SELECT TEAM.TEAM FROM TEAM;";//
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//check and run SQL
	{
		m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(pRes);
		cout << "There was an error: " << m_strLastError << endl;
		return;
	}
	else
	{
		cout << "first name?:" << endl;
		cin >> fname;
		cout << "last name?:" << endl;
		cin >> lname;
		cout << "purchasing team?:" << endl;
		int columnCount = sqlite3_column_count(pRes);//THERE CAN BE ONLY ONE! ~Highlander
		int i = 1, choice;//
		sqlite3_stmt *pRes2;//pRes2 is the pointer to the data in second query
		cout << left;//
		while (sqlite3_step(pRes) == SQLITE_ROW)//drop that first QUERY on the screen
		{
			cout << i << ". " << sqlite3_column_text(pRes, 0);//
			cout << endl;//
			i++;//
		}
		do
		{
			if (!cin) //
			{
				cin.clear();//
				cin.ignore(1000, '\n');//
			}
			cin >> choice;//
			if (!cin || choice < 1 || choice > i)//why are not printing my shit
				cout << "That is not a valid choice! Try Again!" << endl;//will it let me try again?, nope, wtf
		} while (!cin);//

		sqlite3_reset(pRes);//
		for (int i = 0; i < choice; i++)//assign column variables based on choice
			sqlite3_step(pRes);//an "array" of pres from that row
		team = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0));//pRes, 1 takes the column by index of 1

		sqlite3_finalize(pRes);

		
		string query2 = "INSERT INTO PLAYER(PID, fn, ln, team) VALUES (NULL, '"+fname+"', '"+lname+"', '"+team+"'); ";//
		if (sqlite3_prepare_v2(db, query2.c_str(), -1, &pRes2, NULL) != SQLITE_OK)//check and run SQL
		{
			m_strLastError = sqlite3_errmsg(db);
			sqlite3_finalize(pRes2);
			cout << "There was an error: " << m_strLastError << endl;
			return;
		}
		else
		{
			sqlite3_step(pRes2);
			sqlite3_finalize(pRes2);//recycle




		}



	}
///end of edit
}

void newGame(sqlite3 * db)
{
	sqlite3_stmt *pRes;//print CUSTOMER
	string Hteam, Ateam, date, time, Hscr, Ascr;
	string m_strLastError;
	string query = "SELECT TEAM.TEAM FROM TEAM;";//
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//check and run SQL
	{
		m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(pRes);
		cout << "There was an error: " << m_strLastError << endl;
		return;
	}
	else
	{
		cout << "Which was the home team?:" << endl;

		int columnCount = sqlite3_column_count(pRes);//THERE CAN BE ONLY ONE! ~Highlander
		int i = 1, choice;//
		cout << left;//
		while (sqlite3_step(pRes) == SQLITE_ROW)//drop that first QUERY on the screen
		{
			cout << i << ". " << sqlite3_column_text(pRes, 0);//
			cout << endl;//
			i++;//
		}
		do
		{
			if (!cin) //
			{
				cin.clear();//
				cin.ignore(1000, '\n');//
			}
			cin >> choice;//
			if (!cin || choice < 1 || choice > i)//why are not printing my shit
				cout << "That is not a valid choice! Try Again!" << endl;//will it let me try again?, nope, wtf
		} while (!cin);//

		sqlite3_reset(pRes);//
		for (int i = 0; i < choice; i++)//assign column variables based on choice
			sqlite3_step(pRes);//an "array" of pres from that row
		Hteam = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0));//pRes, 1 takes the column by index of 1
	}
		sqlite3_finalize(pRes);

		query = "SELECT TEAM.TEAM FROM TEAM;";//
		if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//check and run SQL
		{
			m_strLastError = sqlite3_errmsg(db);
			sqlite3_finalize(pRes);
			cout << "There was an error: " << m_strLastError << endl;
			return;
		}
		else
		{
			cout << "Which was the away team?:" << endl;

			int columnCount = sqlite3_column_count(pRes);//THERE CAN BE ONLY ONE! ~Highlander
			int i = 1, choice;//
			cout << left;//
			while (sqlite3_step(pRes) == SQLITE_ROW)//drop that first QUERY on the screen
			{
				cout << i << ". " << sqlite3_column_text(pRes, 0);//
				cout << endl;//
				i++;//
			}
			do
			{
				if (!cin) //
				{
					cin.clear();//
					cin.ignore(1000, '\n');//
				}
				cin >> choice;//
				if (!cin || choice < 1 || choice > i)//why are not printing my shit
					cout << "That is not a valid choice! Try Again!" << endl;//will it let me try again?, nope, wtf
			} while (!cin);//

			sqlite3_reset(pRes);//
			for (int i = 0; i < choice; i++)//assign column variables based on choice
				sqlite3_step(pRes);//an "array" of pres from that row
			Ateam = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0));//pRes, 1 takes the column by index of 1

			sqlite3_finalize(pRes);
		}
		date = currentDate();
		///inside the else for the insert///
		cout << "What time did they start play?:" << endl;
		cin >> time;
		cout << "What was the final home score?:" << endl;
		cin >> Hscr;
		cout << "What was the final away score?:" << endl;
		cin >> Ascr;
		query = "INSERT INTO GAME VALUES (NULL, '" + Hteam + "', '" + Ateam + "', '" + date + "', '"+time+"', '"+Hscr+"','"+Ascr+"'); ";//
		if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)//check and run SQL
		{
			m_strLastError = sqlite3_errmsg(db);
			sqlite3_finalize(pRes);
			cout << "There was an error: " << m_strLastError << endl;
			return;
		}
		else
		{
			sqlite3_step(pRes);
			sqlite3_finalize(pRes);//recycle

		}
		///end end ///


}

string currentDate() {
	time_t now = time(0);
	tm *ltm = localtime(&now);
	string year = to_string(1900 + ltm->tm_year);
	string month = to_string(1 + ltm->tm_mon);
	string day = to_string(ltm->tm_mday);
	string date = year + "-" + month + "-" + day;
	return date;
}

