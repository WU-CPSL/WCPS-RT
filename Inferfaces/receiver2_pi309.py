import serial
from time import sleep
import sys
import socket
import time
import os
from datetime import datetime

s=serial.Serial('/dev/ttyUSB1', 115200, timeout = 0.1)
while True:
	if not s.open:
		s=serial.Serial('/dev/ttyUSB1', 115200, timeout = 0.1)
	# data=s.read(1024)
	data=s.readline()
	# print data
	# print s.read(1024)
	if data:
		timeT=datetime.now().strftime('%Y,%m,%d,%H,%M,%S.%f')
		sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		server_address = ('172.20.166.72',14000)
		sock.connect(server_address)
		try:
			sock.sendall(data)
		finally:
			sock.close()
	 	print data[0:26],",",timeT
	else:
	 	continue
