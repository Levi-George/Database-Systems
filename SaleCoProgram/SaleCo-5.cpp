/* Program name: SaleCo.cpp
*  Author: Eric Jimenez
*  Date last updated: 12/8/2019
* Purpose: To add in a new invoice that has customer and product
*/
#include <iostream>
#include <string>
#include <iomanip>
#include "sqlite3.h"

using namespace std;

void printMainMenu(); //prototype function to printing main menu
void viewInvoice(sqlite3 *); //prototype function to viewing invoice
void viewCustomer(sqlite3 *); //prototype function for view customer details
void addInvoice (sqlite3 *); //prototype function to add invoice
int mainMenu(); //prototype main menu function checks for input validation

int main()
{
	int choice; //choice for viewing invoice or view customer info

	sqlite3 *mydb;  //points to database

	int rc;  //return code

	//Need to add code to open the database.
	rc = sqlite3_open_v2("SaleCo.db", &mydb,SQLITE_OPEN_READWRITE, "win32");
	//rc = return code
	if(rc)
	{
		cout << "Error opening SaleCo database: " << sqlite3_errmsg(mydb) << endl; //returns errmsg if SaleCo does not open properly
		return 0;
	}else
	{
		cout << "SaleCo database opened successfully" << endl; // SaleCo database open sucessfully
	}
	cout << "Welcome to SaleCo" << endl; //welcome message in the database
	choice = mainMenu(); //prints out a main menu
	while (true) //loop repeats until not true where -1 is entered
	{
		switch (choice) //switch statement options
		{
			case 1: viewInvoice(mydb); break;
			case 2: viewCustomer(mydb); break;
			case 3: addInvoice(mydb); break;
			case -1: return 0;
			default: cout << "That is not a valid choice." << endl;
		}
		cout << "\n\n"; //new line characters to space out from main menu
		choice = mainMenu(); 
	}
	
	sqlite3_close(mydb);
	
}

//prints main menu options where you get choices to decide upon
void printMainMenu() 
{
	cout << "Please choose an option (enter -1 to quit):  " << endl;
	cout << "1. View an invoice" << endl;
	cout << "2. View Customer Information" << endl;
	cout << "3. Add in an invoice" << endl;
	cout << "Enter Choice: ";
}

//function that checks for input validation for main menu 
int mainMenu()
{
	int choice = 0;
	
	printMainMenu(); //prints main menu
	cin >> choice; //gets choice from user
	while ((!cin || choice < 1 || choice > 3) && choice  != -1) //while loop for input validation checking if user inputs correct options
	{		
		if (!cin) //checks if user doesnt provide correct input and resets the input stream
		{
			cin.clear(); //clears errors
			cin.ignore(1000,'\n');
		}
		cout << "That is not a valid choice." << endl << endl; //statement for invalid choice
		printMainMenu(); //re-prints main menu
		cin >> choice; //gets choice again from user
	} 
	return choice;
}

