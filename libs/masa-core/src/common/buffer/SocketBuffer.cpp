/*******************************************************************************
 *
 * Copyright (c) 2010, 2013   Edans Sandes
 *
 * This file is part of CUDAlign.
 * 
 * CUDAlign is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CUDAlign is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CUDAlign.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "SocketBuffer.hpp"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <errno.h>

SocketBuffer::SocketBuffer(string hostname, int port) {
    this->hostname = hostname;
    this->port = port;
    this->socketfd = -1;
}


SocketBuffer::~SocketBuffer() {
    if (socketfd != -1) {
        fprintf(stderr, "Buffer::destroy()...\n");
        Buffer::destroy();
        fprintf(stderr, "Close Socket...\n");
        //close(socketfd);
        //Buffer::joinThreads();
    }
}

void SocketBuffer::autoFlushThread() {
    char data[512];
    
    int pos = 0;
    while (!isDestroyed()) {
        int len = readBuffer(data, 1, sizeof(data));
        pos += len;
        int ret = send(socketfd, data, len, 0);
        if (ret == -1) {
            fprintf(stderr, "send: Socket error: -1\n");
            break;
        }
        if (ret != len) {
            fprintf(stderr, "Socket error: Sanity Check. Only %d bytes sent (< %d)\n", ret, len);
            break;
        }

    }
    ::close(socketfd);
    socketfd = -1;
    fprintf(stderr, "End of flush data: %d bytes\n", pos);
}

void SocketBuffer::autoLoadThread() {
    char data[512];
    int pos = 0;
    while (!isDestroyed()) {
        int len = recv(socketfd, data, sizeof(data), 0);
        fprintf(stderr, "recv(): %d\n", len);
        if (len == -1) {
            fprintf(stderr, "recv: Socket error -1\n");
            break;
        }
        if (len == 0) {
            break;
        }
        pos += len;
        writeBuffer(data, 1, len);
    }
    ::close(socketfd);
    socketfd = -1;
    fprintf(stderr, "Socket closed by peer: %d bytes\n", pos);
}

void SocketBuffer::initAutoLoad() {
    int rc;
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    


    /* Create a reliable, stream socket using TCP */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "ERROR creating socket: %s\n", strerror(errno));
		sleep(1);
	}

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(hostname.c_str());   /* Server IP address */
    echoServAddr.sin_port        = htons(port); /* Server port */

    /* Establish the connection to the echo server */
    int max_retries = 10;
    int retries = 0;
    int ok = 0;
    while ((retries < max_retries) && !ok) {
		if ((rc=connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr))) < 0) {
			retries++;
			fprintf(stderr, "ERROR connecting to Server [Retry %d/%d]. %s\n",
					retries, max_retries, strerror(errno));
			sleep(1);
		} else {
			ok = 1;
		}
	}
	if (!ok) {
		fprintf(stderr, "ERROR connecting to Server. Aborting\n");
		exit(-1);
	}
    fprintf(stderr, "Connected to Server %s\n", inet_ntoa(echoServAddr.sin_addr));
    
    this->socketfd = sock;
}

void SocketBuffer::destroyAutoLoad() {
    ::close(socketfd);
}

void SocketBuffer::initAutoFlush() {
    int rc;
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int clntLen;            /* Length of client address data structure */
    
    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "ERROR creating server socket; return code from socket() is %d\n", servSock);
        exit(-1);
    }
    
    int optval = 1;
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    
    
    
    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(port);      /* Local port */
    
    /* Bind to the local address */
    if ((rc = bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr))) < 0) {
        fprintf(stderr, "ERROR; return code from bind() is %d\n", rc);
        exit(-1);
    }
    
    /* Mark the socket so it will listen for incoming connections */
    if ((rc=listen(servSock, 1)) < 0) {
        fprintf(stderr, "ERROR; return code from listen() is %d\n", rc);
        exit(-1);
    }
    
    /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);
    
    /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0){
        fprintf(stderr, "ERROR; return code from accept() is %d\n", clntSock);
        exit(-1);
    }
    
    /* clntSock is connected to a client! */
    
    fprintf(stderr, "Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
    
    //HandleTCPClient(clntSock);
    
    ::close(servSock);
    
    this->socketfd = clntSock;
}

void SocketBuffer::destroyAutoFlush() {

}
