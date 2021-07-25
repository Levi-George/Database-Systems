//Darrin Perry
//NBA.cpp
//12/7/2019
//This program allows a user to pull and add data to a database

#include <iostream>
#include <string>
#include <algorithm>
#include "sqlite3.h"

using namespace std;

void intro();
void selectMenu(sqlite3 *NBA);
static int callback(void *data , int argc, char **argv , char **azColName);
string sqlInjection(string input);
string divisionPick(int divPick);


int main() {
	
	sqlite3* NBA;
	int rc;
	char* messageError;
	const char* data = "Callback function called";
	int rows, cols;
	char** result;
	//opens database
	rc = sqlite3_open_v2("NBA.db", &NBA, SQLITE_OPEN_READWRITE, "win32");

	if (rc) {
		cout << "Error opening the database: " << sqlite3_errmsg(NBA) << endl;
	}
	else {
		cout << "Database opened successfully!!" << endl;
	}
	intro();
	char answer;
	//loop lets user do multiple things during one use
	do{
	selectMenu(NBA);
	cout << "Would you like to do another action?" << endl;
	cin >> answer;
	}while(answer == 'Y' || answer == 'y');
	return 0;
}

void intro() {
	cout << "Welcome to the NBA database!!" << endl;
	cout << "This dynamic database will allow you to view all teams" << endl;
	cout << "and add teams at your leisure, you can also view games" << endl;
	cout << "played and even add new games recently played!! \n" << endl;

}
//probably should have refactored this into multiple functions :(
void selectMenu(sqlite3 *NBA) {
	int choice;
	int rc;
	const char* data = "Callback function called";
	int rows, cols;
	char** result;
	char* messageError;
	cout << "What would you like to do with this database?" << endl;
	cout << "1 - View data" << endl;
	cout << "2 - Add data" << endl;
	cin >> choice;
	int choice1 , choice2 , choice3;
	if (choice == 1) {
		cout << "What data would you like to view?" << endl;
		cout << "1 - View division" << endl;
		cout << "2 = View roster" << endl;
		cout << "3 - View game" << endl;
		cin >> choice1;
		if (choice1 == 1) {
			int view1;
			cout << "What division would you like to view?" << endl;
			cout << "1 - Atlantic" << endl;
			cout << "2 - Central" << endl;
			cout << "3 - Southeast" << endl;
			cout << "4 - Northwest" << endl;
			cout << "5 - Southwest" << endl;
			cout << "6 - Pacific" << endl;
			cin >> view1;
			if (view1 == 1) {
				string sqlView1 = "SELECT TM_NAME FROM TEAM WHERE DIV_DIVNAME = 'ATLANTIC';";      //sql statement to pull teams from said division
				rc = sqlite3_exec(NBA , sqlView1.c_str() , callback , (void*) data , &messageError);
				if(rc != SQLITE_OK)
				{
					cout << "There was an error retrieving data from the database " << messageError << endl;
					sqlite3_free(messageError);
				}
				else{
				cout << "Called Select" << endl;
				}
			}
			else if (view1 == 2) {
				string sqlView1 = "SELECT TM_NAME FROM TEAM WHERE DIV_DIVNAME = 'CENTRAL';";
				rc = sqlite3_exec(NBA , sqlView1.c_str() , callback , (void*) data , &messageError);
				if(rc != SQLITE_OK)
				{
					cout << "There was an error retrieving data from the database " << messageError << endl;
					sqlite3_free(messageError);
				}
				else{
				cout << "Called Select" << endl;
				}
			}
			else if (view1 == 3) {
				string sqlView1 = "SELECT TM_NAME FROM TEAM WHERE DIV_DIVNAME = 'SOUTHEAST';";
				rc = sqlite3_exec(NBA , sqlView1.c_str() , callback , (void*) data , &messageError);
				if(rc != SQLITE_OK)
				{
					cout << "There was an error retrieving data from the database " << messageError << endl;
					sqlite3_free(messageError);
				}
				else{
				cout << "Called Select" << endl;
				}
			}
			else if (view1 == 4) {
				string sqlView1 = "SELECT TM_NAME FROM TEAM WHERE DIV_DIVNAME = 'NORTHWEST';";
				rc = sqlite3_exec(NBA , sqlView1.c_str() , callback , (void*) data , &messageError);
				if(rc != SQLITE_OK)
				{
					cout << "There was an error retrieving data from the database " << messageError << endl;
					sqlite3_free(messageError);
				}
				else{
				cout << "Called Select" << endl;
				}
			}
			else if (view1 == 5) {
				string sqlView1 = "SELECT TM_NAME FROM TEAM WHERE DIV_DIVNAME = 'SOUTHWEST';";
				rc = sqlite3_exec(NBA , sqlView1.c_str() , callback , (void*) data , &messageError);
				if(rc != SQLITE_OK)
				{
					cout << "There was an error retrieving data from the database " << messageError << endl;
					sqlite3_free(messageError);
				}
				else{
				cout << "Called Select" << endl;
				}
			}
			else {
				string sqlView1 = "SELECT TM_NAME FROM TEAM WHERE DIV_DIVNAME = 'PACIFIC';";
				rc = sqlite3_exec(NBA , sqlView1.c_str() , callback , (void*) data , &messageError);
				if(rc != SQLITE_OK)
				{
					cout << "There was an error retrieving data from the database " << messageError << endl;
					sqlite3_free(messageError);
				}
				else{
				cout << "Called Select" << endl;
				}
			}
		}
		else if (choice1 == 2) {
			cout << "What team's roster would you like to view?" << endl;
			string sqlView2 = "SELECT TM_NAME , PLYR_LNAME , PLYR_FNAME , PLYR_POSITION FROM PLAYER;";   //sql to pull data from database to be saved into variables
			sqlite3_stmt *pRes;
			string m_strLastError;
			string tm_team , plyr_lname , plyr_fname , plyr_position;
			if (sqlite3_prepare_v2(NBA, sqlView2.c_str(), -1, &pRes, NULL) != SQLITE_OK)
			{
				m_strLastError = sqlite3_errmsg(NBA);
				sqlite3_finalize(pRes);
				cout << "There was an error: " << m_strLastError << endl;
				return;
			}
			else{
				int columnCount = sqlite3_column_count(pRes);
				int i = 1, pick;
				cout << left;
			while (sqlite3_step(pRes) == SQLITE_ROW)
			{
				cout << i << ". " << sqlite3_column_text(pRes, 0);
				cout << endl;
				i++;
			}
			cin >> pick;
			sqlite3_reset(pRes);
			for (int i = 0; i < pick; i++)
			sqlite3_step(pRes);
			//saves information gathered from the query to strings to be used for output
			plyr_lname = reinterpret_cast<const char*>(sqlite3_column_text(pRes,1));
			plyr_fname = reinterpret_cast<const char*>(sqlite3_column_text(pRes,2));
			plyr_position = reinterpret_cast<const char*>(sqlite3_column_text(pRes,3));
			
			cout << "Player name: " << plyr_fname << " " << plyr_lname << endl;
			cout << "Position: " << plyr_position;
			cout << endl;
			}
		}
		
		else {
			cout << "Which game stats would you like to view?" << endl;
			string sqlView2 = "SELECT GM_NUMBER , GM_HOMETEAM , GM_AWAYTEAM , GM_HOMESCORE , GM_AWAYSCORE FROM GAME;";
			sqlite3_stmt *pRes;
			string m_strLastError;
			string gm_number , gm_hometeam , gm_awayteam , gm_homescore , gm_awayscore;
			if (sqlite3_prepare_v2(NBA, sqlView2.c_str(), -1, &pRes, NULL) != SQLITE_OK)
			{
				m_strLastError = sqlite3_errmsg(NBA);
				sqlite3_finalize(pRes);
				cout << "There was an error: " << m_strLastError << endl;
				return;
			}
			else{
				int columnCount = sqlite3_column_count(pRes);
				int i = 1, pick;
				cout << left;
			while (sqlite3_step(pRes) == SQLITE_ROW)
			{
				cout << i << ". Game # " << sqlite3_column_text(pRes, 0);
				cout << endl;
				i++;
			}
			cin >> pick;
			sqlite3_reset(pRes);
			for (int i = 0; i < pick; i++)
			sqlite3_step(pRes);
			//saves information gathered from the query to strings to be used for output
			gm_hometeam = reinterpret_cast<const char*>(sqlite3_column_text(pRes,1));
			gm_awayteam = reinterpret_cast<const char*>(sqlite3_column_text(pRes,2));
			gm_homescore = reinterpret_cast<const char*>(sqlite3_column_text(pRes,3));
			gm_awayscore = reinterpret_cast<const char*>(sqlite3_column_text(pRes,4));
			cout << gm_hometeam << " vs. " << gm_awayteam << endl;
			cout << gm_homescore << " - " << gm_awayscore << endl;
			cout << endl;
			}
		}
		
	}
	else if (choice == 2) {
		string m_strLastError;
		cout << "What would you like to add to the database?" << endl;
		cout << "1 - Add team" << endl;
		cout << "2 - Add player" << endl;
		cout << "3 - Add game" << endl;
		cin >> choice2;
		if (choice2 == 1) {
			string team;
			cin.ignore(80, '\n');
			cout << "What is the name of team to be added?" << endl;
			getline(cin , team);
			team = sqlInjection(team);
			string coach;
			cout << "What is the name of the head coach?" << endl;
			getline(cin, coach);
			coach = sqlInjection(coach);
			string division;
			cout << "What is the name of the division?" << endl;
			sqlite3_stmt *pRes;
			//grabs data from the division name table to populate options
			string divPick1 = "Select DIV_DIVNAME FROM DIVISION;";
			int pick = 0;
			if (sqlite3_prepare_v2(NBA, divPick1.c_str(), -1, &pRes, NULL) != SQLITE_OK)
			{
				m_strLastError = sqlite3_errmsg(NBA);
				sqlite3_finalize(pRes);
				cout << "There was an error: " << m_strLastError << endl;
				return;
			}
			else{
				int columnCount = sqlite3_column_count(pRes);
				int i = 1;
				cout << left;
			while (sqlite3_step(pRes) == SQLITE_ROW)
			{
				cout << i << ". " << sqlite3_column_text(pRes, 0);
				cout << endl;
				i++;
			}
			cin >> pick;
			sqlite3_reset(pRes);
			}
			int salaryCap;
			cout << "What is the teams salary cap?" << endl;
			cin >> salaryCap;
			while(!cin || salaryCap < 0){
				cout << "Illegal input!! Please try again!" << endl;
				cin.clear();
				cin.ignore(80, '\n');
				cout << "What is the teams salary cap?" << endl;
				cin >> salaryCap;
			}
			cin.ignore(80 , '\n');
			string mascot;
			cout << "What is the teams mascot?" << endl;
			getline(cin, mascot);
			mascot = sqlInjection(mascot);
			string city;
			cout << "What city is the team from?" << endl;
			getline(cin, city);
			city = sqlInjection(city);
			string state;
			cout << "What state is the team in?" << endl;
			getline(cin, state);
			state = sqlInjection(state);
			string arena;
			cout << "What is the name of the arena they play in?" << endl;
			getline(cin, arena);
			arena = sqlInjection(arena);
			division = divisionPick(pick);
			cout << team << ' ' << coach << ' ' << division << endl;
			
			string sqlInsert1 = "INSERT INTO TEAM VALUES ('" + team + "','" + division + "','" + coach + "'," + to_string(salaryCap) +
				",'" + mascot + "','" + city + "','" + state + "','" + arena + "');";
				rc = sqlite3_exec(NBA, sqlInsert1.c_str(), NULL , 0, &messageError);
				if( rc != SQLITE_OK ){
				cout << "SQL error: " << messageError;
				sqlite3_free(messageError);
				}
				else{
				cout << "Records created successfully\n";
				}
		}
		else if (choice2 == 2){
			string fname;
			cin.ignore(80, '\n');
			cout << "What is the first name of the player to be added?" << endl;
			getline(cin , fname);
			fname = sqlInjection(fname);
			string lname;
			cout << "What is the last name of the player?" << endl;
			getline(cin, lname);
			lname = sqlInjection(lname);
			int ssn;
			cout << "What is the Social Security Number of the player?" << endl;
			cin >> ssn;
			int number;
			cout << "What is the player's number?" << endl;
			cin >> number;
			cin.ignore(80 , '\n');
			string position;
			cout << "What is the player's position?" << endl;
			getline(cin, position);
			position = sqlInjection(position);
			int salary;
			cout << "What is the player's salary?" << endl;
			cin >> salary;
			cin.ignore(80 , '\n');
			string teamName;
			cout << "What team does the player play for?" << endl;
			getline(cin, teamName);
			teamName = sqlInjection(teamName);
			
			string sqlInsert2 = "INSERT INTO PLAYER VALUES (" + to_string(ssn) + ",'" + teamName + "','" + lname + "','" + fname +
				"','" + position + "'," + to_string(number) + "," + to_string(salary) + ");";
				rc = sqlite3_exec(NBA, sqlInsert2.c_str(), NULL , 0, &messageError);
				if( rc != SQLITE_OK ){
				cout << "SQL error: " << messageError;
				sqlite3_free(messageError);
				}
				else{
				cout << "Records created successfully\n";
				}

		}
		else{
			cin.ignore(80 , '\n');
			string homeTeam;
			cout << "Who is the home team?" << endl;
			getline(cin, homeTeam);
			homeTeam = sqlInjection(homeTeam);
			string awayTeam;
			cout << "Who is the away team?" << endl;
			getline(cin, awayTeam);
			awayTeam = sqlInjection(awayTeam);
			int hScore;
			cout << "What was the home team's score?" << endl;
			cin >> hScore;
			int aScore;
			cout << "What was the away team's score?" << endl;
			cin >> aScore;
			cin.ignore(80 , '\n');
			string date;
			cout << "What was the date of this game?" << endl;
			getline(cin, date);
			date = sqlInjection(date);
			string sqlInsert3 = "INSERT INTO GAME(GM_HOMETEAM , GM_AWAYTEAM , GM_HOMESCORE , GM_AWAYSCORE, GM_DATE) VALUES ('" + homeTeam + "','" + awayTeam + "','" + to_string(hScore) +
				"','" + to_string(aScore) + "','" + date + "');";
				rc = sqlite3_exec(NBA, sqlInsert3.c_str(), NULL , 0, &messageError);
				if( rc != SQLITE_OK ){
				cout << "SQL error: Something went wrong";
				sqlite3_free(sqlite3_errmsg);
				}
				else{
				cout << "Records created successfully\n";
				}
		}

	}
}
//aligns view data for divisions
int callback(void *data , int argc, char **argv , char **azColName){
int i;
for (i = 0; i < argc; i++){
cout << argv[i] << endl;
}
cout << endl;

return 0;

}
//function to "wash" the user input of special characters could also possibly detect people trying to use injection since the statement gets stored into the database
// but missing all the special characters to make it work *shrugs*
string sqlInjection(string input) {
input.erase(
remove(input.begin(), input.end(), '\"'),
input.end()
);
input.erase(
remove(input.begin(), input.end(), ';'),
input.end()
);
input.erase(
remove(input.begin(), input.end(), '*'),
input.end()
);

return input;

}
//takes the user pick and converts it to a string
string divisionPick(int divPick){
	string division;
	switch(divPick){
		case 1: division = "ATLANTIC"; break;
		case 2: division = "CENTRAL"; break;
		case 3: division = "SOUTHEAST"; break;
		case 4: division = "NORTHWEST"; break;
		case 5: division = "PACIFIC"; break;
		case 6: division = "SOUTHWEST"; break;
	}
	
	return division;
}