//function for viewing the invoice
void viewInvoice(sqlite3 * db)
{
	string query = "SELECT INVOICE.INV_NUMBER, INVOICE.INV_DATE, CUSTOMER.CUS_FNAME, CUSTOMER.CUS_LNAME "; //gets invoice number, date, customer fname and lname
	query += "FROM INVOICE JOIN CUSTOMER ON INVOICE.CUS_CODE = CUSTOMER.CUS_CODE;"; //joins invoice and customer table
	sqlite3_stmt *pRes; //holds result set
	string m_strLastError; //error message variable
	string query2;
	string inv_number; //from invoice table
	string inv_date;
	string cus_fname,cus_lname; //from customer table
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK) //calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		m_strLastError = sqlite3_errmsg(db); // sqlite errmsg command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << m_strLastError << endl; //prints error msg
		return;
	}
	else
	{
		cout << "Please choose the invoice you want to see:" << endl;
		int columnCount = sqlite3_column_count(pRes); //gets column count
		int i = 1, choice;
		sqlite3_stmt *pRes2;
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW) //runs through results in the result set
		{
			cout << i << ". " << sqlite3_column_text(pRes, 0); //prints the result in the result set
			cout << endl;
			i++;
		}
		do
		{
			if (!cin)  //do while checks for input failure
			{
				cin.clear(); //resets the input stream 
				cin.ignore(1000, '\n'); 
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i) //checks if its a choice within the menu
				cout << "That is not a valid choice! Try Again!" << endl;
		} while (!cin);

		sqlite3_reset(pRes); //resets result set to run through it again
		for (int i = 0; i < choice; i++)
			sqlite3_step(pRes); //steps to the choice user makes
		inv_number = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0)); //sets invoice num //reinterpret_cast fixes errors
		inv_date = reinterpret_cast<const char*>(sqlite3_column_text(pRes,1)); //sets invoice date
		cus_fname = reinterpret_cast<const char*>(sqlite3_column_text(pRes,2)); //sets cus_fname
		cus_lname = reinterpret_cast<const char*>(sqlite3_column_text(pRes,3)); //sets cus_lname
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		//finalizes query with options listed below
		query2 = "SELECT PRODUCT.P_DESCRIPT as Product ,LINE.LINE_PRICE as Price, LINE.LINE_UNITS as Units ";
		query2 += "FROM LINE ";
		query2 += "JOIN PRODUCT on line.P_CODE = PRODUCT.P_CODE  ";
		query2 += "WHERE LINE.INV_NUMBER = '" + inv_number + "';";

		if (sqlite3_prepare_v2(db, query2.c_str(), -1, &pRes2, NULL) != SQLITE_OK) //checks for errors 
		{
			m_strLastError = sqlite3_errmsg(db);
			sqlite3_finalize(pRes2);
			cout << "There was an error: " << m_strLastError << endl; //prints error msg
			return;
		}
		else //no errors prints #, date, and customer name
		{
			cout << "Invoice #: " << inv_number << endl;
			cout << "Invoice Date: " << inv_date << endl;
			cout << "Customer: " << cus_fname << " " << cus_lname << endl;
			columnCount = sqlite3_column_count(pRes2);
			cout << left;
			for (int i = 0; i < columnCount; i++) //loops through column count
			{
				cout << "|" << setw(25) << sqlite3_column_name(pRes2, i); //sets width
			}
			cout << "|" << endl; //lines for columns
			
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
			sqlite3_finalize(pRes2); //frees memory used by prepared statement
		}

	}
}

