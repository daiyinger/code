#!/usr/bin/env python
# _*_ coding: utf-8 _*_

#《Python 网络编程基础 》第一个小例子，获取用户输入的域名和内容
#发送到对应服务器 然后打印服务器的返回内容 接触新增函数 socket.sendall
#使用示例 python gopherClient.py www.smeoa.com get

import socket,sys

port = 80
host = sys.argv[1]
filename = sys.argv[2]

s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
try:
	s.connect((host,port))
except socket.gaierror,e:
	print("Error connecting to server %s" % e)
	sys.exit(1)
#s.sendall(filename + "\r\n")	#Use Type 1
s.send(filename+"\r\n")		#Use Type 2

while True:
	buf = s.recv(2048)
	if not len(buf):
		break
	#sys.stdout.write(buf)	#Use Type 1
	print(buf)		#Use Type 2

