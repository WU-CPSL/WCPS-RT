import serial
from time import sleep
from thread import *
import sys
import socket
import time
import os


sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('172.20.166.72', 8000)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
sock.listen(2)
serialdata = '00000000 00000000 00000000\n'

def clientthread(conn,addr):
  global serialdata
#infinite loop so that function do not terminate and thread do not end.
  while True:
        if addr[0] =='172.20.166.68':
          global serialdata
          data=conn.recv(27)   
          if data:
                print>>sys.stderr,'received"%s"'% data
                serialdata=data
          else:
                break

        if addr[0] =='172.20.166.113':
          global serialdata
          mesg=conn.recv(16)
          if mesg:
                print>>sys.stderr,'received"%s"'% mesg
                print(serialdata)
                conn.send(serialdata)
          else:
                break
 
while True:
    # global serialdata
#Accepting incoming connections
    conn, addr = sock.accept()
    print >>sys.stderr,'Connection from:', addr
    print >>sys.stderr,'client address:',addr[0]
#Creating new thread. Calling clientthread function for this function and passing conn as argument.
    start_new_thread(clientthread,(conn,addr)) #start new thread takes 1st argument as a function name to be run, second is the tuple of arguments to the function.
 
conn.close()
sock.close()
