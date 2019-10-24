#include <iostream> 
#include <sqlite3.h> 
  
using namespace std; 
  
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
  
int main(int argc, char** argv) 
{ 
    sqlite3* DB; 
    int exit = 0; 
    char *errMessage;
    exit = sqlite3_open("example.db", &DB); 
    string data("CALLBACK FUNCTION"); 
  
    string sql("SELECT * FROM PERSON;"); 
    //string query("INSERT INTO PERSON VALUES(4, 'Akshith', 'Bellare', 45, 'bangalore', 10000);");

    if (exit) { 
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl; 
        return (-1); 
    } 
    else
        std::cout << "Opened Database Successfully!" << std::endl; 
  
    int rc = sqlite3_exec(DB, sql.c_str(), callback, (void*)data.c_str(), NULL);

    // exit = sqlite3_exec(DB, query.c_str(), NULL, 0, &errMessage);	

    // if (exit!=SQLITE_OK){
    // 	std::cerr<<"error inserting " << sqlite3_errmsg(DB) << std::endl;
    // 	return -1;
    // }
    // else std::cout<<"Success inserting..."<<std::endl;
  
    if (rc != SQLITE_OK) 
        cerr << "Error SELECT" << endl; 
    else { 
        cout << "Operation OK!" << endl; 
    } 
  
    sqlite3_close(DB); 
    return (0); 
} 