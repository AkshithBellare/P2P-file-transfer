#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#define SERV_PORT 12345

int main(int argc, char** argv){
	int i,j;
	char s[80],f[80], msg[1024];
	char hello[] = "Hello from server!";
	FILE *fp;

	char buffer[1024];
	char exitcode[] = "3";

	struct sockaddr_in servaddr, cliaddr;

	int listenfd, connfd, clilen; int msglen;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd){
		printf("Socket successfully created!\n");
	}else {
		printf("Error in creating socket!");
		exit(0);
	}
	
	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

	listen(listenfd, 2);
	printf("Server listening on port %d...\n",SERV_PORT);
	clilen = sizeof(cliaddr);

	while(1){
			
		if(connfd<1){
			connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
			printf("Connection accepted by server! \n");
			send(connfd, hello, sizeof(hello), 0);
			printf("Hello message sent\n");
		}

		if(connfd>0){
			recv(connfd, buffer, 1024, 0);

			if(strcmp(buffer,"ls")==0){
				fp = popen("ls", "r");
				if(fp == NULL){
					printf("File doesn't exist!\n");
				}
				while(fscanf(fp, "%s", msg) != EOF){
					send(connfd, msg, sizeof(msg), 0);
					printf("%s\n", msg);
				}
			}
			else if(strcmp(buffer, "3")==0){
				close(connfd);
				printf("Connection closing...\n");
				exit(0);
			}
		}

		if(strcmp(buffer,exitcode)==0){
			close(listenfd);
			printf("Socket closed\n");
			exit(0);
		}
	}
}