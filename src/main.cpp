#include <iostream> // for basic input output 
#include <stdlib.h> // to get an 'exit()' function.
#include <ctime> // to get the current time o save to ordering detail.
#include <cstdlib> // for changing the string to const* char data type.
#include <stdexcept> // for handling error messages 'cerr'.
#include <queue> // to handle queue data structure easily.
#include <stack> // to handle stack data stracture easily.
#include <string> // to handle string.
#include <sqlite3.h> // to handle sqlite database opening executing and so on.

using namespace std; // for standard template library

string userEmail;

static int callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        userEmail = argv[3];
    }
    return 0;
}

time_t currentTime = time(nullptr);
string timeString = ctime(&currentTime);
tm *currentDate = localtime(&currentTime);

struct Product_Info
{
    string Name;
    int Price;
    string Description;
};

struct Order_Detail
{
    string Product_Name;
    int Price;
    string Created_Date;
};

stack<Product_Info> Product_Info_Stack;
Product_Info product;

queue<Order_Detail> Order_Detail_Queue;
Order_Detail order;

void aboutUs()
{
    cout << " -----------------------------------------" << endl;
    cout << " #             ABOUT US                  #" << endl;
    cout << " -----------------------------------------" << endl;

    cout <<"|-----------------------------------------|"<<endl;
    cout <<"| "<<"Name"<<"                   |      "<<"Id No"<<"     |"<<endl;
    cout <<"|-----------------------------------------|"<<endl;
    cout <<"| "<<"Natnael Tesfaye"<<"        |      "<<"URC112/14"<<" |"<<endl;
    cout <<"| "<<"Yared Kiros"<<"            |      "<<"URC119/14"<<" |"<<endl;
    cout <<"| "<<"Mickyas Tesfaye"<<"        |      "<<"URC109/14"<<" |"<<endl;
    cout <<"|-----------------------------------------|"<<endl;

}

void userLogin()
{
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    if (rc)
    {
        cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << endl;
    }
    string email;
    string password;
    cout << "--------------------------------------" << endl;
    cout << "                 LOGIN                " << endl;
    cout << "--------------------------------------" << endl;
    cout << "Enter Your Email: ";
    cin >> email;
    cout << "Enter Your Password: ";
    cin >> password;

    // Prepare SQL statement to check user credentials
    string sql = "SELECT * FROM Users WHERE email='" + email + "' AND password='" + password + "'";
    char *errMsg = nullptr;

    // Execute the query
    rc = sqlite3_exec(db, sql.c_str(), callback, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        cerr << "Error executing SQLite query: " << errMsg << endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
    }
    // Check if login was successful
    if (userEmail.empty())
    {
        cout << "Invalid email or password." << endl;
        sqlite3_close(db);
    }

    // Close the database
    sqlite3_close(db);
}

void userRegister()
{
    string email;
    string password;
    string username;
    cout << "--------------------------------------" << endl;
    cout << "               REGISTER               " << endl;
    cout << "--------------------------------------" << endl;
    cout << "Enter User Name: ";
    cin >> username;
    cout << "Enter Your Email: ";
    cin >> email;
    cout << "Enter Your Password: ";
    cin >> password;

    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    char *errMsg = nullptr;
    const char *sql;

    if (rc != SQLITE_OK)
    {
        cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
    }
    sql = "INSERT INTO Users (Email, Password, Username) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(0);
    }
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, username.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        cout << "Error binding parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        cout << "Error executing statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    sqlite3_finalize(stmt);
    userLogin();
}

