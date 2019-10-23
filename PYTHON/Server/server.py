import socket
import sys
def searchFile(fname,database):
	if fname in database:
		return database.index(fname)
	else:
		return -1
	
port=3000
database=[]
ipdatabase=[]
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
	print('Waiting for task\n1)Add to database\n2)File details')
	task=c.recv(1024).decode()
	if task==1:
		c.send("Enter file name and port no")
		details=c.recv(1024).decode()
		database.append(details)
		ipdatabase.append(addr)
		c.send("Added to database")
	elif task==2:
		c.send("Enter file name")
		fname=c.recv(1024).decode()
		index=searchFile(fname,database)
		if index==-1:
			c.send("File not present")
		else:
			c.send("",ipdatabase[index])
	else:
		c.send("Not a valid option")

c.close()




