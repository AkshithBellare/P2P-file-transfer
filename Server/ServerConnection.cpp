#include <iostream>
#include <string.h>   //strlen  
#include <cstdlib>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <sys/ioctl.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include "Database.h"

using namespace std;

#define TRUE   1 
#define FALSE  0 
#define PUB_PORT 5000
#define SUB_PORT 8000

Database *database = new Database();

class ServerConnection{

    //int pub_master_sock, sub_master_sock;

	public:
		void openPubConnection();
        void openSubConnection();
		void sendCategory();
		void sendKey();
};

void ServerConnection::openPubConnection(){
    int    i, len, rc, on = 1;
    int client_socket[10];
    int    listen_sd, max_sd, new_sd;
    int    desc_ready, end_server = FALSE;
    int    close_conn;
    char   buffer[80];
    struct sockaddr_in addr, cliaddr;
    fd_set              master_set, working_set;


    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0){
        perror("socket\n");
        exit(0);
    }

    rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    if (rc<0){
        perror("setsockopt fail\n");
        exit(0);
    }

    rc = ioctl(listen_sd, FIONBIO, (char*)&on);
    if (rc<0){
        perror("ioctl");
        exit(0);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PUB_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    rc = bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr));
    if(rc<0){
        perror("bind publisher");
        exit(0);
    }

    rc= listen(listen_sd, 10);
    if( rc < 0 ){
        perror("listen");
        exit(0);
    }

    FD_ZERO(&master_set);
    max_sd = listen_sd;
    FD_SET(listen_sd, &master_set);

    do{
        memcpy(&working_set, &master_set, sizeof(working_set));
        rc = select(max_sd+1, &working_set, NULL, NULL, 0);
        if( rc < 0 ){
            perror("select");
            exit(0);
        }

        if(rc==0){
            cout<<"timeout"<<endl;
            break;
        }

        desc_ready = rc;
        cout<<"sockets ready for i/o "<<desc_ready<<endl;
        for(i = 0; i<=max_sd && desc_ready>0;i++){
            if(FD_ISSET(i, &working_set)){
                desc_ready-=1;
                if(i == listen_sd){
                    do{
                        int clilen = sizeof(cliaddr);
                        new_sd = accept(i, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
                        if(new_sd<0){
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  accept() failed");
                                end_server = TRUE;
                            }
                            break;
                        }
                        cout<<"New incoming connection publisher"<<inet_ntoa(cliaddr.sin_addr)<<endl;
                        send(new_sd, "Hello", 6, 0);
                        char key[8];
                        read(new_sd, key, 7);
                        if(strlen(key)>0){
                            cout<<"received key:"<<key<<endl;
                            database->addPublisher(inet_ntoa(cliaddr.sin_addr), key);
                        }
                        FD_SET(new_sd, &master_set);
                        if(new_sd>max_sd)
                            max_sd = new_sd;
                    }while(new_sd!=-1);
                }
                else{
                    cout<<"Readable socket "<<endl;
                    close_conn = FALSE;
                        //recieving choice
                        bzero(buffer, sizeof(buffer));
                        rc = read(i, buffer, sizeof(buffer));
                        if( rc<0 ){
                            if (errno!=EWOULDBLOCK){
                                perror("recv");
                                close_conn = TRUE;
                                close(i);
                            FD_CLR(i, &master_set);
                            }
                            break;
                        }
                        if(rc==0){
                            close_conn=TRUE;
                            cout<<"conn closed"<<endl;
                            close(i);
                            FD_CLR(i, &master_set);
                            break;
                        }       

                        len = rc;
                        cout<<rc<<" bytes received"<<endl;
                        cout<<buffer<<endl;

                        if(strcmp(buffer,"1") == 0 ){
                            cout<<"Sending file dets"<<endl;
                            string CAT_LIST = database->getCategoryList(); 
                            cout<<CAT_LIST<<endl;
                            if(CAT_LIST.length() > 0)
                                send(i, CAT_LIST.c_str(), CAT_LIST.length(), 0);
                            else
                                send(i, "EMPTY", 6, 0); 
                        }
                        else if(strcmp(buffer, "2") == 0){
                            int recv_file_size;
                            if( (recv_file_size = read(i, buffer, sizeof(buffer))) > 0 ){
                                    cout<<"Received file deets"<<endl;
                                    cout<<buffer<<endl;
                                    string cat_file = buffer;
                                    string category, file;

                                    int pos = cat_file.find(":");

                                    category = cat_file.substr(0, pos);
                                    file = cat_file.substr(pos+1, recv_file_size);
                                    cout<<"category : "<<category<<endl;
                                    cout<<"File : "<<file<<endl;
                                    database->addFile(i, category, file);
                                    database->addToQueue(category);
                                    cout<<"added recieved fikes into database"<<endl;

                                    send(i, "Added file", 11, 0);
                            }
                        }
                        if (close_conn)
                        {
                            close(i);
                            FD_CLR(i, &master_set);
                            if (i == max_sd)
                            {
                                while (FD_ISSET(max_sd, &master_set) == FALSE)
                                    max_sd -= 1;
                            }
                        }
                }
            }
        }

    }while(end_server!=TRUE);
}

