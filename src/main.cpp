#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <queue>
#include <stack>
#include <string>
#include <sqlite3.h>

using namespace std;

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

    cout << "|-----------------------------------------|" << endl;
    cout << "| "
         << "Name"
         << "                   |      "
         << "Id No"
         << "     |" << endl;
    cout << "|-----------------------------------------|" << endl;
    cout << "| "
         << "Natnael Tesfaye"
         << "        |      "
         << "URC112/14"
         << " |" << endl;
    cout << "| "
         << "Yared Kiros"
         << "            |      "
         << "URC119/14"
         << " |" << endl;
    cout << "| "
         << "Mickyas Tesfaye"
         << "        |      "
         << "URC109/14"
         << " |" << endl;
    cout << "|-----------------------------------------|" << endl;
}

void dequeueOrder(){
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    if (rc)
    {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
    }

    // Prepare SQL statement to check user credentials
    std::string sql = "DELETE FROM order_details WHERE ROWID IN (SELECT ROWID FROM order_details ORDER BY ROWID ASC LIMIT 1);";
    char *errMsg = nullptr;

    // Execute the query
    rc = sqlite3_exec(db, sql.c_str(), callback, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error executing SQLite query: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
    }

    // Close the database
    cout<<"The First Order has been Finished and Dequeued you can check it!!!"<<endl;
    sqlite3_close(db);
}

void popProduct(){
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    if (rc)
    {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
    }

    // Prepare SQL statement to check user credentials
    std::string sql = "DELETE FROM product_info WHERE ROWID IN (SELECT ROWID FROM product_info ORDER BY ROWID DESC LIMIT 1);";
    char *errMsg = nullptr;

    // Execute the query
    rc = sqlite3_exec(db, sql.c_str(), callback, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error executing SQLite query: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
    }

    // Close the database
    cout<<"The last Product has been Poped out you can check it!!!"<<endl;
    sqlite3_close(db);
}

void userLogin()
{
    sqlite3 *db;
    int rc = sqlite3_open("main.db", &db);
    if (rc)
    {
        std::cerr << "Error opening SQLite database: " << sqlite3_errmsg(db) << std::endl;
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
    std::string sql = "SELECT * FROM Users WHERE email='" + email + "' AND password='" + password + "'";
    char *errMsg = nullptr;

    // Execute the query
    rc = sqlite3_exec(db, sql.c_str(), callback, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error executing SQLite query: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
    }
    // Check if login was successful
    if (userEmail.empty())
    {
        std::cout << "Invalid email or password." << std::endl;
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
         << "Description"
         << " | "
         << "User "
         << endl;
    cout << "--------------------------------------" << endl;
    sql = "SELECT * FROM product_info ORDER BY id DESC;";
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
         << "Name"
         << " | "
         << "Date"
         << " | "
         << "User"
         << endl;
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
        std::cout << "Register / Login First To Order A Product" << std::endl;
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
    sql = "INSERT INTO Product_Info (product_name, product_price, product_description, username) VALUES (?, ?, ?, ?)";
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
    sqlite3_bind_text(stmt, 4, userEmail.c_str(), -1, SQLITE_STATIC);
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
    else
    {
        cout << "Product Added Successfully" << endl;
    }
    sqlite3_finalize(stmt);
}

void order_product()
{

    if (userEmail.empty())
    {
        std::cout << "Register / Login First To Order A Product" << std::endl;
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
    string product_id; // Declare as string
    cout << "Enter product ID The Same As Shown Above: ";
    cin >> product_id;
    cout << "--------------------------------------" << endl;
    sqlite3 *db;
    char *errMsg = nullptr;
    int rc = sqlite3_open("main.db", &db);
    if (rc != SQLITE_OK)
    {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
    }

    const char *sql = "INSERT INTO order_details (product_name, order_date, username) VALUES ((SELECT product_name FROM Product_Info WHERE id = ?), datetime('now'), ?);";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        cout << "Error preparing statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        exit(0);
    }
    sqlite3_bind_text(stmt, 1, product_id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, userEmail.c_str(), -1, SQLITE_STATIC);
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
    else
    {
        cout << "Ordered Product Successfully" << endl;
    }

    sqlite3_finalize(stmt);
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
    cout << "5: Pop Product." << endl;
    cout << "6: Dequeue Order." << endl;
    cout << "7: Register." << endl;
    cout << "8: Login." << endl;
    cout << "9: Logout." << endl;
    cout << "10: About." << endl;
    cout << "11: Quit." << endl;
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
        popProduct();
        prompt();
    case 6:
        dequeueOrder();
        prompt();
    case 7:
        userRegister();
        prompt();
    case 8:
        userLogin();
        prompt();
    case 9:
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
    case 10:
        aboutUs();
        prompt();
    case 11:
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
