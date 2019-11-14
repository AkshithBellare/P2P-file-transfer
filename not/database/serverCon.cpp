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
    obj->get;
}