void ServerConnection::openSubConnection(){
    int    i, len, rc, on = 1;
    int client_sock[10];
    int    listen_sd, max_sd, new_sd;
    int    desc_ready, end_server = FALSE;
    int    close_conn;
    char   buffer[256];
    struct sockaddr_in addr, cliaddr;
    fd_set              master_set, working_set;
    map<int,string> mapSub;

    for(int i=0;i<10;i++){
        client_sock[i]=0;
    }

    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0){
        perror("socket");
        exit(0);
    }

    rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    if (rc<0){
        perror("setsockopt fail");
        exit(0);
    }

    rc = ioctl(listen_sd, FIONBIO, (char*)&on);
    if (rc<0){
        perror("ioctl");
        exit(0);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SUB_PORT);
    //addr.sin_addr.s_addr = INADDR_ANY;
    rc = bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr));
    if(rc<0){
        perror("bind sub failed");
        exit(0);
    }

    rc= listen(listen_sd, 10);
    if( rc < 0 ){
        perror("listen");
        exit(0);
    }

    FD_ZERO(&master_set);
    max_sd = listen_sd;
    FD_SET(listen_sd, &master_set);

    do{
        memcpy(&working_set, &master_set, sizeof(working_set));

        rc = select(max_sd+1, &working_set, NULL, NULL, NULL);
        if( rc < 0 ){
            perror("select");
            exit(0);
        }

        if(rc==0){
            cout<<"timeout"<<endl;
            break;
        }

        desc_ready = rc;
        cout<<"sockets ready for i/o "<<desc_ready<<endl;
        for(i = 0; i<=max_sd && desc_ready>0;i++){
            if(FD_ISSET(i, &working_set)){
                desc_ready-=1;
                 string notify;
                
                if(i == listen_sd){
                    do{
                        int clilen = sizeof(cliaddr);
                        new_sd = accept(i, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
                        if(new_sd<0){
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  accept() failed");
                                end_server = TRUE;
                            }
                            break;
                        }
                        cout<<"New incoming connection subscriber"<<inet_ntoa(cliaddr.sin_addr)<<endl;
                        string subName;
                        bzero(buffer, sizeof(buffer));
                        if( read(new_sd, buffer, 256) > 0 ){
                                cout<<"Received name"<<endl;
                                subName = buffer;
                                mapSub[new_sd]=subName;
                        }

                        FD_SET(new_sd, &master_set);
                        if(new_sd>max_sd)
                            max_sd = new_sd;
                    }while(new_sd!=-1);
                }
                else{
                    cout<<"Readable socket "<<endl;
                    close_conn = FALSE;
                    string notify;
                    if(database->checkIfPresent(mapSub[i])){
                        notify="Y";
                        string condition="user_name='"+mapSub[i]+"';";
                        database->deleteFromTable("queue",condition );
                    }
                    else 
                    {
                        notify="N";
                    }               
                    bzero(buffer, sizeof(buffer));
                    //recv choice
                    rc = read(i, buffer, sizeof(buffer));
                    if( rc<0 ){
                        if (errno!=EWOULDBLOCK){
                            perror("recv");
                            close_conn = TRUE;
                            close(i);
                            FD_CLR(i, &master_set);
                        }
                        break;
                    }
                    if(rc==0){
                        close_conn=TRUE;
                        cout<<"conn closed"<<endl;
                        close(i);
                        FD_CLR(i, &master_set);
                        break;
                    }       
                    cout<<rc<<" bytes received from "<<mapSub[i]<<endl;
                    if(strcmp(buffer,"1") == 0 ){
                        cout<<"Sending file deets"<<endl;
                        string CAT_LIST = database->getCategoryList(); 
                        if(CAT_LIST.length() > 0){
                            CAT_LIST=notify+CAT_LIST;
                            send(i, CAT_LIST.c_str(), CAT_LIST.length(), 0);
                        }
                        else
                            send(i, "EMPTY", 6, 0); 
                    }else if(strcmp(buffer, "2") == 0){
                        int recv_file_size;
                        //recv category
                        bzero(buffer, sizeof(buffer));
                        if( (recv_file_size = read(i, buffer, sizeof(buffer))) > 0 ){
                            cout<<"Received request for"<<endl;
                            cout<<buffer<<endl;
                            string category = buffer;
                            string send_file = database->getFilenames(category);
                            //send filename
                            send_file=notify+send_file;
                            send(i, send_file.c_str(), send_file.length(), 0);
                            cout<<"sent file names"<<endl;

                            bzero(buffer, sizeof(buffer));
                            //recv file name and send IP, key
                            if( read(i, buffer, sizeof(buffer)) > 0){
                                string IP_KEY = database->getIP(category, buffer);
                                cout<<IP_KEY<<endl;
                                IP_KEY = IP_KEY +":"+database->getKey(category, buffer);
                                cout<<IP_KEY<<endl;
                                if( send(i, IP_KEY.c_str(), IP_KEY.length(), 0) > 0){
                                    cout<<"Send success"<<endl;
                                }
                            }  
                        }
                    }
                    else if(strcmp(buffer, "3") == 0){
                        cout<<"Sending file deets"<<endl;
                        string CAT_LIST = database->getCategoryList(); 
                        if(CAT_LIST.length() > 0){
                            CAT_LIST=notify+CAT_LIST;
                            send(i, CAT_LIST.c_str(), CAT_LIST.length(), 0);

                        }
                        else
                            send(i, "EMPTY", 6, 0); 
                            
                        //recv category subscription request
                        bzero(buffer, sizeof(buffer));
                        read(i, buffer, sizeof(buffer));
                        cout<<"Received subscription request"<<endl;
                        cout<<buffer<<endl;
                        string category = buffer; 
                        database->addSubscriber(mapSub[i],category);
                    }
                    if (close_conn)
                    {
                        close(i);
                        FD_CLR(i, &master_set);
                        if (i == max_sd)
                        {
                            while (FD_ISSET(max_sd, &master_set) == FALSE)
                                max_sd -= 1;
                        }
                    }
                }
               
            }
        }
    }while(end_server!=TRUE);
}

int main(int argc, char **argv){

    ServerConnection *server = new ServerConnection();

    //forking
    pid_t pid = fork();

    if( pid == 0){
        cout<<"running from child, publisher connections\n";
        server->openPubConnection();
    }else if( pid > 0 ){
        cout<<"running from parent, subscriber connection\n";
        server->openSubConnection();
    }else{
        cout<<"fork failed\n";
    }
}