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
#include <string.h>

using namespace std;

#define SERV_PORT 6000
#define SUB_PUB_PORT 4000
#define SERV_IP "10.50.19.120"
#define PUBLISHER_IP "10.53.92.211"

class SubscriberConnection{
public:
    char buffer[4098];
    struct sockaddr_in server_address;
    int SubscriberSockfd;
    int SubToPubSockfd;
    struct sockaddr_in publisher_as_server;
    void connectToServer(char* argv);
    void connectToPublisher();
    void serverShowsListOfTopics();
    void subscribe();
    void ReceiveMessageFromServer();
    void ReceiveFileFromPublisher();
    void sendFileNameToServer(char* filename);
    void error(const char *msg){
        perror(msg);
        exit(0);
    }
};

void SubscriberConnection::ReceiveMessageFromServer(){
    
}

/***void SubscriberConnection::ReceiveFileFromPublisher(){
 int ch = 0;
 //char fileName[100] = {0};
 char buffer[256]= {0};
 FILE* fp;
 recv(SubToPubSockfd, fileName, strlen(fileName),0);
 fp = fopen("", "a");
 int words;
 
 read(SubToPubSockfd, &words, sizeof(int));
 
 while(ch!=words){
 read(SubToPubSockfd,buffer, 255);
 fprintf(fp,"%s",buffer);
 ch++;
 }
 
 printf("The file was received successfully\n\n");
 }
 **/

void SubscriberConnection::sendFileNameToServer(char* filename){
    send(SubToPubSockfd, filename, sizeof(filename),0);
}

void SubscriberConnection::connectToServer(char* argv){
    //char buffer[100]= {0};
    char ack[13] = "acknowledged";
    SubscriberSockfd = socket(AF_INET,SOCK_STREAM,0);
    if(SubscriberSockfd < 0) {
        error("ERROR opening socket");
    }
    
    bzero(&server_address,sizeof(server_address));
    server_address.sin_family=AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port=htons(SERV_PORT);
    
    inet_pton(AF_INET,SERV_IP,&server_address.sin_addr);
    
    if (connect(SubscriberSockfd,(struct sockaddr*)&server_address,sizeof(server_address)) < 0) {
        error("ERROR connecting");
        exit(-1);
    }
    cout<<"Connected to server";
    recv(SubscriberSockfd, buffer,sizeof(buffer),0);
    cout << buffer << "\n";
    send(SubscriberSockfd,ack,sizeof(ack),0);
    /**int received_bytes = recv(SubscriberSockfd, buffer, sizeof(buffer), 0);
     if(received_bytes < 0){
     cout << "error receiving files\n\n";
     }
     cout << "%s" << buffer;
     **/
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
    cout << "OK";
    bzero(&publisher_as_server,sizeof(publisher_as_server));
    publisher_as_server.sin_family = AF_INET;
    publisher_as_server.sin_addr.s_addr = INADDR_ANY;
    publisher_as_server.sin_port = htons(SUB_PUB_PORT);
    
    inet_pton(AF_INET,PUBLISHER_IP,&publisher_as_server.sin_addr);
    if (connect(SubToPubSockfd,(struct sockaddr *) &publisher_as_server,sizeof(publisher_as_server)) < 0) {
        error("ERROR connecting");
    }
    cout<<"Connected to server";
}

int main(int argc, char** argv){
    SubscriberConnection *subscriber = new SubscriberConnection();
    subscriber->connectToServer(argv[1]);
    //subscriber->ReceiveFileFromPublisher();
}
