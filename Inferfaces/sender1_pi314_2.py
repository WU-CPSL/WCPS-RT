#!/usr/bin/env python
import sys
import socket
import os
import time
import serial
from datetime import datetime
# pi 314_2
# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('172.20.166.70', 10000)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
sock.listen(1)
while True:
    connection, client_address = sock.accept()
   # print >>sys.stderr,'Connection from:', client_address
    try:
      while True:
            data=connection.recv(16)      
            if data:
		  timeT=datetime.now().strftime('%Y,%m,%d,%H,%M,%S.%f')
                  print data,",",timeT 
                  ser=serial.Serial('/dev/ttyUSB0',115200,timeout=1)
                  #print(ser.name)
                  ser.write(data.encode())
                  ser.close

            else:
                  break

    finally:
        connection.close()
        
