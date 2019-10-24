// #include <iostream>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <string.h>
// #include <arpa/inet.h>
// #define SERV_PORT 12345
// using namespace std;

// class Socket{
// 	public:
// 		int createSocket();
// 		int serverConnect();
// }

// int Socket::createSocket(){
// 	struct sockaddr_in servaddr;
	
// 	int sockfd;
// 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 	bzero(&servaddr, sizeof(servaddr));

// 	servaddr.sin_family = AF_INET;
// 	servaddr.sin_port = htons(SERV_PORT);

// 	inet_pton(sockfd, argv[1], &servaddr.sin_addr);

// 	return sockfd;
// }

// int Socket::serverConnect(){

// }














// int main(int argc, char **argv){
// 	int i,j;

// 	char file[1024],hello[80];
// 	int choice;char msg[1024];
// 	char *command;

// 	FILE *fp;

// 	if(argc<3){
// 		cout<<"Not enough arguments!\n";
// 		exit(0);
// 	}

// 	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
// 	cout<<"Connection to server established!\n";

// 	cout<<"Data from server : \n";
// 	read(sockfd, file, sizeof(file));
// 	cout<<"%s\n", file;
	
// 	while(1){
// 		cout<<"Enter an option\n";
// 		cout<<"1. Check files\n";
// 		cout<<"2. Receive a file\n";
// 		cout<<"3. Submit a file\n";
// 		cin>>choice;

// 		switch(choice){
// 			case 1:
// 				command = "ls";				
// 				send(sockfd, command, sizeof(command), 0);
// 				while(recv(sockfd, msg, sizeof(msg), 0)!=NULL)					
// 					cout<<msg;
// 				cout<<"done!\n";
// 				break;
// 			case 2:
// 				cout<<"Enter file to be received\n";
// 				cin>>file;
// 				send(sockfd, file, sizeof(file), 0);
// 				fp = fopen("received-file", "wb");
// 				while(recv(sockfd, file, sizeof(file), 0)!=NULL){
// 					fwrite(file, 1, sizeof(file),fp);
// 				}
// 				cout<<"file successfully received\n";
// 				break;
// 			case 3:
// 				break;
// 			case 4:
// 				close(sockfd);
// 				cout<<"Exiting...\n";
// 				exit(0);
// 				break;
// 			default:
// 				cout<<"Invalid choice!\n";
// 		}
// 		//send(sockfd, userinput, sizeof(userinput), 0);
// 	}
// }





#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define SERV_PORT 12345
using namespace std;

class Socket{
private:
	struct sockaddr_in servaddr;
	int sockfd;
public:
	int createSocket();
	int connect();
}

int Socket::createSocket(){

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	inet_pton(sockfd, argv[1], &servaddr.sin_addr);

	return sockfd;	
}

int main(int argc, char **argv){

	if(argc<3){
		cout<<"Enter host and port !!\n";
		exit(0);
	}
	
	if(createSocket()){
		cout<<"Error creating socket. Exiting...\n";
		exit(0);
	}
}