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

//defining ports
#define SERV_PORT 5000
#define PORT_PUB_SUB 7000

//#define SERV_IP "localhost"
std::string KEY="123456";

using namespace std;

//string KEY;

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
	void connectToServer(char *SERV_IP); 
	void listenForSubscriber(); 
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

string Key_Gen(char *SERV_IP){
	string key = "";
	int str_len = strlen(SERV_IP);
	for ( int i = 0; i < 6 ; i++ ){
		//key+=append((char)SERV_IP[i]);
	}
}

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
void PublisherConnection::connectToServer(char *SERV_IP)
{
	//only runs for first time connection and only then
	ALCSockFd = socket(AF_INET, SOCK_STREAM, 0);
	if (ALCSockFd < 0)
	{
		error("ERROR opening socket");
	}
	bzero(&ALCServAddr, sizeof(ALCServAddr));
	ALCServAddr.sin_family = AF_INET;
	//ALCServAddr.sin_addr.s_addr = INADDR_ANY;
	ALCServAddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, SERV_IP, &ALCServAddr.sin_addr);
	if (connect(ALCSockFd, (struct sockaddr *)&ALCServAddr, sizeof(ALCServAddr)) < 0)
	{
		error("ERROR connecting");
	}
	cout << "Connected to server"<<endl;
	recv(ALCSockFd, buffer, 6, 0);
	cout<<buffer<<endl;
	write(ALCSockFd,KEY.c_str(),sizeof(KEY));
	int ch;
	do{
		cout<<"Do you want to see the list of categories(1) or send new file(2)  0 for exit?"<<endl;
		cin>>ch;
		switch(ch)
		{
		case 1:{
			send(ALCSockFd, "1", 2, 0);
			cout<<"Displaying list"<<endl;
			serverShowsList();
			break;
		}
		case 2:{
			send(ALCSockFd, "2", 2, 0);
			cout<<"Enter the category"<<endl;
			sendCategoryAndFile();
			break;
		}
		case 0:{
			cout<<"Closing socket and exiting"<<endl;
			close(ALCSockFd);
			exit(0);
			break;
		}
		default:
			cout<<"Invalid choice!"<<endl;
			break;
		}
	}while(ch!=0);
}

void PublisherConnection::serverShowsList()
{
	
	bzero(buffer, 256);
	if ( read(ALCSockFd, buffer, 256 ) < 0 ){
		error("ERROR reading from socket\n");
		exit(0);
	}
	cout<<buffer<<endl;
}

void PublisherConnection::sendCategoryAndFile()
{
	string category, fileName;
	int n;
	cout << "Enter Category\n";
	cin >> category;
	cout << "Enter File name\n";
	cin >> fileName;
	string buf = category + ":" + fileName;
	n = write(ALCSockFd, buf.c_str(), buf.length()); 
	if (n < 0)
		error("Error writing to socket\n");
	bzero(buffer, sizeof(buffer));
	recv(ALCSockFd, buffer, sizeof(buffer), 0);
}
int main(int argc, char *argv[])
{
	PublisherConnection *withServer = new PublisherConnection();
	
	if(argc < 2){
		cout<<"Please enter IP address"<<endl;
	}else
		withServer->connectToServer(argv[1]);
}