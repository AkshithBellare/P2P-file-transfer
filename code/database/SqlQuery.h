#include <sqlite3.h> 
#include <string>
#include <iostream>
using namespace std;
 class SqlQuery{
   public:
      const char* databaseName;
      sqlite3 *DB;
           
      void printError(string);
      void openDatabase();
      void createTable(string);
      void displayContents(string);
      void insertIntoTable(string);
      void deleteFromTable(string,string);
};

static int callback(void* data, int argc, char** argv, char** azColName) 
{ 
    int i; 
    fprintf(stderr, "%s: ", (const char*)data); 
  
    for (i = 0; i < argc; i++) { 
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL"); 
    } 
  
    printf("\n"); 
    return 0; 
}
void SqlQuery::openDatabase(){
   int exit=sqlite3_open(databaseName,&DB);
   if(exit){
      printError("Opening database");
      cout<<"Retrying";
      openDatabase();
   }
   else{
      cout<<"Opened database successfully:\n"<<exit;
      //sqlite3_close(DB); 
   }
}
void SqlQuery::createTable(string sqlCommand){
   char* messaggeError; 
    int exit = sqlite3_exec(DB, sqlCommand.c_str(), NULL, 0, &messaggeError); 
  
    if (exit != SQLITE_OK) { 
        printError("Creating table\n");
        cout<<messaggeError;
        sqlite3_free(messaggeError); 
    } 
    else
        std::cout << "Table created Successfully" << std::endl;
}
void SqlQuery::displayContents(string tableName){
   string sqlQuery="SELECT * FROM "+tableName;
   sqlite3_exec(DB, sqlQuery.c_str(), callback, NULL, NULL); 

} 
void SqlQuery::deleteFromTable(string tableName,string condition){
   char *messageError;
   string sqlCommand("DELETE FROM "+tableName+" WHERE "+condition+";");
   int exit = sqlite3_exec(DB, sqlCommand.c_str(), NULL, 0, &messageError); 
    if (exit != SQLITE_OK) { 
        printError("Deleting from table\n");
        cout<<messageError;
        sqlite3_free(messageError);  
    } 
    else
        std::cout << "Record deleted Successfully!"; 
}

void SqlQuery::insertIntoTable(string sqlCommand){
    char* messageError; 
    int exit = sqlite3_exec(DB, sqlCommand.c_str(), NULL, 0, &messageError); 
    if (exit != SQLITE_OK) { 
         printError("Inserting into table");
         cout<<messageError;
         sqlite3_free(messageError); 
    } 
    else
      cout << "Records created Successfully!"; 
}
void SqlQuery::printError(string errorMessage){
   cerr<<"Error while: "<<errorMessage;
}
/*
int main(int argc,char** argv){
   SqlQuery *databaseObject=new SqlQuery();
   databaseObject->databaseName="example.db";
   databaseObject->openDatabase();
   string sqlQuery = "CREATE TABLE PERSON("
                      "ID INT PRIMARY KEY     NOT NULL, "
                      "NAME           TEXT    NOT NULL, "
                      "SURNAME          TEXT     NOT NULL, "
                      "AGE            INT     NOT NULL, "
                      "ADDRESS        CHAR(50), "
                      "SALARY         REAL );"; 
  // databaseObject->createTable(sqlQuery);
    string sql("INSERT INTO PERSON VALUES(1, 'STEVE', 'GATES', 30, 'PALO ALTO', 1000.0);"
               "INSERT INTO PERSON VALUES(2, 'BILL', 'ALLEN', 20, 'SEATTLE', 300.22);"
               "INSERT INTO PERSON VALUES(3, 'PAUL', 'JOBS', 24, 'SEATTLE', 9900.0);"); 
   //databaseObject->insertIntoTable(sql);
   sql = "DELETE FROM PERSON WHERE ID = 2;"; 
   //databaseObject->deleteFromTable(sql);
   databaseObject->displayContents("PERSON");

   sqlite3_close(databaseObject->DB); 

}*/