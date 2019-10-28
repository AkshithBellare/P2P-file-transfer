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

#define SERV_PORT 8888
#define PORT_PUB_SUB 4000
#define SERV_IP "10.50.19.120"

using namespace std;

class PublisherConnection
{
	public :
		struct sockaddr_in ALSServAddr;
		int ALSSockFd;
		int ALCSockFd, ALCNewSockFd;
		char buffer[4098];
		socklen_t SubLen;
     	struct sockaddr_in ALCServAddr, Sub_Addr;

		void connectToServer();//acts like client
		void listenForSubscriber();//server
		void serverShowsList();
		void sendCategoryAndFile();
		void sendFileToSub(char *file);
		void error(const char *msg){
    		perror(msg);
    		exit(0);
		}
};
void PublisherConnection::connectToServer(){

	ALSSockFd=socket(AF_INET,SOCK_STREAM,0);
	if (ALSSockFd < 0) {
        error("ERROR opening socket");
	}
	bzero(&ALSServAddr,sizeof(ALSServAddr));
	ALSServAddr.sin_family=AF_INET;
	ALSServAddr.sin_port=htons(SERV_PORT);

	inet_pton(AF_INET,SERV_IP,&ALSServAddr.sin_addr);
    if (connect(ALSSockFd,(struct sockaddr *) &ALSServAddr,sizeof(ALSServAddr)) < 0) {
        error("ERROR connecting");
    }
    cout<<"Connected to server";
}
void PublisherConnection::listenForSubscriber(){
	 ALCSockFd = socket(AF_INET, SOCK_STREAM, 0);
     if (ALCSockFd < 0) 
        error("ERROR opening socket");
     bzero((char *) &ALCServAddr, sizeof(ALCServAddr));
     ALCServAddr.sin_family = AF_INET;
     ALCServAddr.sin_addr.s_addr = INADDR_ANY;
     ALCServAddr.sin_port = htons(PORT_PUB_SUB);
     if (bind(ALCSockFd, (struct sockaddr *) &ALCServAddr,sizeof(ALCServAddr)) < 0){
        error("ERROR on binding");	
     }
     listen(ALCSockFd,5);
     SubLen = sizeof(Sub_Addr);
     ALCNewSockFd = accept(ALCSockFd,(struct sockaddr *) &Sub_Addr,&SubLen);
     if (ALCNewSockFd < 0){
     	error("ERROR on accept");
     }

}
void PublisherConnection::serverShowsList(){
	bzero(buffer,256);
    int n = read(ALCSockFd,buffer,255);
    if (n < 0){
        error("ERROR reading from socket");
	}
	cout << buffer;
}

void PublisherConnection::sendCategoryAndFile(){
	string category,fileName;
	int n;
	cout<<"Enter Category";
	cin>>category;
	cout<<"Enter File name";
	cin>>fileName;

	string buf=category+":"+fileName;

	
	n = write(ALCSockFd,buf.c_str(),255);//Hardcoded length 
	if (n < 0) 
    	error("Error writing to socket");
}

void PublisherConnection::sendFileToSub(char *file){
	char *fileContents;
	FILE *fp;
	fp = fopen(file, "r+");
	if(fp == NULL){
		cout<<"Error opening file, exiting...\n";
		return;
	}
	fscanf(fp, "%c", fileContents);
	int sent_bytes = send(PORT_PUB_SUB, fileContents, sizeof(fileContents), 0);
	if(sent_bytes>0)
		cout<<"Sent files succesfully!\n";
}

int main(int argc,char *argv[]){
	PublisherConnection *publisher=new PublisherConnection();
	publisher->connectToServer();
	publisher->serverShowsList();
	publisher->sendCategoryAndFile();


	//server asks for file and category

	publisher->listenForSubscriber();



}