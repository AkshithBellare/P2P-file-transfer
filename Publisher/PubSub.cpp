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
using namespace std;

string KEY="";


class PubSub
{
public:
    struct sockaddr_in ALSServAddr;
    int ALSSockFd, ALSNewSockFd;
    int ALCSockFd;
    char buffer[4098];
    socklen_t SubLen;
    struct sockaddr_in ALCServAddr, Sub_Addr;

    void listenForSubscriber(); 
    void askForFile();
    void sendFileToSub(const char *file);
    void error(const char *msg)
    {
        perror(msg);
        exit(0);
    }
};
void PubSub::listenForSubscriber()
{

    ALSSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (ALSSockFd < 0)
        error("ERROR opening socket\n");
    else
        printf("Socket successfully created..\n");
    int on = 1;
    int rc = setsockopt(ALSSockFd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    if (rc<0){
        perror("setsockopt fail\n");
        exit(0);
    }
    bzero((char *)&ALSServAddr, sizeof(ALSServAddr));
    ALSServAddr.sin_family = AF_INET;
    //ALSServAddr.sin_addr.s_addr = INADDR_ANY;     //for localhost
    ALSServAddr.sin_port = htons(PORT_PUB_SUB);
    if (bind(ALSSockFd, (struct sockaddr *)&ALSServAddr, sizeof(ALSServAddr)) < 0)
    {
        error("ERROR on binding\n");
    }
    
	ALSNewSockFd = 0;
	KEY = KEY.append("\n");

	while(ALSNewSockFd == 0){
		if ((listen(ALSSockFd, 5)) != 0)
		{
			printf("Listen failed...\n");
			exit(0);
		}
		else
			printf("Publisher listening..\n");

		SubLen = sizeof(Sub_Addr);
		if(ALSNewSockFd == 0)
			ALSNewSockFd = accept(ALSSockFd, (struct sockaddr *)&Sub_Addr, &SubLen);
		if (ALSNewSockFd < 0)
		{
			error("ERROR on accept\n");
		}
		else
			cout << "Connected to Subscriber\n";
		askForFile();
		ALSNewSockFd = close(ALSNewSockFd);
	}

}

void PubSub::askForFile()
{
    char buff[1024];
    int n, i;

    //asks to enter key
    string message="Please enter the key";
    write(ALSNewSockFd,message.c_str(),sizeof(message));
    cout<<"Asked For key\n";
    
    //receives key;
    bzero(buff, sizeof(buff));
    read(ALSNewSockFd, buff, sizeof(buff));
    if (KEY.compare(buff) == 0)
    {
        string message = "verified";
        write(ALSNewSockFd, message.c_str(), sizeof(buff));

        //receving file name
		cout<<"Recieving file name"<<endl;
        bzero(buff, sizeof(buff));
        read(ALSNewSockFd, buff, sizeof(buff));
        cout << "Publisher asked for: " << buff<<"\n";
        sendFileToSub(buff);
        
    }
    else{
        string message = "Wrong key";
        write(ALSNewSockFd, message.c_str(), sizeof(buff));
        askForFile();
    }
}
void PubSub::sendFileToSub(const char *fileName)
{
    FILE *f;
    int words = 0;
    char c;
    cout << fileName << endl;
    
    string fname = fileName;
    fname = fname.substr(0,fname.length()-1);
    
    f = fopen(fname.c_str(), "r");
    
    if(f!=NULL){
    while ((c = getc(f)) != EOF) //Counting No of words in the file
    {
        if (isspace(c) || c == '\t')
            words++;
    }
    write(ALSNewSockFd, &words, sizeof(int));
    rewind(f);
    char ch;
    while (ch != EOF)
    {
        fscanf(f, "%s", buffer);
        write(ALSNewSockFd, buffer, 255);
        ch = fgetc(f);
    }
    printf("The file was sent successfully\n");
    }
    else {
        printf("The file doesnt exist\n");
    }
}

int main(int argc, char *argv[])
{
    PubSub *withSubscriber = new PubSub();
    if(argc<2){
        cout<<"Please enter key!"<<endl;
    }else{
        KEY = argv[1];
        withSubscriber->listenForSubscriber();
    }
}