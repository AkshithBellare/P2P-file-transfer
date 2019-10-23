#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define SERV_PORT 12345

int main(int argc, char **argv){
	int i,j;

	char file[80],hello[80];
	int choice;char msg[1024];
	char *command;

	FILE *fp;

	struct sockaddr_in servaddr;

	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	inet_pton(sockfd, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	printf("Connection to server established!\n");

	printf("Data from server : \n");
	read(sockfd, file, sizeof(file));
	printf("%s\n", file);
	
	while(1){
		printf("Enter an option\n");
		printf("1. Check files\n");
		printf("2. Receive a file\n");
		scanf("%d", &choice);

		switch(choice){
			case 1:
				command = "ls";				
				send(sockfd, command, sizeof(command), 0);
				while(recv(sockfd, msg, sizeof(msg), 0)!=NULL)					
					printf("%s\n", msg);
				break;
			case 2:
				break;
			case 3:
				close(sockfd);
				printf("Exiting...\n");
				exit(0);
				break;
			default:
				printf("Invalid choice!\n");
		}
		send(sockfd, userinput, sizeof(userinput), 0);
	}
}