#!/usr/bin/env python
# _*_ coding: utf-8 _*_

#《Python 网络编程基础 》第二个小例子，获取用户输入的域名和内容
#发送到对应服务器 新增使用makefile 获取read write 方式发送接收网络内容
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

fd = s.makefile("rw",0)

fd.write(filename + "\r\n")

for line in fd.readlines():
	print(line)

