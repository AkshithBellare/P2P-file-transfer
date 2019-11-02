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
#include <netdb.h>

using namespace std;

#define SERV_PORT 6000
#define SUB_PUB_PORT 7000

char *Publisher_IP = "127.0.0.1";
long key = 0;

class SubscriberConnection
{
public:
    char buffer[4098]; //this buffer is used to receive messages
    struct sockaddr_in server_address;
    int SubscriberSockfd;
    int SubToPubSockfd; //socket for publisher to subscriber connection
    struct sockaddr_in publisher_as_server;

    //server side functions
    void connectToServer(char *argv);
    void serverShowsListOfTopics();
    void subscribe();
    void ReceiveMessageFromServer();
    long ReceiveKeyFromServer();
    void SendMessageToServer(char *message);

    void setPublisherIP(char *PublisherIP);
    char *getPublisherIP();

    //publisher side functions
    void connectToPublisher(char *Pub_IP);
    void ReceiveFileFromPublisher();
    void ReceiveMessageFromPublisher();
    void SendMessageToPublisher(char *message);
    string receive(int);
    void error(const char *msg)
    {
        perror(msg);
        exit(0);
    }
};
string SubscriberConnection::receive(int socket)
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
void SubscriberConnection::ReceiveMessageFromServer()
{ //To receive messages from the server
    /*char buffer[4098] = {0};
    recv(SubscriberSockfd, buffer, sizeof(buffer), 0);
    cout << buffer;*/
    string message=receive(SubscriberSockfd);
    cout<<message;
}

void SubscriberConnection::SendMessageToServer(char *message)
{ //To send messages to the server
    send(SubscriberSockfd, message, sizeof(message), 0);
}

void SubscriberConnection::ReceiveMessageFromPublisher()
{ //To receive messages from the Publisher
    char buffer[4098] = {0};
    int bytesRcvd;
    string str;

    do
    {
        bytesRcvd = recv(SubToPubSockfd, buffer, sizeof(buffer) - 2, 0);
        buffer[bytesRcvd] = '\0';
        str += buffer;
    } while (bytesRcvd != 0);

    cout << str;
}

long SubscriberConnection::ReceiveKeyFromServer()
{
    char *buffer;
    int n = recv(SubscriberSockfd, buffer, sizeof(buffer), 0);
    key = (long)buffer;
}

void SubscriberConnection::SendMessageToPublisher(char *message)
{ //To send messages to the Pubisher
    send(SubToPubSockfd, message, sizeof(message), 0);
}

void SubscriberConnection::ReceiveFileFromPublisher()
{ //To receive file from the publisher
    char *fileName = "test.txt";
    //getline(cin,fileName);
    send(SubscriberSockfd, fileName, sizeof(fileName), 0);
    int ch = 0;
    char buffer[256] = {0};
    FILE *fp;
    recv(SubToPubSockfd, fileName, strlen(fileName), 0);
    fp = fopen(fileName, "a");
    int words;

    read(SubToPubSockfd, &words, sizeof(int));

    while (ch != words)
    {
        read(SubToPubSockfd, buffer, 255);
        fprintf(fp, "%s", buffer);
        ch++;
    }
    printf("The file was received successfully\n\n");
}

void SubscriberConnection::connectToServer(char *argv)
{
    SubscriberSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (SubscriberSockfd < 0)
    {
        error("ERROR opening socket");
    }
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv, &server_address.sin_addr);
    if (connect(SubscriberSockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        error("ERROR connecting");
        exit(-1);
    }
    cout << "Connected to server";
    send(SubscriberSockfd, "hello", 6, 0);
    ReceiveMessageFromServer();
}

void SubscriberConnection::subscribe()
{ //subscriber is done
    SendMessageToServer("Subscribe");
    ReceiveMessageFromServer(); //Receive list of topics from the subscriber
    cout << "Enter the topics you want to subscribe to\n";
    char *topic; //Topic to subscribe
    cin >> topic;
    SendMessageToServer(topic);
    //Modify the below part based on how the data is stored in the database
    ReceiveMessageFromServer(); //receive table of IPs and file names
    cout << "Enter the Publisher IP address:";
    cin >> Publisher_IP;
    SendMessageToServer(Publisher_IP);
    ReceiveKeyFromServer(); //Receive key of the publisher from the server
}

void SubscriberConnection::connectToPublisher(char *Pub_IP)
{ //connection to publisher works

    SubToPubSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (SubToPubSockfd < 0)
    {
        error("ERROR opening socket");
    }
    struct hostent *server;
    server = gethostbyname(Pub_IP);
    bzero(&publisher_as_server, sizeof(publisher_as_server));
    publisher_as_server.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&publisher_as_server.sin_addr.s_addr, server->h_length);
    //publisher_as_server.sin_addr.s_addr = INADDR_ANY;
    publisher_as_server.sin_port = htons(SUB_PUB_PORT);

    inet_pton(AF_INET, Pub_IP, &publisher_as_server.sin_addr);

    if (connect(SubToPubSockfd, (struct sockaddr *)&publisher_as_server, sizeof(publisher_as_server)) < 0)
    {
        error("ERROR connecting");
    }
    cout << "Connected to Publisher\n";
    cout << "Message from Publisher:";
    ReceiveMessageFromPublisher();
}

int main(int argc, char **argv)
{
    SubscriberConnection *subscriber = new SubscriberConnection();
    subscriber->connectToServer("127.0.0.1");
    //SubscriberConnection *connectToPublisher = new SubscriberConnection();
    //cout << "Trying to connect to server:" << argv[1];
    //subscriber->connectToPublisher("127.0.0.1");
    //subscriber->connectToPublisher("10.53.95.124");
    /*
    pid_t pid = fork();
    
    
    if( pid == 0 ){
        cout << "opening a child";
        subscriber->connectToServer("192.168.43.30");
        key=3;

    }else if( pid > 0 ){
        cout<<"ChilPAR\n";
        if(key > 0 ){
        subscriber->connectToPublisher("192.168.43.47");
        subscriber->ReceiveFileFromPublisher();
        cout << "PUBLISHER\n";
        }
    }else{
        cout<<"fork failed\n";
    }

    /**
    subscriber->connectToServer(argv[1]);//done
    //subscriber->subscribe();//done
    //char* PublisherIP;
    //char* filename;
    //PublisherIP = subscriber->getPublisherIP();
    subscriber->connectToPublisher("127.0.0.1");
    char fileName[100];
    cout << "Enter the name of the file you want to receive:";
    cin >> fileName;
    subscriber->ReceiveFileFromPublisher(fileName);
    **/
}