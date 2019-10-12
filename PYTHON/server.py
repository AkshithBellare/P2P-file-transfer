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
s.bind(('',port))
s.listen(2)
while True:
	c,addr=s.accept()
	print("Connected to ",addr)
	c.send('Hello ')
	

	fname='hello.pdf'
	f=open(fname,'rb')
	l=f.read(1024)
	while l:
		c.send(l)
		l=f.read(1024)
	f.close()
	print("Sent")
	c.close()



