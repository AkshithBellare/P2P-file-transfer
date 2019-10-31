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

#define SERV_PORT 5000
#define PORT_PUB_SUB 7000

using namespace std;

class PublisherConnection
{
	public :
		struct sockaddr_in ALSServAddr;
		int ALSSockFd,ALSNewSockFd;
		int ALCSockFd;
		char buffer[4098];
		socklen_t SubLen;
     	struct sockaddr_in ALCServAddr, Sub_Addr;

		void connectToServer(char* argv);//acts like client
		void listenForSubscriber();//server
		void serverShowsList();
		void sendCategoryAndFile();
		void askForFile();
		void sendFileToSub();
		void error(const char *msg)
		{
    		perror(msg);
    		exit(0);
		}
};
void PublisherConnection::connectToServer(char* argv){
	ALCSockFd = socket(AF_INET,SOCK_STREAM,0);
    if(ALCSockFd < 0) {
        error("ERROR opening socket");
    }
    
    bzero(&ALCServAddr,sizeof(ALCServAddr));
    ALCServAddr.sin_family=AF_INET;
    ALCServAddr.sin_addr.s_addr = INADDR_ANY;
    ALCServAddr.sin_port=htons(SERV_PORT);
    
    inet_pton(AF_INET,argv,&ALCServAddr.sin_addr);
    
    if (connect(ALCSockFd,(struct sockaddr*)&ALCServAddr,sizeof(ALCServAddr)) < 0) {
        error("ERROR connecting");
        exit(-1);
    }
    cout<<"Connected to server";
    recv(ALCSockFd, buffer,sizeof(buffer),0);
    cout << buffer << "\n";
}
void PublisherConnection::listenForSubscriber(){
	 
	 ALSSockFd = socket(AF_INET, SOCK_STREAM, 0);
     if (ALSSockFd < 0) 
        error("ERROR opening socket");
     
	 bzero((char *) &ALSServAddr, sizeof(ALSServAddr));
     
	 ALSServAddr.sin_family = AF_INET;
     ALSServAddr.sin_addr.s_addr = INADDR_ANY;
     ALSServAddr.sin_port = htons(PORT_PUB_SUB);
     
	 if (bind(ALSSockFd, (struct sockaddr *) &ALSServAddr,sizeof(ALSServAddr)) < 0){
        error("ERROR on binding");	
     }
     listen(ALSSockFd,5);
     SubLen = sizeof(Sub_Addr);
     ALSNewSockFd = accept(ALSSockFd,(struct sockaddr *) &Sub_Addr,&SubLen);
     if (ALSNewSockFd < 0){
     	error("ERROR on accept");
     }
	 else{
		 cout<<"Connected to Subscriber";
	 }
	 send(ALSNewSockFd,"connected",10,0);
}
void PublisherConnection::serverShowsList(){
	bzero(buffer,256);
    int n = read(ALCSockFd,buffer,255);
    if (n < 0) 
        error("ERROR reading from socket");
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
void PublisherConnection::askForFile(){
	int n;
	char * message="What is the required file";
	send(ALCSockFd,message,sizeof(message),0); 
	if (n < 0) 
    	error("Error writing to socket");
	else
		bzero(buffer,256);
		n = read(ALCSockFd,buffer,255);
		if (n < 0) 
			error("ERROR reading from socket");
		else
		{
			sendFileToSub();
		}
}
void PublisherConnection::sendFileToSub(){
	char filename[100];
	
	recv(ALSSockFd,filename,sizeof(filename),0);
	
	FILE *f;
    int words = 0;
    char c;
    f=fopen(filename,"r");
    while((c=getc(f))!=EOF)			//Counting No of words in the file
	{	
		fscanf(f , "%s" , buffer);
		if(isspace(c)||c=='\t')
		words++;	
	}  
	write(ALSNewSockFd, &words, sizeof(int));
	rewind(f);
    char ch ;
       while(ch != EOF)
      {		
		fscanf(f , "%s" , buffer);
		write(ALSNewSockFd,buffer,512);
		ch = fgetc(f);
      }
	printf("The file was sent successfully");
}

int main(int argc,char *argv[]){
	PublisherConnection *obj=new PublisherConnection();
	//obj->connectToServer(argv[1]);
	//obj->serverShowsList();
	//obj->sendCategoryAndFile();
	//server asks for file and category
	obj->listenForSubscriber();
	//obj->sendFileToSub();
	//obj->askForFile();
}