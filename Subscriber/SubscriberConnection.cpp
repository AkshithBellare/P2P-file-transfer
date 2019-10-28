#include <stdio.h>  
#include <string.h>
#include <iostream>   
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>     
#include <arpa/inet.h>      
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <sys/time.h> 
using namespace std;

#define SERV_PORT 9999
#define SUB_PUB_PORT 4000
#define SERV_IP "10.50.19.120"
#define PUBLISHER_IP "10.50.19.120"

class SubscriberConnection{
    public:
        char buffer[4098];
        struct sockaddr_in server_address;
        int SubscriberSockfd;
        int SubToPubSockfd;
        struct sockaddr_in publisher_as_server;
        void connectToServer();
        void connectToPublisher();
        void serverShowsListOfTopics();
        void subscribe();
        void ReceiveMessageFromServer();
        void ReceiveFileFromPublisher();
        void error(const char *msg){
    		perror(msg);
    		exit(0);
		}
};

void SubscriberConnection::ReceiveMessageFromServer(){

}

void SubscriberConnection::ReceiveFileFromPublisher(){
    
}
void SubscriberConnection::connectToServer(){
    SubscriberSockfd = socket(AF_INET,SOCK_STREAM,0);
	if (SubscriberSockfd < 0) {
        error("ERROR opening socket");
	}
	bzero(&server_address,sizeof(server_address));
	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(SERV_PORT);

	inet_pton(AF_INET,SERV_IP,&server_address.sin_addr);
    if (connect(SubscriberSockfd,(struct sockaddr *) &server_address,sizeof(SubscriberSockfd)) < 0) {
        error("ERROR connecting");
    }
    cout<<"Connected to server";
}

void SubscriberConnection::serverShowsListOfTopics(){
    bzero(buffer,256);
     int n = read(SubscriberSockfd,buffer,255);
    if (n < 0){
        error("ERROR reading from socket");
    }
    cout << buffer;
}

void SubscriberConnection::subscribe(){
    cout << "Enter the topics you want to subscribe to:";
    cin >> buffer;
    send(SubscriberSockfd, buffer, strlen(buffer),0);
}

void SubscriberConnection::connectToPublisher(){
    SubToPubSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(SubToPubSockfd < 0){
        error("ERROR opening socket");
    }
    bzero(&publisher_as_server,sizeof(publisher_as_server));
    publisher_as_server.sin_family = AF_INET;
    publisher_as_server.sin_port = htons(SUB_PUB_PORT);

    inet_pton(AF_INET,PUBLISHER_IP,&publisher_as_server.sin_addr);
    if (connect(SubToPubSockfd,(struct sockaddr *) &publisher_as_server,sizeof(SubToPubSockfd)) < 0) {
        error("ERROR connecting");
    }
    cout<<"Connected to server";


}






