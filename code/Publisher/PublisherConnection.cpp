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
#include <bits/stdc++.h>
#define SERV_PORT 5000
#define PORT_PUB_SUB 7000

#define SERV_IP "127.0.0.1"

using namespace std;

class PublisherConnection
{
public:
	struct sockaddr_in ALSServAddr;
	int ALSSockFd, ALSNewSockFd;
	int ALCSockFd;
	char buffer[4098];
	socklen_t SubLen;
	struct sockaddr_in ALCServAddr, Sub_Addr;

	string receive(int);
	void connectToServer(); //acts like client
	void receiveMessageFromServer();
	void sendMessageToServer(char *);
	void receiveMessageFromSubscriber();
	void listenForSubscriber(); //server
	void serverShowsList();
	void sendCategoryAndFile();
	void askForFile();
	void sendFileToSub(const char *file);
	void error(const char *msg)
	{
		perror(msg);
		exit(0);
	}
};
static long key = 123456;
string PublisherConnection::receive(int socket)
{
	char buffer[1024];
	int bytesRead;
	string receivedData;
	do
	{
		bytesRead = recv(socket, buffer, 1024 - 2, 0);
		buffer[1024 - 1] = '\0';
		receivedData += buffer;
	} while (bytesRead != 0);

	return receivedData;
}
void PublisherConnection::receiveMessageFromServer()
{ //To receive messages from the server
	//char buffer[4098] = {0};
	//recv(ALCSockFd, buffer, sizeof(buffer), 0);
	string response = receive(ALCSockFd);
	cout << response;
}
void PublisherConnection::sendMessageToServer(char *message)
{ //To send messages to the server
	send(ALCSockFd, message, sizeof(message), 0);
}
void PublisherConnection::receiveMessageFromSubscriber()
{ //To receive messages from the Publisher
	char buffer[4098] = {0};
	recv(ALSNewSockFd, buffer, sizeof(buffer), 0);
	cout << buffer;
}

void PublisherConnection::connectToServer()
{

	ALCSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (ALCSockFd < 0)
	{
		error("ERROR opening socket");
	}
	bzero(&ALCServAddr, sizeof(ALCServAddr));
	ALCServAddr.sin_family = AF_INET;
	ALCServAddr.sin_addr.s_addr = INADDR_ANY;
	ALCServAddr.sin_port = htons(SERV_PORT);

	inet_pton(AF_INET, SERV_IP, &ALCServAddr.sin_addr);
	if (connect(ALCSockFd, (struct sockaddr *)&ALCServAddr, sizeof(ALCServAddr)) < 0)
	{
		error("ERROR connecting");
	}
	cout << "Connected to server";
	while (1)
		receiveMessageFromServer();
}
void PublisherConnection::listenForSubscriber()
{

	ALSSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (ALSSockFd < 0)
		error("ERROR opening socket");
	else
		printf("Socket successfully created..\n");
	bzero((char *)&ALSServAddr, sizeof(ALSServAddr));
	ALSServAddr.sin_family = AF_INET;
	ALSServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	ALSServAddr.sin_port = htons(PORT_PUB_SUB);
	if (bind(ALSSockFd, (struct sockaddr *)&ALSServAddr, sizeof(ALSServAddr)) < 0)
	{
		error("ERROR on binding");
	}
	else
		printf("Socket successfully binded..\n");
	if ((listen(ALSSockFd, 5)) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Publisher listening..\n");
	SubLen = sizeof(Sub_Addr);
	ALSNewSockFd = accept(ALSSockFd, (struct sockaddr *)&Sub_Addr, &SubLen);
	if (ALSNewSockFd < 0)
	{
		error("ERROR on accept");
	}
	else
		cout << "Connected to Subscriber";
	askForFile();
}
void PublisherConnection::serverShowsList()
{
	bzero(buffer, 256);
	string list = receive(ALSNewSockFd);
	cout << list;
	/*int n = read(ALCSockFd, buffer, 255);
	if (n < 0)
		error("ERROR reading from socket");*/
}
void PublisherConnection::sendCategoryAndFile()
{
	string category, fileName;
	int n;
	cout << "Enter Category";
	cin >> category;
	cout << "Enter File name";
	cin >> fileName;

	string buf = category + ":" + fileName;

	n = write(ALCSockFd, buf.c_str(), 255); //Hardcoded length
	if (n < 0)
		error("Error writing to socket");
}
void PublisherConnection::askForFile()
{
	string key = "123456\n";
	char buff[1024];
	int n, i;

	//asks to enter key
	string message="Please enter the key";
	write(ALSNewSockFd,message.c_str(),sizeof(message));
	cout<<"Asked For key";
	
	//receives key;
	bzero(buff, sizeof(buff));
	read(ALSNewSockFd, buff, sizeof(buff));
	string inputKey(buff);
	if (key.compare(inputKey) == 0)
	{
		string message = "verified";
		write(ALSNewSockFd, message.c_str(), sizeof(buff));

		//receving file name
		bzero(buff, sizeof(buff));
		read(ALSNewSockFd, buff, sizeof(buff));
		cout << "\nPublisher asked for:" << buff;
		sendFileToSub(buff);
		//printf("From Subscriber : %s", buff);
		//}
	}
	else{
		string message = "Wrong key";
		write(ALSNewSockFd, message.c_str(), sizeof(buff));
		askForFile();
	}
}
void PublisherConnection::sendFileToSub(const char *fileName)
{

	FILE *f;
	int words = 0;
	char c;
	f = fopen("test.txt", "r");
	while ((c = getc(f)) != EOF) //Counting No of words in the file
	{
		fscanf(f, "%s", buffer);
		if (isspace(c) || c == '\t')
			words++;
	}
	write(ALSNewSockFd, &words, sizeof(int));
	rewind(f);
	char ch;
	while (ch != EOF)
	{
		fscanf(f, "%s", buffer);
		write(ALSNewSockFd, buffer, 512);
		ch = fgetc(f);
	}
	printf("The file was sent successfully");
}

int main(int argc, char *argv[])
{
	PublisherConnection *withServer = new PublisherConnection();
	PublisherConnection *withSubscriber = new PublisherConnection();
	//cout<<"akm";
	withSubscriber->listenForSubscriber();
	//withServer->connectToServer();
	/*
	pid_t pid = fork();

	if (pid == 0)
	{
		cout << "Server\n";
		withServer->connectToServer();
	}
	else if (pid > 0)
	{
		cout << "Publisher\n";

		withSubscriber->listenForSubscriber();
	}
	else
	{
		cout << "fork failed\n";
	}
	//obj->listenForSubscriber();
	/*obj->serverShowsList();
	obj->sendCategoryAndFile();
	//server asks for file and category
	obj->listenForSubscriber();
	obj->askForFile();*/
}