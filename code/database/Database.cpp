#include<iostream>
#include<string.h>
#include <sqlite3.h> 
#include "SqlQuery.h"
using namespace std;

class Database {
    public:
        void addPublisher(string ,string,SqlQuery*);
        void addFile(int,string,string,SqlQuery*);
        void addSubscriber(string,string,SqlQuery*);

        string getCategoryList();
        string getFilename();
        string getIP();
        string getKey();
};
void Database::addPublisher(string IP,string key,SqlQuery* object){
    string sql("insert into publisher_details(ip_address,auth_key) values('"+IP+"' ,'"+key+"');");
    object->insertIntoTable(sql);
}
void Database::addFile(int pubNo,string category,string file,SqlQuery* object){
    string sql("insert into files(publisher_id,category,file_name) values("+to_string(pubNo)+",'"+category+"','"+file+"');");
    object->insertIntoTable(sql);
}
void Database::addSubscriber(string IP,string userName,SqlQuery* object){
    string sql("insert into subscriber_details(ip_address,user_name) values('"+IP+"' ,'"+userName+"');");
    object->insertIntoTable(sql);
}
int main(){
    SqlQuery *obj=new SqlQuery();
    Database *db=new Database();
    obj->databaseName="PUBLISHER.db";
    obj->openDatabase();
    string sql("CREATE TABLE publisher_details (publisher_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,ip_address TEXT NOT NULL,auth_key TEXT NOT NULL );");
    //obj->createTable(sql);
    //db->addPublisher("192.168.43.40","123",obj);
    string sql1("CREATE TABLE files (file_no INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,publisher_id INTEGER NOT NULL ,category TEXT NOT NULL,file_name TEXT NOT NULL , FOREIGN KEY(publisher_id) REFERENCES publisher_details(publisher_id));");
    //obj->createTable(sql1);
    //db->addFile(1,"Music","TS",obj);
    string sql2("CREATE TABLE subscriber_details (subscriber_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,ip_address TEXT NOT NULL,user_name TEXT NOT NULL );");
    //obj->createTable(sql2);
    //db->addSubscriber("192.168.43.41","harsh",obj);
    obj->displayContents("subscriber_details");

}
/*
ask if new publisher
store ip and key
ask for category and file name to be added
store cat,fname,pubid;
notify subscribers



*/
//string sql("CREATE TABLE publisher_details (publisher_id INT NOT NULL AUTO_INCREMENT,ip_address VARCHAR(100) NOT NULL,auth_key VARCHAR(100) NOT NULL,PRIMARY KEY (publisher_id) );");
//insert into publisher_details(ip_address,auth_key) values('192.168.43.41','qwert');
