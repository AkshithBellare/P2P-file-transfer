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

int main(int argc, char **argv){
	int i,j;

	char buf[2048];string str;
	int choice;char *msg;
	size_t len =0;
	int msg_size;int count=0;
	char *command;

	FILE *fp;

	struct sockaddr_in servaddr;

	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(8888);

	inet_pton(sockfd, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	cout<<"Connection to server established!\n";

	cout<<"Data from server : \n";
	recv(sockfd, buf, 2048, 0);
	cout<< buf<<"\n";
	
	while(1){
		//	bzero(buf, 2048);
		cout<<">";
		getline(cin,str);
		while(str[count]!='\0')
			count++;
		for(int i=0;i<count;i++)
			buf[i] = str[i];
		send(sockfd, buf, sizeof(buf), 0);
		while((msg_size = recv(sockfd, buf, sizeof(buf), 0))>0){
			cout<<buf;
		}
	}
}