//function for viewing customer info in the database
void viewCustomer(sqlite3 * db)
{ 
	string query1 = "SELECT CUS_CODE, CUS_FNAME, CUS_LNAME, CUS_AREACODE, CUS_PHONE, CUS_BALANCE FROM CUSTOMER ORDER BY CUS_CODE;"; //selects customer code, customer fname and lname, areacode, phone and balance from the customer table
	sqlite3_stmt *pRes; //holds result set
	string sqlError; //error message
	string query2; 
	string cus_code, cus_fname,cus_lname, cus_areacode, cus_phone, cus_balance; //info needed from database
	
	if (sqlite3_prepare_v2(db, query1.c_str(), -1, &pRes, NULL) != SQLITE_OK) //calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); // sqlite error message command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //prints error msg
		return;
	}
	else
	{
		cout << "Select the customer you want to see:" << endl;
		int columnCount = sqlite3_column_count(pRes); //gets column count
		int i = 1, choice;
		sqlite3_stmt *pRes2;
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW) //runs through results in the result set
		{
			cout << i << ". " << sqlite3_column_text(pRes, 0); //prints the result in the result set
			cout << endl;
			i++; 
		}
		do
		{
			if (!cin)  //do while checks for input failure
			{
				cin.clear(); //resets the input stream 
				cin.ignore(1000, '\n'); 
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i) //checks if its a choice within the menu
				cout << "That is not a valid choice! Try Again!" << endl; //prints statement if invalid choice
		} while (!cin);

		sqlite3_reset(pRes); //resets result set to run through it again
		
		for (int i = 0; i < choice; i++)
			sqlite3_step(pRes); //steps to the choices user makes
		
		cus_code = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0)); //sets cus_code //reinterpret_cast fixes errors
		cus_fname = reinterpret_cast<const char*>(sqlite3_column_text(pRes,1)); //sets cus_fname
		cus_lname = reinterpret_cast<const char*>(sqlite3_column_text(pRes,2)); //sets cus_lname
		cus_areacode = reinterpret_cast<const char*>(sqlite3_column_text(pRes,3)); //sets cus_areacode
		cus_phone = reinterpret_cast<const char*>(sqlite3_column_text(pRes,4)); //sets cus_phone
		cus_balance = reinterpret_cast<const char*>(sqlite3_column_text(pRes,5)); //sets cus_balance
		
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		
		query2 = "SELECT CUS_FNAME, CUS_LNAME, CUS_AREACODE, CUS_PHONE, CUS_BALANCE FROM CUSTOMER WHERE CUS_CODE = " + cus_code + ";";

		if (sqlite3_prepare_v2(db, query2.c_str(), -1, &pRes2, NULL) != SQLITE_OK) //prepares sql statement
		{
			sqlError = sqlite3_errmsg(db); //error with database
			sqlite3_finalize(pRes2);
			cout << "There was an unexpected error: " << sqlError << endl; //prints error msg
			return;
		}
		else //customer info found
		{
			cout << "Here is what we found for Customer ID#: " << cus_code << endl;
			cout << endl;
			cout << "Name: " << cus_fname << " " << cus_lname << endl;
			cout << "Area Code: " << cus_areacode << endl;
			cout << "Phone #: " << cus_phone << endl;
			cout << "Balance: " << cus_balance << endl;
			cout << endl;
			cout << "---Displaying customer info in table form--- " << endl; //displays customer info in table form
			columnCount = sqlite3_column_count(pRes2); //count the number of columns in the prepare statement
			cout << left;
			for (int i = 0; i < columnCount; i++) //loops through the columns and displays customer info in table like form
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
			sqlite3_finalize(pRes2); //frees memory
		}

	}
	
}
void addInvoice(sqlite3 *db)
{
	//Query for getting all the customers
	string query3 = "SELECT CUS_CODE, CUS_FNAME, CUS_LNAME FROM CUSTOMER;"; //Selects customer code, fname, and lname from customer table
	sqlite3_stmt *pRes;
	string cus_code, cus_fname, cus_lname, p_code, p_descript, inv_number;
	string sqlError;
		if (sqlite3_prepare_v2(db, query3.c_str(), -1, &pRes, NULL) != SQLITE_OK) //calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); // sqlite error message command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //prints error msg
		return;
	}
	else
	{
		cout << "Choose a customer to be added to the invoice:\n" << endl;
		int columnCount = sqlite3_column_count(pRes); //gets column count
		int i = 1, choice;
		sqlite3_stmt *pRes2;
		cout << left;
		cout << "CUSTOMER ID     CUSTOMER Name" << endl;
		cout << " ----------     -------------" << endl;
		while (sqlite3_step(pRes) == SQLITE_ROW) //runs through results in the result set
		{
			cout << i << ".) " << sqlite3_column_text(pRes, 0) << "\t " << sqlite3_column_text(pRes, 1) << " " << sqlite3_column_text(pRes, 2); //prints the result in the result set
			cout << endl;
			i++; 
		}
		do
		{
			if (!cin)  //do while checks for input failure
			{
				cin.clear(); //resets the input stream 
				cin.ignore(1000, '\n'); 
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i) //checks if its a choice within the menu
				cout << "That is not a valid choice! Try Again!" << endl; //prints statement if invalid choice
		} while (!cin);
		
	string query4 = "SELECT P_CODE, P_DESCRIPT FROM PRODUCT;"; //Selects product code, description from product table	
	if (sqlite3_prepare_v2(db, query4.c_str(), -1, &pRes, NULL) != SQLITE_OK) //calls sqlite3_prepare_v2 function to execute statement on the database and checks for errors
	{
		sqlError = sqlite3_errmsg(db); // sqlite error message command
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		cout << "There was an error: " << sqlError << endl; //prints error msg
		return;
	}
	else
	{
		cout << "Now choose a product to be added to the invoice:\n" << endl;
		int columnCount = sqlite3_column_count(pRes); //gets column count
		int i = 1, choice;
		sqlite3_stmt *pRes2;
		cout << left;
		cout << "PRODUCT ID     PRODUCT Description" << endl;
		cout << " --------      -------------------" << endl;
		while (sqlite3_step(pRes) == SQLITE_ROW) //runs through results in the result set
		{
			cout << i << ".) " << sqlite3_column_text(pRes, 0) << "\t " << sqlite3_column_text(pRes, 1); //prints the result in the result set
			cout << endl;
			i++; 
		}
		do
		{
			if (!cin)  //do while checks for input failure
			{
				cin.clear(); //resets the input stream 
				cin.ignore(1000, '\n'); 
			}
			cin >> choice;
			if (!cin || choice < 1 || choice > i) //checks if its a choice within the menu
				cout << "That is not a valid choice! Try Again!" << endl; //prints statement if invalid choice
		} while (!cin);
	
		sqlite3_reset(pRes); //resets result set to run through it again
		
		for (int i = 0; i < choice; i++)
			sqlite3_step(pRes); //steps to the choices user makes
		
		cus_code = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0)); //sets cus_code //reinterpret_cast fixes errors
		p_code = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0)); //sets p_code //reinterpret_cast fixes errors
		sqlite3_finalize(pRes); //frees memory used by prepared statement
		
		string query5 = "SELECT INV_NUMBER FROM Invoice;"; //selects the invoice number from the invoice table
		//query5 += "FROM INVOICE JOIN PRODUCT ON INVOICE.P_CODE = PRODUCT.P_CODE;"; //tried joining the P_CODE to the Invoice table
			if (sqlite3_prepare_v2(db, query5.c_str(), -1, &pRes2, NULL) != SQLITE_OK) //prepares sql statement
		{
			sqlError = sqlite3_errmsg(db); //error with database
			sqlite3_finalize(pRes2);
			cout << "There was an unexpected error: " << sqlError << endl; //prints error msg
			return;
		}
		else
		{
			int count = 1;
			while(sqlite3_step(pRes2) == SQLITE_ROW)
			{
				count++;
			}
			for (int i = 0; i < count - 1; i++)
			{
				sqlite3_step(pRes2);
			}
			inv_number = to_string(sqlite3_column_int(pRes2,0) + 1); //new invoice added in after prior invoices
			cout << "A new invoice has been added: " << inv_number << endl; //prints statement if invoice has been added in
			sqlite3_finalize(pRes2);
			
			sqlite3_stmt *pRes3;
			//inserts the values for the new invoice
			string new_invoice = "INSERT INTO INVOICE(INV_NUMBER, CUS_CODE, P_CODE, INV_DATE) VALUES(" + inv_number + ", " + cus_code + ", " + p_code +  ", date('now'))";
			
			if (sqlite3_prepare_v2(db, new_invoice.c_str(), -1, &pRes3, NULL) != SQLITE_OK)
			{
				sqlite3_finalize(pRes3); //frees memory used by prepared statement
				cout << "There was an error: " << sqlError << endl; //error if database can insert the data correctly
				return;
			}
			else
			{
				if(sqlite3_step(pRes3))
				{
					cout << "The invoice has been successfully added in." << endl; //prints if invoice has been successfully added
					sqlite3_finalize(pRes3); //frees memory used by prepared statement
				}
			}
			
			
		}	
	}
}
}
