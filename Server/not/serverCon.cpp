#include <sqlite3.h>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include "Database.h"
using namespace std;
int main(){
    Database *obj=new Database();

    obj->databaseName="PUBLISHER.db";

    obj->openDatabase();
    //cout<<obj->getCategoryList();

    //obj->addPublisher("abcs", "ipadd" );
    //obj->addFile(3,"Game", "2K" );
    //obj->addSubscriber("100.100.100.100", "Mr beast" );

    //cout<<obj->getFilenames("Game");
    //cout<<obj->getIP("Game", "2K");
    //obj->getSubscribersList("Music");
    //obj->addToQueue("Music");
    /*vector<string> v=obj->getUserNames();
    for(int i=0;i<v.size();i++)
        cout<<v[i]<<"\n";*/
    //cout<<obj->checkIfPresent("Akshith");
    /*string var="Akshith";
                            string condition="user_name='"+var+"';";
    obj->deleteFromTable("queue",condition );*/
    /*string var="Akshith";
    string category = "Music"; 
    obj->addSubscriber("Shash","Music");*/
    obj->addSubscriber("Shash","Books");
    vector<string> v=obj->getSubscribersList("Books");
    for (int i=0;i<v.size();i++)
        cout<<v[i]<<"\n";

}