#!/usr/bin/env python
# -*- coding:utf-8 -*-

from socket import *  
from time import ctime  
  

HOST = ''
PORT = 10025
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpSerSock = socket(AF_INET, SOCK_STREAM)
#socket.setReuseAddress(true);
tcpSerSock.bind(ADDR)
tcpSerSock.listen(5)

while True:
    print 'waiting for connection...'
    tcpCliSock, addr = tcpSerSock.accept()
    print '...connected from:', addr

    while True:
        data = tcpCliSock.recv(BUFSIZ)
        if not data:
            break
        tcpCliSock.send(data)
    tcpCliSock.close()

tcpSerSock.close() 
