#include <iostream>
#include <string.h>   //strlen  
#include <cstdlib>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include "Database.h"

using namespace std;

#define TRUE   1 
#define FALSE  0 
#define PUB_PORT 5000
#define SUB_PORT 6001

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
    int opt = TRUE;
    struct sockaddr_in address;
    int listening;
    int addrlen, max_sd, sd, new_sock, activity, rcvd_bytes; 
    int client_socket[10];  //list of sockets  
    fd_set readfds;
    char hello[] = "Welcome to the server publisher!";
    char buffer[1024];

    //initializing all sockets
    for( int i=0 ;i < 10; i++){
        client_socket[i] = 0;
    }

    //initialzing listening socket for publishers
    if( (listening = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //enabling reusing address
    if( setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //setting up the socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PUB_PORT);

    //binding socket
    if( bind(listening, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout<<"Listener on port "<<PUB_PORT<<"\n";

    //listening
    if (listen(listening, 3) < 0) { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }

    //accepting new connection
    addrlen = sizeof(address);   
    puts("Waiting for connection...");
    
    while (TRUE){
         
        FD_ZERO(&readfds);  //clear socket set

        FD_SET(listening, &readfds);    //setting the master socket to readfds
        max_sd = listening;

        //add child sockets to the fd_set
        for( int i = 0; i < 10 ; i++ ){
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor, add to the fd_set
            if(sd > 0)
                FD_SET( sd, &readfds );

            //highest file desc number, need it for select()
            if( sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets indefinitely
        activity = select( max_sd+1, &readfds, NULL, NULL, NULL);

        if( (activity < 0) && (errno!=EINTR)){
            cout<<"select error\n";
        }

        //check for new connections
        if( FD_ISSET(listening, &readfds) ){
            if ( (new_sock = accept(listening, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number
            cout<<"New connection, socket fd is "<< new_sock <<", IP address : "<< inet_ntoa(address.sin_addr) <<", Port : "<< ntohs(address.sin_port)<<endl;
            
            //send welcome message
            if( send(new_sock, hello, strlen(hello), 0) != strlen(hello) ){
                perror("send");
            }

            puts("Welcome message sent successfully");

            //adding only for initial connection
            // char* buffer = "Enter your unique key\n";
            // char* key = " ";
            // send(new_sock,buffer,sizeof(buffer),0);
            char key[8];
            int key_size = recv(new_sock,key,sizeof(key),0);

            if(key_size > 0){
                cout<<"Key received : "<<key<<endl;
                database->addPublisher(inet_ntoa(address.sin_addr),key);
            }

            //add new socket to array of sockets
            for( int i=0; i < 10; i++ ){
                //if position is free
                if(client_socket[i] == 0){
                    client_socket[i] = new_sock;
                    cout<<"Adding to list of sockets, socket : "<< i <<endl;
                    break;
                }
            }
        }

        //or an I/O operation
        for( int i=0; i < 10; i++ ){
            sd = client_socket[i];
            memset(buffer, 0, 1024);

            //check if it is in the list
            if( FD_ISSET( sd, &readfds)){
                //check and read the message, if zero, close connection
                if( (rcvd_bytes = read(sd, buffer, 1024)) == 0){
                    //get details of disconnected user
                    getpeername( sd, (struct sockaddr*)&address, (socklen_t *)&addrlen);
                    cout<<"Host disconnected from IP : "<< inet_ntoa(address.sin_addr)<<" at port : "<<ntohs(address.sin_port);

                    //close the socket and set it to zero
                    close(sd);
                    client_socket[i] = 0;
                }  
                else{
                    //Publisher operations begin



                }
            }
        }
    }
    return;
}

void ServerConnection::openSubConnection(){
    int opt = TRUE;
    struct sockaddr_in address;
    int listening;
    int addrlen, max_sd, sd, new_sock, activity, rcvd_bytes; 
    int client_socket[10];  //list of sockets  
    fd_set readfds;
    char hello[] = "Welcome to the server! subscriber";
    char buffer[1024];

    //initialise all client_socket[] to 0 so not checked  
    for ( int i = 0; i < 10; i++)   
    {   
        client_socket[i] = 0;   
    } 

    //initialzing listening socket for subscribers
    if( (listening = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //enabling reusing sockets
    if( setsockopt(listening, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //setting up the socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SUB_PORT);

    //binding socket
    if( bind(listening, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    cout<<"Listener on port "<<SUB_PORT<<"\n";

    //listen to conns
    if (listen(listening, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }

    //accepting new connection
    addrlen = sizeof(address);

    puts("Waiting for connection...");

    while (TRUE){
        if (listen(listening, 3) < 0) { 
            perror("listen"); 
            exit(EXIT_FAILURE); 
        }
        FD_ZERO(&readfds);  //clear socket set

        FD_SET(listening, &readfds);    //setting the master socket to readfds
        max_sd = listening;

        //add child sockets to the fd_set
        for( int i = 0; i < 10 ; i++ ){
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor, add to the fd_set
            if(sd > 0)
                FD_SET( sd, &readfds );

            //highest file desc number, need it for select()
            if( sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets indefinitely
        activity = select( max_sd+1, &readfds, NULL, NULL, NULL );

        if( (activity < 0) && (errno!=EINTR) ){
            cout<<"select error\n";
        }

        //check for new connections
        if( FD_ISSET(listening, &readfds) ){
            if ( (new_sock = accept(listening, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number
            cout<<"New connection, socket fd is "<< new_sock <<", IP address : "<< inet_ntoa(address.sin_addr) <<", Port : "<< ntohs(address.sin_port)<<endl;
            
            //send welcome message
            if( send(new_sock, hello, sizeof(hello), 0) != strlen(hello) ){
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for( int i=0; i < 10; i++ ){
                //if position is free
                if(client_socket[i] == 0){
                    client_socket[i] = new_sock;
                    cout<<"Adding to list of sockets, socket : "<< i <<endl;
                    break;
                }
            }
        }

        //or an I/O operation
        for( int i=0; i < 10; i++ ){
            sd = client_socket[i];
            memset(buffer, 0, 1024);

            //check if it is in the list
            if( FD_ISSET( sd, &readfds) ){
                //check and read the message, if zero, close connection
                if( rcvd_bytes = read(sd, buffer, 1024) == 0){
                    //get details of disconnected user
                    getpeername( sd, (struct sockaddr*)&address, (socklen_t *)&addrlen);
                    cout<<"Host disconnected from IP : "<< inet_ntoa(address.sin_addr)<<" at port : "<<ntohs(address.sin_port);

                    //close the socket and set it to zero
                    close(sd);
                    client_socket[i] = 0;
                }  
                else{
                    //Subscriber operations begin



                }
            }
        }
    }
    return;
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
        cout<<"running from child, subscriber connections\n";
        server->openSubConnection();
    }else if( pid > 0 ){
        cout<<"running from parent, publisher connection\n";
        server->openPubConnection();
    }else{
        cout<<"fork failed\n";
    }
}