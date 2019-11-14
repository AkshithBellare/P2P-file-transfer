#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include "Database.h"

using namespace std;

#define TRUE   1 
#define FALSE  0 
#define PUB_PORT 5000
#define SUB_PORT 6001

Database *database = new Database();

int main(int argc, char** argv){
    int    i, len, rc, on = 1;
    int    listen_sd, max_sd, new_sd;
    int    desc_ready, end_server = FALSE;
    int    close_conn;
    char   buffer[80];
    struct sockaddr_in addr, cliaddr;
    struct timeval      timeout;
    fd_set              master_set, working_set;


    listen_sd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sd < 0){
        perror("socket");
        exit(0);
    }

    rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    if (rc<0){
        perror("setsockopt fail");
        exit(0);
    }

    rc = ioctl(listen_sd, FIONBIO, (char*)&on);
    if (rc<0){
        perror("ioctl");
        exit(0);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PUB_PORT);
    rc = bind(listen_sd, (struct sockaddr*)&addr, sizeof(addr));
    if(rc<0){
        perror("bind");
        exit(0);
    }

    rc= listen(listen_sd, 10);
    if( rc < 0 ){
        perror("listen");
        exit(0);
    }

    FD_ZERO(&master_set);
    max_sd = listen_sd;
    FD_SET(listen_sd, &master_set);

    do{
        memcpy(&working_set, &master_set, sizeof(working_set));
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        //cout<<"Select started"<<endl;
        rc = select(max_sd+1, &working_set, NULL, NULL, &timeout);
        if( rc < 0 ){
            perror("select");
            exit(0);
        }

        if(rc==0){
            cout<<"timeout"<<endl;
            break;
        }

        desc_ready = rc;
        cout<<"sockets ready for i/o "<<desc_ready<<endl;
        for(i = 0; i<=max_sd && desc_ready>0;i++){
            if(FD_ISSET(i, &working_set)){
                desc_ready-=1;
                if(i == listen_sd){
                    do{
                        int clilen = sizeof(cliaddr);
                        new_sd = accept(i, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
                        if(new_sd<0){
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  accept() failed");
                                end_server = TRUE;
                            }
                            break;
                        }
                        cout<<"New incoming connection "<<inet_ntoa(cliaddr.sin_addr)<<endl;
                        send(new_sd, "Hello", 6, 0);
                        char key[8];
                        recv(new_sd, key, 7, 0);
                        if(strlen(key)>0){
                            cout<<"received key "<<key<<endl;
                            database->addPublisher(inet_ntoa(cliaddr.sin_addr), key);
                        }
                        FD_SET(new_sd, &master_set);
                        if(new_sd>max_sd)
                            max_sd = new_sd;
                    }while(new_sd!=-1);
                }

                else{
                    cout<<"Readable socket "<<endl;
                    close_conn = FALSE;

                  
                        bzero(buffer, sizeof(buffer));
                        rc = recv(i, buffer, sizeof(buffer), 0);
                        if( rc<0 ){
                            if (errno!=EWOULDBLOCK){
                                perror("recv");
                                close_conn = TRUE;
                                close(i);
                            FD_CLR(i, &master_set);
                            }
                            break;
                        }
                        if(rc==0){
                            close_conn=TRUE;
                            cout<<"conn closed"<<endl;
                            close(i);
                            FD_CLR(i, &master_set);
                            break;
                        }       

                        len = rc;
                        cout<<rc<<" bytes received"<<endl;
                        cout<<buffer<<endl;

                        if(strcmp(buffer,"1") == 0 ){
                            cout<<"Sending file deets"<<endl;
                            string CAT_LIST = database->getCategoryList(); 
                            cout<<CAT_LIST<<endl;
                            if(CAT_LIST.length() > 0)
                                send(i, CAT_LIST.c_str(), CAT_LIST.length(), 0);
                            else
                                send(i, "EMPTY", 6, 0); 
                        }else if(strcmp(buffer, "2") == 0){
                            int recv_file_size;
                            if( (recv_file_size = recv(i, buffer, sizeof(buffer), 0)) > 0 ){
                                    cout<<"Received file deets"<<endl;
                                    cout<<buffer<<endl;
                                    string cat_file = buffer;
                                    string category, file;

                                    int pos = cat_file.find(":");

                                    category = cat_file.substr(0, pos-1);
                                    file = cat_file.substr(pos+1, recv_file_size-1);

                                    database->addFile(i, category, file);
                                    send(i, "Added file", 11, 0);
                            }
                        }
                        if (close_conn)
                        {
                            close(i);
                            FD_CLR(i, &master_set);
                            if (i == max_sd)
                            {
                                while (FD_ISSET(max_sd, &master_set) == FALSE)
                                    max_sd -= 1;
                            }
                        }
                }
            }
        }

    }while(end_server!=TRUE);

    
}