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

//ports setting
#define SERV_PORT 8000
#define SUB_PUB_PORT 7000

string Publisher_IP;
string KEY;

class SubscriberConnection{
public:
    char buffer[4098]; //this buffer is used to receive messages
    struct sockaddr_in server_address;
    int SubscriberSockfd; 
    int SubToPubSockfd; //socket for publisher to subscriber connection
    struct sockaddr_in publisher_as_server;
    
    //server side functions
    void connectToServer(char* argv);
    void serverShowsListOfTopics();
    void subscribe();
    void ReceiveMessageFromServer();
    string ReceiveKeyFromServer();
    void SendMessageToServer(const char* message);
   
    void setPublisherIP(char* PublisherIP);
    char* getPublisherIP();
    
    void error(const char *msg){
        perror(msg);
        exit(0);
    }
};
void SubscriberConnection::connectToServer(char* SERV_IP){
    
    SubscriberSockfd = socket(AF_INET,SOCK_STREAM,0);
    if(SubscriberSockfd < 0) {
        error("ERROR opening socket");
    }
    
    bzero(&server_address,sizeof(server_address));

    //init server address values
    server_address.sin_family=AF_INET;
    //server_address.sin_addr.s_addr = INADDR_ANY;      //only for localhost
    server_address.sin_port=htons(SERV_PORT);
    
    inet_pton(AF_INET,SERV_IP,&server_address.sin_addr);
    
    if (connect(SubscriberSockfd,(struct sockaddr*)&server_address,sizeof(server_address)) < 0) {
        error("ERROR connecting");
        exit(-1);
    }
    string subName;

    cout<<"Connected to server"<<endl;
    cout<<"Enter your name:"<<endl;
    cin>>subName;
    write(SubscriberSockfd,subName.c_str(),subName.length());
    int ch;
	do{
        cout<<"Do you want to see the list of categories(1) or request file(2)  or subscribe(3) 0 for exit?"<<endl;
        cin>>ch;
        switch(ch)
		{
            case 1:{
                send(SubscriberSockfd, "1", 2, 0);
                bzero(buffer, sizeof(buffer));
                if ( read(SubscriberSockfd, buffer, sizeof(buffer) ) < 0){
                    error("ERROR reading from socket\n");
                    exit(0);
                }
                if(buffer[0]=='Y')
                    cout<<"NEW NOTIFICATION"<<endl;
                cout<<"Displaying list"<<endl;
                string modBuff=buffer;
                modBuff=modBuff.substr(1);
                cout<<modBuff<<endl;
                break;
            }

            case 2:{
                string category, filename;
                send(SubscriberSockfd, "2", 2, 0);
                cout<<"Enter the category"<<endl;
                cin>>category;
                send(SubscriberSockfd, category.c_str(), category.length(), 0);
                bzero(buffer, sizeof(buffer));
                read(SubscriberSockfd, buffer, sizeof(buffer));
                if(buffer[0]=='Y')
                    cout<<"NEW NOTIFICATION"<<endl;
                string modBuff=buffer;
                modBuff=modBuff.substr(1);
                filename = modBuff;
                cout<<"Files : \n"<<filename<<endl;
                string fName;
                cout<<"Enter which file u want"<<endl;
                cin>>fName;
                    send(SubscriberSockfd, fName.c_str(), fName.length(), 0);
                    bzero(buffer,sizeof(buffer));
                    int recv_file_size;
                    if( recv_file_size = read(SubscriberSockfd, buffer, sizeof(buffer)) > 0 ){
                        cout<<"Recieved IP:key "<<buffer<<endl;
                        //buffer[recv_file_size] = "\0";
                        string IP_KEY = buffer;
                        int pos = IP_KEY.find(":");
                        Publisher_IP = IP_KEY.substr(0, pos);
                        KEY = IP_KEY.substr(pos+1);
                        cout<<"IP : "<<Publisher_IP<<"\nkey : "<<KEY<<endl;
                    }
                
                break;
            
            }

            case 3:{
                string category;
                send(SubscriberSockfd, "3", 2, 0);
                bzero(buffer, sizeof(buffer));
                if ( read(SubscriberSockfd, buffer, sizeof(buffer) ) < 0){
                    error("ERROR reading from socket\n");
                    exit(0);
                }
                if(buffer[0]=='Y')
                    cout<<"NEW NOTIFICATION"<<endl;
                cout<<"Displaying list"<<endl;
                string modBuff=buffer;
                modBuff=modBuff.substr(1);
                cout<<modBuff<<endl;
                cout<<"Enter the category you want to subscribe to:"<<endl;
                cin>>category;
                write(SubscriberSockfd, category.c_str(), category.length());
                break;
            }

            case 0:{
                cout<<"Closing socket and exiting"<<endl;
                close(SubscriberSockfd);
                exit(0);
                break;
            }

            default:{
                cout<<"Invalid choice!"<<endl;
                break;
            }
        }
    }while(ch!=0);
}
int main(int argc, char** argv){
    SubscriberConnection *subscriber = new SubscriberConnection();
    if(argc < 2){
        cout<<"PLEASE ENTER IP "<<endl;
    }
    else
        subscriber->connectToServer(argv[1]);
}