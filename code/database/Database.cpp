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

        string getCategoryList(SqlQuery*);
        string getFilenames(string,SqlQuery*);
        string getIP(string,string,SqlQuery*);
        string getKey(string,string,SqlQuery*);
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
string Database::getCategoryList(SqlQuery* object){
    string sql("select distinct category from files;");
    Records records = object->getContents(sql);
    string data="";
    for (auto& record : records) {
    // do something with your records
        data=data+record[0]+"\n";
    }
    return data;
}
string Database::getFilenames(string category,SqlQuery* object){
    string sql("select distinct file_name from files where category='"+category+"';");
    Records records = object->getContents(sql);
    string data="";
    for (auto& record : records) {
    // do something with your records
        data=data+record[0]+"\n";
    }
    return data;
}
string Database::getIP(string category,string fileName,SqlQuery* object){
    string getid("select publisher_id from files where file_name='"+fileName+"' and category='"+category+"';");
    Records records = object->getContents(getid);
    string pubid="";
    for (auto& record : records) {
        pubid=record[0];
    }
    string getip("select ip_address from publisher_details where publisher_id='"+pubid+"';");
    Records records1 = object->getContents(getip);
    string ip;
    for (auto& record : records1) {
        ip=record[0];
    }

    return ip;
}
string Database::getKey(string category,string fileName,SqlQuery* object){
    string getid("select publisher_id from files where file_name='"+fileName+"' and category='"+category+"';");
    Records records = object->getContents(getid);
    string pubid="";
    for (auto& record : records) {
        pubid=record[0];
    }
    string getkey("select auth_key from publisher_details where publisher_id='"+pubid+"';");
    Records records1 = object->getContents(getkey);
    string key;
    for (auto& record : records1) {
        key=record[0];
    }
    return key;
}

int main(){
    SqlQuery *obj=new SqlQuery();
    Database *db=new Database();
    obj->databaseName="PUBLISHER.db";
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
}
