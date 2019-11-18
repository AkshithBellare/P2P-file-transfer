# Publish-Subscriber-File-transfer
P2P-file-transfer using socket programming

A Simple Client - Server Socket program in C++ for multiple clients for two different services.

How to run : 

Server:
  1. Run Server program using "g++ ServerConnection.cpp -g -l sqlite3 -o Server"

Publisher:
  1. Run the publisher to server connection using "g++ -g PubServer.cpp -o PubServer"
  2. Run the file transfer program "g++ -g PubSub.cpp -o PubSub"
  
Subscriber:
  1. Run the Subscriber to server connection using "g++ -g SubServer.cpp -o SubServer"
  2. Run the file transfer program "g++ -g SubPub.cpp -o SubPub"
