/* Program name: SaleCo.cpp
   Author: Cory Knoll
   Date last modified: 12/6/2019
   Purpose: Allow a user to add and view an invoice of their choice and review customer information
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <map>
#include "sqlite3.h"

using namespace std;

void printMainMenu();
void viewInvoice(sqlite3 *);
void viewCustomer(sqlite3 *);
void addInvoice(sqlite3 *);
int mainMenu();
void clearbuffer();
template<class numType> numType verifyNumPrompt(numType, numType, string);

int main()
{
	int choice;

	sqlite3 *mydb;

	int rc; 

	//attempt to open database
	rc = sqlite3_open_v2("SaleCo.db", &mydb, SQLITE_OPEN_READWRITE, NULL);

	//print an error or success message, if error: return
	rc ? fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(mydb)) : fprintf(stderr, "Opened database successfully.\n\n");
	if (rc)
		return(0);
	
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
	
}

void printMainMenu() 
{
	cout << "Please choose an option (enter -1 to quit):  " << endl;
	cout << "1. View an invoice" << endl;
	cout << "2. View Customer Information" << endl;
	cout << "3. Add an invoice" << endl;
	cout << "Enter Choice: ";
}

int mainMenu()
{
	int choice = 0;
	
	while ((choice < 1 || choice > 3) && choice  != -1)
	{
		printMainMenu();
		while (!(cin >> choice))
		{
			cin.clear();
			cin.ignore(1000,'/n');
			cout << "That is not a valid choice." << endl << endl;
		}
		if((choice < 1 || choice > 3) && choice != -1)
			cout << "That is not a valid choice." << endl << endl;
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
		int i = 1, choice = 0;
		sqlite3_stmt *pRes2;
		cout << left;
		while (sqlite3_step(pRes) == SQLITE_ROW)
		{
			cout << i << ". " << sqlite3_column_text(pRes, 0);
			cout << endl;
			i++;
		}
		while (choice < 1 || choice > i-1)
		{
			while (!(cin >> choice))
			{
				cin.clear();
				cin.ignore(1000, '\n');
				cout << "That is not a valid choice! Try Again!" << endl;
			}
			if (choice < 1 || choice > i-1)
				cout << "That is not a valid choice! Try Again!" << endl;
		}

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

void viewCustomer(sqlite3 *db)
{
	string query;			//sql statement to get list of customers for prompt
	sqlite3_stmt* pRes;		//results of query
	string m_strLastError;	//error message from running query

	//set query to get customer names and IDs
	query = "SELECT CUS_CODE, CUS_FNAME, CUS_LNAME FROM CUSTOMER;";

	//input query
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)
	{
		//error occured, output error message, finalize pRes
		m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(pRes);
		cout << "There was an error: " << m_strLastError << endl;
		return;
	}
	else
	{
		//no errors
		int i = 1;				//increments to count results of query
		int choice = 0;			//user choice from menu options
		string query2;			//sql statment to get data on chosen customer
		string cus_code;		//id of chosen customer
		sqlite3_stmt* pRes2;	//results of query2

		//prompt for customer selection
		cout << "Please choose the customer you want to see:" << endl;
		while (sqlite3_step(pRes) == SQLITE_ROW)
		{
			cout << i << ". #" << sqlite3_column_text(pRes, 0)
				<< " - " << sqlite3_column_text(pRes, 1)
				<< " " << sqlite3_column_text(pRes, 2) << endl;
			i++;
		}

		//validate type and bounds of user input
		choice = verifyNumPrompt<int>(1, i - 1, "Your selection: ");

		//iterate pRes to chosen customer and save customer code
		sqlite3_reset(pRes);
		for (int i = 0; i < choice; i++)
			sqlite3_step(pRes);
		cus_code = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0));
		sqlite3_finalize(pRes);

		//need to provide the query to select the customers with the chosen region from the database
		query2 = "SELECT CUS_FNAME, CUS_LNAME, CUS_AREACODE, CUS_PHONE, CUS_BALANCE ";
		query2 += "FROM CUSTOMER ";
		query2 += "WHERE CUS_CODE = '" + cus_code + "';";

		//input query
		if (sqlite3_prepare_v2(db, query2.c_str(), -1, &pRes2, NULL) != SQLITE_OK)
		{
			//error occured, output error message, finalize pRes2
			m_strLastError = sqlite3_errmsg(db);
			sqlite3_finalize(pRes2);
			cout << "There was an error: " << m_strLastError << endl;
			return;
		}
		else
		{
			//no errors, output results of query and finalize pRes2
			while(sqlite3_step(pRes2) == SQLITE_ROW)
				cout << "Customer: " << sqlite3_column_text(pRes2, 0)
					<< " " << sqlite3_column_text(pRes2, 1) << endl
					<< "Phone: (" << sqlite3_column_text(pRes2, 2)
					<< ") " << sqlite3_column_text(pRes2, 3) << endl
					<< "Balance: $" << sqlite3_column_text(pRes2, 4) << endl;
			sqlite3_finalize(pRes2);
		}
	}
}


void addInvoice(sqlite3* db) {
	string query;			//sql statement to get list of customers for prompt
	sqlite3_stmt* pRes;		//results of query
	string m_strLastError;	//error message from running query

	//set query to get customer names and IDs
	query = "SELECT CUS_CODE, CUS_FNAME, CUS_LNAME FROM CUSTOMER;";

	//input query
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &pRes, NULL) != SQLITE_OK)
	{
		//error occured, output error message, finalize pRes
		m_strLastError = sqlite3_errmsg(db);
		sqlite3_finalize(pRes);
		cout << "There was an error: " << m_strLastError << endl;
		return;
	}
	else
	{
		//no errors
		int i = 1;						//increments to count results of query
		int choice = 0;					//user choice from menu options
		string cus_code;				//id of chosen customer
		string query2;					//sql statment to get data on chosen customer
		sqlite3_stmt* pRes2 = nullptr;	//results of query2
		string query3;					//sql statment to submite invoice transaction
		sqlite3_stmt* pRes3;			//results of query3
		bool order_incomplete = true;	//boolean that's true when more order details to be entered
		int line_num = 1;				//line number of order
		map<int, int> orders;			//map storing the order amount for each product line thus far
		map<int, int>::iterator it;		//iterator for orders map

		//prompt for customer selection
		cout << endl << "Please choose the customer to invoice:" << endl;
		while (sqlite3_step(pRes) == SQLITE_ROW)
		{
			cout << i << ". #" << sqlite3_column_text(pRes, 0)
				<< " - " << sqlite3_column_text(pRes, 1)
				<< " " << sqlite3_column_text(pRes, 2) << endl;
			i++;
		}

		//validate type and bounds of user input
		choice = verifyNumPrompt<int>(1, i - 1, "Your selection: ");

		//iterate pRes to chosen customer and save customer code
		sqlite3_reset(pRes);
		for (int i = 0; i < choice; i++)
			sqlite3_step(pRes);
		cus_code = reinterpret_cast<const char*>(sqlite3_column_text(pRes, 0));
		sqlite3_finalize(pRes);

		//need to provide the query to select the product from the available options
		query2 = "SELECT P_CODE, P_DESCRIPT, P_QOH ";
		query2 += "FROM PRODUCT ";
		query2 += "WHERE P_QOH > 0;";

		//begin building transaction with begin command and inserting the new invoice
		query3 = "BEGIN TRANSACTION; ";
		query3 += "INSERT INTO INVOICE (INV_NUMBER, CUS_CODE, INV_DATE) ";
		query3 += "VALUES ((SELECT MAX(INV_NUMBER) FROM INVOICE) + 1, '" + cus_code + "', (SELECT date('now'))); ";

		while (order_incomplete) {

			//input query
			if (sqlite3_prepare_v2(db, query2.c_str(), -1, &pRes2, NULL) != SQLITE_OK)
			{
				//error occured, output error message, finalize pRes2
				m_strLastError = sqlite3_errmsg(db);
				sqlite3_finalize(pRes2);
				cout << "There was an error: " << m_strLastError << endl;
				return;
			}
			else
			{
				//no errors
				i = 1;						//increments to count results of query
				choice = 0;					//user choice from menu options
				string pro_code;			//id of chosen product
				int quantity_on_hand;		//number of chosen product available
				int order_amount;			//number of chosen product to be added to invoice

				//prompt for product selection
				cout << endl << "Please choose the product to add to your order:" << endl;

				//display resultant products in a menu (either display quanity on hand or a notice that they product is already selected)
				while (sqlite3_step(pRes2) == SQLITE_ROW)
				{
					it = orders.find(i);
					if (it == orders.end())
						cout << i << ". #" << sqlite3_column_text(pRes2, 0)
						<< " - " << sqlite3_column_text(pRes2, 1)
						<< " ( " << sqlite3_column_text(pRes2, 2)
						<< " remaining on hand)" << endl;
					else
						cout << i << ". #" << sqlite3_column_text(pRes2, 0)
						<< " - " << sqlite3_column_text(pRes2, 1)
						<< " (ALREADY ORDERED)" << endl;
					i++;
				}
				cout << i << ". NO MORE PRODUCTS TO ADD, SUBMIT ORDER." << endl;

				//validate type and bounds of user input and make sure product hasn't been selected before
				do {
					choice = verifyNumPrompt<int>(1, i, "Your selection: ");
					it = orders.find(choice);
					if (it != orders.end())
						cout << endl << "\tProduct already added to order, chose a different product." << endl << endl;
				} while (it != orders.end());

				//determine if user chose to end order, else get quantity of product
				if (choice == i)
					order_incomplete = false;
				else {
					//iterate pRes2 to chosen customer and save product code and quantity on hand
					sqlite3_reset(pRes2);
					for (int i = 0; i < choice; i++)
						sqlite3_step(pRes2);
					pro_code = reinterpret_cast<const char*>(sqlite3_column_text(pRes2, 0));
					quantity_on_hand = atoi(reinterpret_cast<const char*>(sqlite3_column_text(pRes2, 2)));

					//get and validate order amount (must be a positive number no more than the quantity on hand),
					//add product and amount to order map
					order_amount = verifyNumPrompt<int>(0, quantity_on_hand, "Quantity of product '" + pro_code + "' to order (" + to_string(quantity_on_hand) + " units on hand): ");
					orders.insert(pair<int, int>(choice, order_amount));

					//add sql instructions to insert the line item including price from quantity*price*(1-discount) for the product order
					//and update the quantity on hand
					query3 += "INSERT INTO LINE(INV_NUMBER, LINE_NUMBER, P_CODE, LINE_UNITS, LINE_PRICE) ";
					query3 += "VALUES ((SELECT MAX(INV_NUMBER) FROM INVOICE), " + to_string(line_num) + ", '";
					query3 += pro_code + "', " + to_string(order_amount) + ", " + to_string(order_amount);
					query3 += " * (SELECT P_PRICE FROM PRODUCT WHERE P_CODE = '" + pro_code + "') *";
					query3 += " ( 1 - (SELECT P_DISCOUNT FROM PRODUCT WHERE P_CODE = '" + pro_code + "'))); ";
					query3 += "UPDATE PRODUCT ";
					query3 += "SET P_QOH = P_QOH - " + to_string(order_amount) + " ";
					query3 += "WHERE P_CODE = '" + pro_code + "'; ";
					line_num++;
				}
			}
		}

		//increase the customer's balance by the sum of each line price for the order
		query3 += "UPDATE CUSTOMER ";
		query3 += "SET CUS_BALANCE = CUS_BALANCE + ";
		query3 += "(SELECT SUM(LINE.LINE_PRICE) ";
		query3 += "FROM LINE ";
		query3 += "WHERE INV_NUMBER = ";
		query3 += "(SELECT MAX(INV_NUMBER) FROM INVOICE)) ";
		query3 += "WHERE CUS_CODE = '" + cus_code + "';";
		query3 += "COMMIT;";

		//execute transaction
		sqlite3_exec(db, query3.c_str(), NULL, NULL, NULL);
	}
}

void clearbuffer()
{
	//clear cin buffer of error flags and ignore rest of line (up to 2000 characters)
	cin.clear();
	cin.ignore(2000, '\n');
}

template<class numType>
numType verifyNumPrompt(numType low, numType high, string prompt)
{
	numType input;	//some type of numerical input

	//get and verify input is correct type and within bounds
	do
	{
		cout << prompt;
		//will verify that input matches type, else will display error and reprompt
		while (!(cin >> input))
		{
			clearbuffer();
			cout << endl << "\tInvalid type. Try again." << endl << endl << prompt;
		}
		clearbuffer(); //clears error flags and/or excess input

		//display an error message if input is too high or too low, then reprompt
		if (input < low|| input > high)
			cout << endl << "\tInput out of bound. Try again." << endl << endl;
	} while (input < low || input > high);

	return input;
}