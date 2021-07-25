/*
Program Name: Sale Co 
Author: Levi George (providing query, viewCustomer, and addInvoice code)
Date Last Updated: 11/9/2019
Program Purpose: To print out the results of a query from the program on the database
only the code that I have added is commented
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <stdlib.h>
#include "sqlite3.h"

using namespace std;

void printMainMenu();
void viewInvoice(sqlite3 *);
void viewCustomer(sqlite3 *);
void addInvoice(sqlite3 *);
int mainMenu();

int main()
{
	int choice;

	sqlite3 *mydb;

	int rc; 

	rc = sqlite3_open_v2("SaleCo.db", &mydb, SQLITE_OPEN_READWRITE, "win32");
	
	cout << "Welcome to SaleCo" << endl;
	choice = mainMenu();
	while (true)
	{
		switch (choice) 
		{
			case 1: viewInvoice(mydb); break;
			case 2: viewCustomer(mydb); break;
			case 3: addInvoice(mydb); break;
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

void printMainMenu() 
{
	cout << "Please choose an option (enter -1 to quit):  " << endl;
	cout << "1. View an invoice" << endl;
	cout << "2. View Customer Information" << endl;
	cout << "3. Add Invoice" << endl;
	cout << "Enter Choice: ";
}

int mainMenu()
{
	int choice = 0;
	
	printMainMenu();
	cin >> choice;
	while ((!cin || choice < 1 || choice > 3) && choice  != -1)
	{		
		if (!cin)
		{
			cin.clear();
			cin.ignore(1000,'\n');
		}
		cout << "That is not a valid choice." << endl << endl;
		printMainMenu();
		cin >> choice;
	} 
	return choice;
}

void viewInvoice(sqlite3 * db)
{
	string query = "SELECT INVOICE.INV_NUMBER, INVOICE.INV_DATE, CUSTOMER.CUS_FNAME, CUSTOMER.CUS_LNAME ";
	query += "FROM INVOICE JOIN CUSTOMER ON INVOICE.CUS_CODE = CUSTOMER.CUS_CODE;";
	sqlite3_stmt *pRes;
	string m_strLastError;
	string query2;
	string inv_number;
	string inv_date;
	string cus_fname,cus_lname;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)
	{
		m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(pRes);
		cout << "There was an error: " << m_strLastError << endl;
		return;
	}
	else
	{
		cout << "Please choose the invoice you want to see:" << endl;
		int columnCount = sqlite3_column_count(pRes);
		int i = 1, choice;
		sqlite3_stmt *pRes2;
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(pRes, 0);
			cout << endl;
			i++;
		}
		do
		{
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '/n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin);

		sqlite3_reset(pRes);
		for (int i = 0; i < choice; i++)
			sqlite3_step(pRes);
		inv_number = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0));
		inv_date = reinterpret_cast<const char*>(sqlite3_column_text(pRes,1));
		cus_fname = reinterpret_cast<const char*>(sqlite3_column_text(pRes,2));
		cus_lname = reinterpret_cast<const char*>(sqlite3_column_text(pRes,3));
		sqlite3_finalize(pRes);
		//need to provide the query to select the customers with the chosen region from the database
		query2 = "SELECT PRODUCT.P_DESCRIPT as Product ,LINE.LINE_PRICE as Price, LINE.LINE_UNITS as Units ";
		query2 += "FROM LINE ";
		query2 += "JOIN PRODUCT on line.P_CODE = PRODUCT.P_CODE  ";
		query2 += "WHERE LINE.INV_NUMBER = '" + inv_number + "';";

		if (sqlite3_prepare_v2(db, query2.c_str(), -1, &pRes2, NULL) != SQLITE_OK)
		{
			m_strLastError = sqlite3_errmsg(db);
			sqlite3_finalize(pRes2);
			cout << "There was an error: " << m_strLastError << endl;
			return;
		}
		else
		{
			cout << "Invoice #: " << inv_number << endl;
			cout << "Invoice Date: " << inv_date << endl;
			cout << "Customer: " << cus_fname << " " << cus_lname << endl;
			columnCount = sqlite3_column_count(pRes2);
			cout << left;
			for (int i = 0; i < columnCount; i++)
			{
				cout << "|" << setw(25) << sqlite3_column_name(pRes2, i);
			}
			cout << "|" << endl;
			
			while (sqlite3_step(pRes2) == SQLITE_ROW)
			{
				for (int i = 0; i < columnCount; i++)
				{
					if (sqlite3_column_type(pRes2, i) != SQLITE_NULL)
						cout << "|" << setw(25) << sqlite3_column_text(pRes2, i);
					else
						cout << "|" << setw(25) << " ";
				}
				cout << "|" << endl;
			}
			sqlite3_finalize(pRes2);
		}

	}
}

//This function accepts a database as it's parameter and performs operations on it.
void viewCustomer(sqlite3 * db)
{
	
	string query2, query = "SELECT CUS_CODE, CUS_FNAME, CUS_LNAME, CUS_AREACODE, CUS_PHONE, CUS_BALANCE "; 
	query += "FROM CUSTOMER ORDER BY CUS_CODE;";//Query prompts db for information
	sqlite3_stmt *prepState;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	string cus_fname,cus_lname, cus_ID, cus_areaCode, cus_phoneNum, cus_Bal;//these will hold values from the column text function
	
	//runs first query on the database, if the query fails it will notify the user of the error, 
	//otherwise the program will run like normal
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{	//Prompts user for a desired record 
		cout << "Please choose the customer record you want to review:" << endl;
		int numOfCol = sqlite3_column_count(prepState);//gets number of columns from running query on the database
		int i = 1, choice; //declares base value an the variable to hold the user's desired row number
		cout << left;
		while (sqlite3_step(prepState) == SQLITE_ROW)//While the step function returns a row 
		{
			//outputs text of the Customer Code column for each row
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " " << sqlite3_column_text(prepState, 1); 
			cout << " " << sqlite3_column_text(prepState, 2) << " ";
			cout << endl;
			i++;
		}
		do
		{
			//validates user input, ensuring they don't break the stream and enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '/n');
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin);
		
		
		sqlite3_reset(prepState);//returns prepared statement back to initial value
		
		
		for (int i = 0; i < choice; i++)
			sqlite3_step(prepState);
		
		//these reinterpret cast takes the data and turns it into a const char* type so that it can be printed more easily.
		cus_ID = reinterpret_cast<const char*>(sqlite3_column_text(prepState,0)); //returns the name of the CUS_CODE column to cus_ID
		cus_fname = reinterpret_cast<const char*>(sqlite3_column_text(prepState,1));//returns the name of the CUS_FNAME column to cus_fname
		cus_lname = reinterpret_cast<const char*>(sqlite3_column_text(prepState,2));//returns the name of the CUS_LNAME column to cus_lname 
		cus_areaCode = reinterpret_cast<const char*>(sqlite3_column_text(prepState,3));//returns the area code
		cus_phoneNum = reinterpret_cast<const char*>(sqlite3_column_text(prepState,4));//returns phone number
		cus_Bal = reinterpret_cast<const char*>(sqlite3_column_text(prepState,5));//returns customer balance
		
		sqlite3_finalize(prepState);//closes possible resource leaks from prepared Statement
		
		//displays the customer info
		cout << "Customer Code #: " << cus_ID << endl;
		cout << "Customer Name: " << cus_fname << " " << cus_lname << endl;
		cout << "Customer Phone Number: " << cus_areaCode << "-" << cus_phoneNum << endl;
		cout << "Customer Balance: $" << cus_Bal << endl;
		cout << left << "\n\n";
			
	}
}

//adds an invoice
void addInvoice(sqlite3 *db)
{
	//sql statements
	string sqlSelectCust = "SELECT CUS_FNAME, CUS_LNAME, CUS_CODE FROM CUSTOMER ORDER BY CUS_CODE;";
	string sqlInsertInvoice = "INSERT INTO INVOICE VALUES(";
	string sqlInsertLine = "INSERT INTO LINE VALUES(";
	string sqlSelectProduct = "SELECT P_CODE, P_DESCRIPT, P_PRICE FROM PRODUCT ORDER BY P_CODE;";
	string sqlSelectInv = "SELECT INV_NUMBER FROM INVOICE ORDER BY INV_NUMBER;";
	string ROLLBACK = "ROLLBACK;";
	string COMMIT = "COMMIT;";
	
	sqlite3_stmt *prepState, *prepRoll, *prepCommit;//This will hold a modified version of the query after use
	string errorStatement;//this will hold a error message from the database
	
	//this will be used to create the sql for line insertion
	int custNum = 10010, numOfItems = 0;
	int inv_number = 1001;
	double custBalance = 0;
	int i = 1, cusChoice;
		
	//this runs to make sure that the inventory number is brought up to the right value for the sql
	//ensures sql is good
	if(sqlite3_prepare_v2(db, sqlSelectInv.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//runs while there are invoices
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			inv_number++;
			
		}
		
		sqlite3_finalize(prepState);//closes resource leaks
	}
	
	//verifies sql statement is valid
	if(sqlite3_prepare_v2(db, sqlSelectCust.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		//input prompt
		cout << "What customer is opening the invoice?\n";
		
		//runs sql for the number of customers in the database
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			//outputs text of the Customer column for each row
			cout << i << ". " << sqlite3_column_text(prepState, 0) << " " << sqlite3_column_text(prepState, 1) << "";
			cout << endl;
			i++;
			
		}
		
		sqlite3_finalize(prepState);//closes resource leaks
		
	}//END Else
		
	//input validation for the chosen customer
	do
	{
			
		//validates user input, ensuring they don't break the stream and that they enter proper input
		if (!cin) 
		{
			cin.clear();
			cin.ignore(1000, '\n');
		}
		cin >> cusChoice;
		if (!cin || cusChoice < 1 || cusChoice > i)
			cout << "That is not a valid choice! Try Again!" << endl;
		
	} while (!cin || cusChoice < 1 || cusChoice > i);
		
	//sets customer number to -1 to make up for the i = 1 difference at the beginning
	custNum += cusChoice - 1;
	
	//Product Selection Branch
	cout << "How many products is the customer buying?\n";
		
	//input validation
	do
	{
			
		//validates user input, ensuring they don't break the stream and that they enter proper input
		if (!cin) 
		{
			cin.clear();
			cin.ignore(1000, '\n');
		}
		cin >> numOfItems;
		if (!cin || numOfItems < 1 || numOfItems > 16)
			cout << "That is not a valid choice! Try Again!" << endl;
	} while (!cin || numOfItems < 1 || numOfItems > 16);
		
	//Product Select SQL
	
	
	int prodNum[16] = {-1}; //holds the number of product 
	double prodCost[16] = {0}; //holds the product cost
	string prodCodes[16] = {" "};//holds product codes
	
	//validates sql
	if(sqlite3_prepare_v2(db, sqlSelectProduct.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		
		i = 1;
		
		while(sqlite3_step(prepState) == SQLITE_ROW)
		{
			//outputs description column for each row
			//reinterpret_cast<const char*>(sqlite3_column_text(prepState,0));
			
			//changes values to appropriate types
			prodCodes[i-1] = reinterpret_cast<const char*>(sqlite3_column_text(prepState,0));
			prodCost[i-1] = atof(reinterpret_cast<const char*>(sqlite3_column_text(prepState, 2)));
			//displays column names
			cout << i << ". " << sqlite3_column_text(prepState, 1) << "";
			cout << endl;
			i++;
		
		}
			
		sqlite3_finalize(prepState);//closes resource leaks
			
		
	}//END else	
	
	//input prompt
	cout << "\nWhat items would you like?\n";
	

	int prodChoice[16] = {-1}; //holds the user choices
	//gets then items the user wants
	//runs while the user wants an item
	for(int i = 0; i < numOfItems; i++)
	{
		cout << "Enter an item's number.\n";
			
		//input validation on the item chosen
		do
		{
		
			//validates user input, ensuring they don't break the stream and that they enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> prodChoice[i];
					
			if (!cin || prodChoice[i] < 1 || prodChoice[i] > 16)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin || prodChoice[i] < 1 || prodChoice[i] > 16);
				
		prodChoice[i] = prodChoice[i] - 1;//makes up for that i = 1 discrepancy logically
		cout << "How many would you like?\n";//prompt for input
				
		//input validation
		do
		{
			
			//validates user input, ensuring they don't break the stream and that they enter proper input
			if (!cin) 
			{
				cin.clear();
				cin.ignore(1000, '\n');
			}
			cin >> prodNum[i];
			if (!cin || prodNum[i] < 1)
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin || prodNum[i] < 1);
				
				
	}
	
	
	string sqlBeginTrans = "BEGIN TRANSACTION;";
	//SQL INSERT INVOICE
			
	//used to reference sql statement
	//sqlState2 = "INSERT INTO INVOICE VALUES(";
	//sqlState3 = "INSERT INTO LINE VALUES(";
			
	//builds sql
	sqlInsertInvoice = "INSERT INTO INVOICE VALUES( " + to_string(inv_number) + ", " + to_string(custNum) + ", date('now'));";
	
	
	if(sqlite3_prepare_v2(db, sqlBeginTrans.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
				
	}
	else
	{
		//runs the invoice sql
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);//closes resource leaks
				
	}
	
	
	//ensures that sql is valid
	if(sqlite3_prepare_v2(db, sqlInsertInvoice.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
				
		//ensures that the sql is valid, runs rollback if sql cannot be completed
		if(sqlite3_prepare_v2(db, ROLLBACK.c_str(), -1, &prepRoll, NULL) != SQLITE_OK)
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepRoll);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
				
			return;//finishes function
		}
		else
		{
					//runs the rollback in case of a failure
			sqlite3_step(prepRoll);
			sqlite3_finalize(prepRoll);//closes resource leaks
		}
				
		return;//finishes function since sql failed
	}
	else
	{
		//runs the invoice sql
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);//closes resource leaks
				
	}
			
	//cout << sqlInsertInvoice << "\n";		used for debugging
	
	//this for loop will run for the number of items that the user has ordered
	for(int i = 0; i < numOfItems; i++)
	{
				
		sqlInsertLine = "INSERT INTO LINE VALUES( " + to_string(inv_number) + ", " + to_string(i+1) + ", \"" + prodCodes[prodChoice[i]] + "\", \"";
		sqlInsertLine += to_string(prodNum[i]) + "\", " + to_string(prodNum[i] * prodCost[prodChoice[i]]) + ");";
				
		//cout << sqlInsertLine << "\n";	used for debugging
				
		custBalance += prodNum[i] * prodCost[prodChoice[i]];
				
		//ensures that the sql is valid, if it is not and can't be completed it will run the callback and close resource leaks
		if(sqlite3_prepare_v2(db, sqlInsertLine.c_str(), -1, &prepState, NULL) != SQLITE_OK)
		{
			errorStatement = sqlite3_errmsg(db);//gets error message
			sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
			cout << "There was an error: " << errorStatement << endl; //displays error message
			
			//This will run the rollback in case of failure
			if(sqlite3_prepare_v2(db, ROLLBACK.c_str(), -1, &prepRoll, NULL) != SQLITE_OK)
			{
				errorStatement = sqlite3_errmsg(db);//gets error message
				sqlite3_finalize(prepRoll);//finalizes statement to prevent data leaks
				cout << "There was an error: " << errorStatement << endl; //displays error message
		
				return;//finishes function
			}
			else
			{
				//runs an sql statement
				sqlite3_step(prepRoll);
				sqlite3_finalize(prepRoll);//closes resource leaks
			}
			
			return;//finishes function
		}
		else
		{
			//runs an sql statement
			sqlite3_step(prepState);
			sqlite3_finalize(prepState);//closes resource leaks
		}//END Else
	}//END For

	string sqlUpdateCusBal = "UPDATE CUSTOMER SET CUS_BALANCE = CUS_BALANCE +" + to_string(custBalance) + " WHERE CUS_CODE = " + to_string(custNum) + ";";
	
	//cout << sqlUpdateCusBal << "\n";   	this code was used to debug
	
	//updates cust balance, checks if sql is valid
	if(sqlite3_prepare_v2(db, sqlUpdateCusBal.c_str(), -1, &prepState, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepState);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		sqlite3_step(prepState);
		sqlite3_finalize(prepState);//closes resource leaks
	}

	//esnures that the the sql is valid and displays an error message if not
	if(sqlite3_prepare_v2(db, COMMIT.c_str(), -1, &prepCommit, NULL) != SQLITE_OK)
	{
		errorStatement = sqlite3_errmsg(db);//gets error message
		sqlite3_finalize(prepCommit);//finalizes statement to prevent data leaks
		cout << "There was an error: " << errorStatement << endl; //displays error message
		return;//finishes function
	}
	else
	{
		sqlite3_step(prepCommit);
		sqlite3_finalize(prepCommit);//closes resource leaks
	}
	
	
		
} //End function addInvoice


