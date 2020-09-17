import serial
from time import sleep
import sys
import socket
import time
import os


s=serial.Serial('/dev/ttyUSB3', 115200, timeout = 0.1)
while True:
	if not s.open:
		s=serial.Serial('/dev/ttyUSB3', 115200, timeout = 0.1)#receiver node 1
	# data=s.read(1024)
	data=s.readline()
	# print data
	# print s.read(1024)
	if data:
		sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
		# server_address = ('macbook-pro.seas.wustl.edu',8000)
		server_address = ('172.16.21.31',14000)
		sock.connect(server_address)
		try:
			sock.sendall(data)
		finally:
			sock.close()
	 	print data
	else:
	 	continue
