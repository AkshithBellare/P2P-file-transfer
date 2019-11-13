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
    // int opt = TRUE;
    // struct sockaddr_in address;
    // int listening;
    // int addrlen, max_sd, sd, new_sock, activity, rcvd_bytes; 
    // int client_socket[10];  //list of sockets  
    // fd_set readfds;
    // char hello[] = "Welcome to the server publisher!";
    // char buffer[1024];

    // //initializing all sockets
    // for( int i=0 ;i < 10; i++){
    //     client_socket[i] = 0;
    // }

    // //initialzing listening socket for publishers
    // if( (listening = socket(AF_INET, SOCK_STREAM, 0)) == 0){
    //     perror("socket failed");
    //     exit(EXIT_FAILURE);
    // }

    // //enabling reusing address
    // if( setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0){
    //     perror("setsockopt");
    //     exit(EXIT_FAILURE);
    // }

    // //setting up the socket
    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;
    // address.sin_port = htons(PUB_PORT);

    // //binding socket
    // if( bind(listening, (struct sockaddr *)&address, sizeof(address)) < 0){
    //     perror("bind failed");
    //     exit(EXIT_FAILURE);
    // }
    // cout<<"Listener on port "<<PUB_PORT<<"\n";

    // //listening
    // if (listen(listening, 3) < 0) { 
    //     perror("listen"); 
    //     exit(EXIT_FAILURE); 
    // }

    // //accepting new connection
    // addrlen = sizeof(address);   
    // puts("Waiting for connection...");
    
    // while (TRUE){
         
    //     FD_ZERO(&readfds);  //clear socket set

    //     FD_SET(listening, &readfds);    //setting the master socket to readfds
    //     max_sd = listening;

    //     //add child sockets to the fd_set
    //     for( int i = 0; i < 10 ; i++ ){
    //         //socket descriptor
    //         sd = client_socket[i];

    //         //if valid socket descriptor, add to the fd_set
    //         if(sd > 0)
    //             FD_SET( sd, &readfds );

    //         //highest file desc number, need it for select()
    //         if( sd > max_sd)
    //             max_sd = sd;
    //     }

    //     //wait for an activity on one of the sockets indefinitely
    //     activity = select( max_sd+1, &readfds, NULL, NULL, NULL);

    //     if( (activity < 0) && (errno!=EINTR)){
    //         cout<<"select error\n";
    //     }

    //     //check for new connections
    //     if( FD_ISSET(listening, &readfds) ){
    //         if ( (new_sock = accept(listening, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
    //         {
    //             perror("accept");
    //             exit(EXIT_FAILURE);
    //         }

    //         //inform user of socket number
    //         cout<<"New connection, socket fd is "<< new_sock <<", IP address : "<< inet_ntoa(address.sin_addr) <<", Port : "<< ntohs(address.sin_port)<<endl;
            
    //         //send welcome message
    //         if( send(new_sock, hello, strlen(hello), 0) != strlen(hello) ){
    //             perror("send");
    //         }

    //         puts("Welcome message sent successfully");

    //         //adding only for initial connection
    //         char key[8];
    //         int key_size = recv(new_sock,key,sizeof(key),0);

    //         if(key_size > 0){
    //             cout<<"Key received : "<<key<<endl;
    //             database->addPublisher(inet_ntoa(address.sin_addr),key);
    //         }

    //         //add new socket to array of sockets
    //         for( int i=0; i < 10; i++ ){
    //             //if position is free
    //             if(client_socket[i] == 0){
    //                 client_socket[i] = new_sock;
    //                 cout<<"Adding to list of sockets, socket : "<< i <<endl;
    //                 break;
    //             }
    //         }
    //     }

    //     //or an I/O operation
    //     for( int i=0; i < 10; i++ ){
    //         sd = client_socket[i];
    //         cout<<"socket descriptor for "<<i<<" is "<<sd<<endl;
    //         //cout<<"looping through sockets at "<<i<<endl;
    //         if( sd != 0 ){
    //             cout<<"going into socket"<<endl;
    //             memset(buffer, 0, 1024);

    //             //check if it is in the list
    //             if( FD_ISSET( sd, &readfds)){
    //                 //check and read the message, if zero, close connection
    //                 if( (rcvd_bytes = read(sd, buffer, 1024)) == 0){
                        
    //                     //get details of disconnected user
    //                     cout<<"Checking if socket is up "<<endl;
    //                     cout<<buffer<<endl;
    //                     getpeername( sd, (struct sockaddr*)&address, (socklen_t *)&addrlen);
    //                     cout<<"Host disconnected from IP : "<< inet_ntoa(address.sin_addr)<<" at port : "<<ntohs(address.sin_port)<<endl;

    //                     //close the socket and set it to zero
    //                     close(sd);
    //                     client_socket[i] = 0;
    //                 }  
    //                 else{
    //                     //Publisher operations begin
    //                     cout<<"Publisher ops begin"<<endl; 
    //                     char ch[2];
    //                     cout<<"gonna start reading"<<endl;
    //                     int readb= read(sd, ch, 2);
    //                     cout<<ch<<endl;
                        
    //                     if( strcmp(ch, "1") == 0 ){
    //                             string CAT_LIST = database->getCategoryList();
    //                             send(sd, CAT_LIST.c_str(), CAT_LIST.length(), 0);
    //                             cout<<"Sent file deets"<<endl;
    //                     }
    //                     else if( strcmp(ch, "2") == 0 ){
    //                         int recv_file_size;
    //                             if( (recv_file_size = recv(sd, buffer, sizeof(buffer), 0)) > 0 ){
    //                                 cout<<"Received file deets"<<endl;
    //                                 cout<<buffer<<endl;
    //                                 string cat_file = buffer;
    //                                 string category, file;

    //                                 int pos = cat_file.find(":");

    //                                 category = cat_file.substr(0, pos-1);
    //                                 file = cat_file.substr(pos+1, recv_file_size-1);

    //                                 database->addFile(sd, category, file);
    //                             }

    //                         send(sd, "Add file success", 17, 0);
    //                     }
                        
    //                 }
    //             }
    //         }
    //     }
    // }
    // return;
    int    i, len, rc, on = 1;
    int client_socket[10];
    int    listen_sd, max_sd, new_sd;
    int    desc_ready, end_server = FALSE;
    int    close_conn;
    char   buffer[80];
    struct sockaddr_in addr, cliaddr;
    struct timeval      timeout;
    fd_set              master_set, working_set;


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
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        //cout<<"Select started"<<endl;
        rc = select(max_sd+1, &working_set, NULL, NULL, &timeout);
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
                        cout<<"New incoming connection publisher-san"<<inet_ntoa(cliaddr.sin_addr)<<endl;
                        send(new_sd, "Hello", 6, 0);
                        char key[8];
                        recv(new_sd, key, 7, 0);
                        if(strlen(key)>0){
                            cout<<"received key "<<key<<endl;
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

                  
                        bzero(buffer, sizeof(buffer));
                        rc = recv(i, buffer, sizeof(buffer), 0);
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
                            cout<<"Sending file deets"<<endl;
                            string CAT_LIST = database->getCategoryList(); 
                            cout<<CAT_LIST<<endl;
                            if(CAT_LIST.length() > 0)
                                send(i, CAT_LIST.c_str(), CAT_LIST.length(), 0);
                            else
                                send(i, "EMPTY", 6, 0); 
                        }else if(strcmp(buffer, "2") == 0){
                            int recv_file_size;
                            if( (recv_file_size = recv(i, buffer, sizeof(buffer), 0)) > 0 ){
                                    cout<<"Received file deets"<<endl;
                                    cout<<buffer<<endl;
                                    string cat_file = buffer;
                                    string category, file;

                                    int pos = cat_file.find(":");

                                    category = cat_file.substr(0, pos-1);
                                    file = cat_file.substr(pos+1, recv_file_size-1);

                                    database->addFile(i, category, file);
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
    char   buffer[80];
    struct sockaddr_in addr, cliaddr;
    struct timeval      timeout;
    fd_set              master_set, working_set;

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
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        //cout<<"Select started"<<endl;
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
                        cout<<"New incoming connection subscriber-san "<<inet_ntoa(cliaddr.sin_addr)<<endl;
                        send(new_sd, "Hello", 6, 0);
                        
                        //add new socket to array of sockets
                        for( int j=0; j < 10; j++ ){
                            //if position is free
                            if(client_sock[j] == 0){
                                client_sock[j] = i;
                                cout<<"Adding to list of sockets, socket : "<< i <<endl;
                                break;
                            }
                        }
                        FD_SET(new_sd, &master_set);
                        if(new_sd>max_sd)
                            max_sd = new_sd;
                    }while(new_sd!=-1);
                }

                else{
                    cout<<"Readable socket "<<endl;
                    close_conn = FALSE;

                  
                        bzero(buffer, sizeof(buffer));
                        rc = recv(i, buffer, sizeof(buffer), 0);
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
                            cout<<"Sending file deets"<<endl;
                            string CAT_LIST = database->getCategoryList(); 
                            //CAT_LIST = CAT_LIST+database->getFilenames(CAT_LIST);
                            cout<<CAT_LIST<<endl;
                            if(CAT_LIST.length() > 0)
                                send(i, CAT_LIST.c_str(), CAT_LIST.length(), 0);
                            else
                                send(i, "EMPTY", 6, 0); 
                        }else if(strcmp(buffer, "2") == 0){
                            int recv_file_size;
                            //recv category
                            if( (recv_file_size = recv(i, buffer, sizeof(buffer), 0)) > 0 ){
                                cout<<"Received request"<<endl;
                                cout<<buffer<<endl;
                                string category = buffer;
                                string send_file = database->getFilenames(category);
                                //send filename
                                send(i, send_file.c_str(), send_file.length(), 0);
                                cout<<"sent file names"<<endl;

                                bzero(buffer, sizeof(buffer));
                                //recv file name and send IP, key
                                if( recv(i, buffer, sizeof(buffer), 0) > 0){
                                    string IP_KEY = database->getIP(category, buffer);
                                    IP_KEY = IP_KEY +":"+database->getKey(category, buffer);
                                    if( send(i, IP_KEY.c_str(), IP_KEY.length(), 0) > 0){
                                        cout<<"Send success"<<endl;
                                    }
                                }  
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

void ServerConnection::sendCategory(){

}

void ServerConnection::sendKey(){

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