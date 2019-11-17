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
#include <bits/stdc++.h>

using namespace std;

#define SERV_PORT 6000
#define SUB_PUB_PORT 7000

class SubPub
{
public:
    char buffer[4098]; //this buffer is used to receive messages
    struct sockaddr_in server_address;
    int SubscriberSockfd;
    int SubToPubSockfd; //socket for publisher to subscriber connection
    struct sockaddr_in publisher_as_server;

    //publisher side functions
    void ReceiveMessageFromPublisher();
    void connectToPublisher(char *Pub_IP);
    void ReceiveFileFromPublisher();
    void SendMessageToPublisher(char *message);
    string receive(int);
    void error(const char *msg)
    {
        perror(msg);
        exit(0);
    }
};
string SubPub::receive(int socket)
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


void SubPub::ReceiveMessageFromPublisher()
{ //To receive messages from the Publisher
    char buff[1024];
    int n;

    bzero(buff, sizeof(buff));
    read(SubToPubSockfd, buff, sizeof(buff));
    printf("From Publisher : %s", buff);
    bzero(buff, sizeof(buff));
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
        ;
    write(SubToPubSockfd, buff, sizeof(buff));
    ReceiveFileFromPublisher();
}

void SubPub::ReceiveFileFromPublisher()
{
    char buff[1024];
    string check="verified";
    int n;
    //sending key
    bzero(buff,sizeof(buff));
    read(SubToPubSockfd,buff,sizeof(buff));
    cout<<buff<<"\n";
    
    bzero(buff, sizeof(buff));
    n=0;
    while ((buff[n++] = getchar()) != '\n');

    write(SubToPubSockfd, buff, sizeof(buff));

    //receiving verification
    bzero(buff, sizeof(buff));
    read(SubToPubSockfd, buff, sizeof(buff));
    string reply(buff);
    if (reply.compare(check) == 0)
    {
        cout << "Enter name of file required\n";
        bzero(buff, sizeof(buff));
        n = 0;
        while ((buff[n++] = getchar()) != '\n');
        write(SubToPubSockfd, buff, sizeof(buff));
        cout<<"File request sent \n";
        int ch = 0;
        buff[n-1] = '\0';
        char buffer[256] = {0};
        FILE *fp;
        fp = fopen(buff, "a");
        int words;
        read(SubToPubSockfd, &words, sizeof(int));
        while (ch != words)
        {
            read(SubToPubSockfd, buffer, 255);
            fprintf(fp, "%s", buffer);
            ch++;
        }
        printf("The file was received successfully\n");
    }
    else
    {
        cout<<buff<<"\n";
        ReceiveFileFromPublisher();
    }
}
void SubPub::connectToPublisher(char *Pub_IP)
{ //connection to publisher works

    SubToPubSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (SubToPubSockfd < 0)
    {
        error("ERROR opening socket");
    }
    else
        printf("Socket successfully created..\n");

    bzero(&publisher_as_server, sizeof(publisher_as_server));

    publisher_as_server.sin_family = AF_INET;
    publisher_as_server.sin_addr.s_addr = inet_addr(Pub_IP);
    publisher_as_server.sin_port = htons(SUB_PUB_PORT);

    if (connect(SubToPubSockfd, (struct sockaddr *)&publisher_as_server, sizeof(publisher_as_server)) < 0)
    {
        error("ERROR connecting");
        exit(0);
    }
    cout << "Connected to Publisher\n";
    ReceiveFileFromPublisher();
}

int main(int argc, char **argv)
{
    SubPub *subscriber = new SubPub();
    if(argc>2){
        subscriber->connectToPublisher(argv[1]);
    }else
    {
        printf("Please enter IP!");
    }
    
}