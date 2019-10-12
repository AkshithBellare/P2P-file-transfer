import socket
import sys
port=input("Enter port number")
try:
	s=socket.socket()
except Exception as e:
	print("Not able to create a socket")
	sys.exit()
else:
	print("Socket created")
s.connect(('127.0.0.1',port))
with open('received_file', 'wb') as f:
    print 'file opened'
    while True:
        print('receiving data...')
        data = s.recv(1024)
        print('data=%s', (data))
        if not data:
            break
        # write data to a file
        f.write(data)
f.close()
print('Received the file')
s.close()