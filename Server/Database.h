#include <sqlite3.h>
#include <string>
#include <iostream>
#include "bits:stdc++.h"
using Record = std::vector<std::string>; 
using Records = std::vector<Record>;
using namespace std;
class Database
{
public:
    const char *databaseName; //name of database
    sqlite3 *DB;

    void printError(string);
    void openDatabase();
    void createTable(string);
    void displayContents(string);
    void insertIntoTable(string);
    void deleteFromTable(string, string);
    Records getContents(string);

    void addPublisher(string, string );
    void addFile(int, string, string );
    void addSubscriber(string, string );

    string getCategoryList();
    string getFilenames(string );
    string getIP(string, string);
    string getKey(string, string );
};
static string returningString;

static int callback(void *data, int argc, char **argv, char **azColName) //what does this function do?
{
    int i;
    fprintf(stderr, "%s: ", (const char *)data);
    returningString = "";

    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        returningString = returningString + argv[i] + "\n";
    }
    printf("\n");
    return 0;
}

int select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names)
{
    Records *records = static_cast<Records *>(p_data);
    try
    {
        records->emplace_back(p_fields, p_fields + num_fields);
    }
    catch (...)
    {
        return 1;
    }
    return 0;
}
void Database::openDatabase()
{
    int exit = sqlite3_open(databaseName, &DB);
    if (exit)
    {
        printError("Opening database");
        cout << "Retrying";
        openDatabase();
    }
    /*else
    {
        //cout << "Opened database successfully:\n"
        //     << exit;
        //sqlite3_close(DB);
    }*/
}
void Database::createTable(string sqlCommand)
{
    char *messaggeError;
    int exit = sqlite3_exec(DB, sqlCommand.c_str(), NULL, 0, &messaggeError);

    if (exit != SQLITE_OK)
    {
        printError("Creating table\n");
        cout << messaggeError;
        sqlite3_free(messaggeError);
    }
    else
        std::cout << "Table created Successfully" << std::endl;
}
void Database::displayContents(string Database)
{
    sqlite3_exec(DB, Database.c_str(), callback, NULL, NULL);
    //cout << "Displayed";
}
Records Database::getContents(string Database)
{
    Records records;
    char *errmsg;
    int ret = sqlite3_exec(DB, Database.c_str(), select_callback, &records, &errmsg);
    if (ret != SQLITE_OK)
    {
        std::cerr << "Error in select statement " << Database << "[" << errmsg << "]\n";
    }
    else
    {
        //std::cerr << records.size() << " records returned.\n";
    }

    return records;
}

void Database::deleteFromTable(string tableName, string condition)
{
    char *messageError;
    string sqlCommand("DELETE FROM " + tableName + " WHERE " + condition + ";");
    int exit = sqlite3_exec(DB, sqlCommand.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK)
    {
        printError("Deleting from table\n");
        cout << messageError;
        sqlite3_free(messageError);
    }
    else
        std::cout << "Record deleted Successfully!";
}

void Database::insertIntoTable(string sqlCommand)
{
    char *messageError;
    int exit = sqlite3_exec(DB, sqlCommand.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK)
    {
        printError("Inserting into table");
        cout << messageError;
        sqlite3_free(messageError);
    }
    else
        cout << "Records created Successfully!";
}
void Database::printError(string errorMessage)
{
    cerr << "Error while: " << errorMessage;
}

void Database::addPublisher(string IP, string key)
{
    string sql("insert into publisher_details(ip_address,auth_key) values('" + IP + "' ,'" + key + "');");
    insertIntoTable(sql);
}
void Database::addFile(int pubNo, string category, string file)
{
    string sql("insert into files(publisher_id,category,file_name) values(" + to_string(pubNo) + ",'" + category + "','" + file + "');");
    insertIntoTable(sql);
}
void Database::addSubscriber(string IP, string userName)
{
    string sql("insert into subscriber_details(ip_address,user_name) values('" + IP + "' ,'" + userName + "');");
    insertIntoTable(sql);
}
string Database::getCategoryList()
{
    string sql("select distinct category from files;");
    Records records = getContents(sql);
    string data = "";
    for (auto &record : records)
    {
        // do something with your records
        data = data + record[0] + "\n";
    }
    return data;
}
string Database::getFilenames(string category)
{
    string sql("select distinct file_name from files where category='" + category + "';");
    Records records = getContents(sql);
    string data = "";
    for (auto &record : records)
    {
        // do something with your records
        data = data + record[0] + "\n";
    }
    return data;
}
string Database::getIP(string category, string fileName)
{
    string getid("select publisher_id from files where file_name='" + fileName + "' and category='" + category + "';");
    Records records = getContents(getid);
    string pubid = "";
    for (auto &record : records)
    {
        pubid = record[0];
    }
    string getip("select ip_address from publisher_details where publisher_id='" + pubid + "';");
    Records records1 = getContents(getip);
    string ip;
    for (auto &record : records1)
    {
        ip = record[0];
    }

    return ip;
}
string Database::getKey(string category, string fileName)
{
    string getid("select publisher_id from files where file_name='" + fileName + "' and category='" + category + "';");
    Records records = getContents(getid);
    string pubid = "";
    for (auto &record : records)
    {
        pubid = record[0];
    }
    string getkey("select auth_key from publisher_details where publisher_id='" + pubid + "';");
    Records records1 = getContents(getkey);
    string key;
    for (auto &record : records1)
    {
        key = record[0];
    }
    return key;
}

/*int main(){
    Database *obj=new Database();
    Database *db=new Database();
    obj->openDatabase();
    /*string sql("CREATE TABLE publisher_details (publisher_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,ip_address TEXT NOT NULL,auth_key TEXT NOT NULL );");
    obj->createTable(sql);
    db->addPublisher("192.168.43.40","123",obj);
        db->addPublisher("192.168.43.41","456",obj);
            db->addPublisher("192.168.43.42","789",obj);
    string sql1("CREATE TABLE files (file_no INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,publisher_id INTEGER NOT NULL ,category TEXT NOT NULL,file_name TEXT NOT NULL , FOREIGN KEY(publisher_id) REFERENCES publisher_details(publisher_id));");
    obj->createTable(sql1);
    db->addFile(1,"Music","TS",obj);
     db->addFile(2,"Music","SD",obj);
    db->addFile(1,"Books","HP",obj);
        db->addFile(3,"Books","abc",obj);
   
    string sql2("CREATE TABLE subscriber_details (subscriber_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,ip_address TEXT NOT NULL,user_name TEXT NOT NULL );");
    obj->createTable(sql2);
    db->addSubscriber("192.168.43.41","harsh",obj);
    db->addSubscriber("192.168.43.45","laharsh",obj);
    db->addSubscriber("192.168.43.46","akharsh",obj);
    obj->displayContents("subscriber_details");
    obj->getKey
*/

//cout<<db->getCategoryList(obj);
//string cat="Music",file="SD";
//cout<<db->getFilenames(cat,obj);
//cout<<db->getIP(cat,file,obj);