void printProducts()
{
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    char *errMsg = nullptr;
    const char *sql;
    cout << "--------------------------------------" << endl;
    cout << "               PRODUCTS               " << endl;
    cout << "--------------------------------------" << endl;
    cout << "Id"
         << " | "
         << "Name"
         << " | "
         << "Price"
         << " | "
         << "Description" << endl;
    cout << "--------------------------------------" << endl;
    sql = "SELECT * FROM product_info;";
    rc = sqlite3_exec(
        db, sql, [](void *data, int argc, char **argv, char ** /*colNames*/) -> int
        {
        for (int i = 0; i < argc; i++) {
            cout << argv[i] << " | ";
        }
        cout << endl;
        return 0; },
        nullptr, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

void printOrders()
{
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    char *errMsg = nullptr;
    cout << "--------------------------------------" << endl;
    cout << "            ONGOING ORDERS            " << endl;
    cout << "--------------------------------------" << endl;
    cout << "Id"
         << " | "
         << "Product Id"
         << " | "
         << "Date" << endl;
    cout << "--------------------------------------" << endl;
    const char *sql = "SELECT * FROM order_details;";
    rc = sqlite3_exec(
        db, sql, [](void *data, int argc, char **argv, char ** /*colNames*/) -> int
        {
        for (int i = 0; i < argc; i++) {
            cout << argv[i] << " | ";
        }
        cout << endl;
        return 0; },
        nullptr, &errMsg);

    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

void add_product()
{
    if (userEmail.empty())
    {
        cout << "Register / Login First To Order A Product" << endl;
        int usnum;
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "Enter a Number To Perform: ";
        cin >> usnum;
        switch (usnum)
        {
        case 1:
            userRegister();
            break;
        case 2:
            userLogin();
        default:
            cout << "Invalid Input Enter a Valid Number" << endl;
        }
    }
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    char *errMsg = nullptr;
    const char *sql;
    string product_name;
    int product_price;
    string product_description;
    cout << "--------------------------------------" << endl;
    cout << "              ADD PRODUCTS             " << endl;
    cout << "--------------------------------------" << endl;
    cout << "Enter Product Name: ";
    cin >> product_name;
    cout << "Enter Product Price: ";
    cin >> product_price;
    cout << "Enter Product Description: ";
    cin >> product_description;
    cout << "--------------------------------------" << endl;
    product = {product_name, product_price, product_description};
    Product_Info_Stack.push(product);
    if (rc != SQLITE_OK)
    {
        cout << "Error opening database: " << sqlite3_errmsg(db) << endl;
    }
    sql = "INSERT INTO Product_Info (product_name, product_price, product_description) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(0);
    }
    sqlite3_bind_text(stmt, 1, product_name.c_str(), -1, SQLITE_STATIC);
    string price = to_string(product_price);
    sqlite3_bind_text(stmt, 2, price.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, product_description.c_str(), -1, SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        cout << "Error binding parameter: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        cout << "Error executing statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
    sqlite3_finalize(stmt);
}

void order_product()
{

    if (userEmail.empty())
    {
        cout << "Register / Login First To Order A Product" << endl;
        int usnum;
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "Enter a Number To Perform: ";
        cin >> usnum;
        switch (usnum)
        {
        case 1:
            userRegister();
            break;
        case 2:
            userLogin();
        default:
            cout << "Invalid Input Enter a Valid Number" << endl;
        }
    }

    string product_name;
    int product_price;
    string id;
    printProducts();
    cout << endl;
    cout << "--------------------------------------" << endl;
    cout << "            ORDER PRODUCT             " << endl;
    cout << "--------------------------------------" << endl;
    char productName[100]; // Declare as char array
    cout << "Enter product Name The Same As Shown Above: ";
    cin >> productName;
    cout << "--------------------------------------" << endl;
    sqlite3 *db;
    char *errMsg = nullptr;
    int rc;

    rc = sqlite3_open("main.db", &db);
    if (rc != SQLITE_OK)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
    }

    string sql = "SELECT id FROM product_info WHERE product_name = ?";
    sqlite3_stmt *stmt;

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    rc = sqlite3_bind_text(stmt, 1, productName, -1, SQLITE_STATIC);

    if (rc != SQLITE_OK)
    {
        cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
    }

    rc = sqlite3_step(stmt);

    if (rc == SQLITE_ROW)
    {
        int productId = sqlite3_column_int(stmt, 0);
        string insertSql = "INSERT INTO order_details (product_id, created_date) VALUES (?, datetime('now'));";
        sqlite3_stmt *insertStmt;

        rc = sqlite3_prepare_v2(db, insertSql.c_str(), -1, &insertStmt, nullptr);

        if (rc != SQLITE_OK)
        {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        rc = sqlite3_bind_int(insertStmt, 1, productId);

        if (rc != SQLITE_OK)
        {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }

        rc = sqlite3_step(insertStmt);

        if (rc != SQLITE_DONE)
        {
            cerr << "SQL error: " << sqlite3_errmsg(db) << endl;
        }
        else
        {
            cout << "Data added successfully" << endl;
        }

        sqlite3_finalize(insertStmt);
    }
    else
    {
        cerr << "Product not found" << endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void prompt()
{
    int user_input;
    cout << "Welcome " << userEmail << endl;
    cout << "--------------------------------------" << endl;
    cout << "         E-COMMERCE PROJECT           " << endl;
    cout << "--------------------------------------" << endl;
    cout << "1: Add A Product." << endl;
    cout << "2: Order A Product." << endl;
    cout << "3: Show Order Queue." << endl;
    cout << "4: Show Products Stack." << endl;
    cout << "5: Register." << endl;
    cout << "6: Login." << endl;
    cout << "7: Logout." << endl;
    cout << "8: About."<<endl;
    cout << "9: Quit." << endl;
    cout << "--------------------------------------" << endl;
    cout << "Enter A Number Respectively To Operate : ";
    cin >> user_input;

    switch (user_input)
    {
    case 1:
        add_product();
        prompt();
        break;
    case 2:
        order_product();
        prompt();
    case 3:
        printOrders();
        prompt();
    case 4:
        printProducts();
        prompt();
    case 5:
        userRegister();
        prompt();
    case 6:
        userLogin();
        prompt();
    case 7:
        if (userEmail.empty())
        {
            cout << "#########################\n Login/Register First To logout \n#########################" << endl;
            prompt();
        }
        else
        {
            userEmail.clear();
            prompt();
        }
    case 8:
        aboutUs();
        prompt();
    case 9:
        exit(0);
    default:
        cout << endl
             << "Invalid Input Try Again And Choose The Numbers Respectively." << endl;
    }
}

int main()
{
    prompt();
    return 0;
}
