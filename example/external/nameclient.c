/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

// If you are on Windows, make sure WIN32 is defined.
// otherwise, we assume UNIX.

// This simple demo program doesn't deal with fragmentation of tcp
// reads/writes.  If you're doing something serious, make sure you
// deal with cases where socket reads/writes are only partially completed.
// in one call.

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

// port number for yarp server
#define PORT        10000

// IP address or host name for yarp server
#define HOST        "127.0.0.1"

// buffer size for responses from name server
#define BUFSIZE     1000

#ifdef WIN32
void windowsNetStart(void) {
    int     wsaRc;
    WSADATA wsaData;
    if(wsaRc=WSAStartup(0x0101, &wsaData)) {
        perror("WinSock init");
    }
    if(wsaData.wVersion != 0x0101) {
        WSACleanup();
        perror("wsaData.wVersion");
    }
}
#endif


int main(int argc, char *argv[]) {
    char buf[BUFSIZE];
    int i;
#ifdef WIN32
    SOCKET sd;
#else
    int sd;
#endif
    //struct sockaddr_in sin;
    struct sockaddr_in pin;
    struct hostent *hp;

    if (argc<=1) {
        printf("Please supply a message to send to the nameserver. ");
        printf(" Examples:\n");
        printf("   help\n");
        printf("   list\n");
        printf("   query /portname\n");
        exit(1);
    }

#ifdef WIN32
    // start windows networking
    windowsNetStart();
#endif

    // get host information
    if ((hp = gethostbyname(HOST)) == 0) {
        perror("gethostbyname");
        exit(1);
    }

    // set up socket structure
    memset(&pin, 0, sizeof(pin));
    pin.sin_family = AF_INET;
    pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    pin.sin_port = htons(PORT);

    // get a socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // connect to PORT on HOST
    if (connect(sd,(struct sockaddr *)  &pin, sizeof(pin)) == -1) {
        perror("connect");
        exit(1);
    }

    // Commands sent to the yarp server must begin with
    // "NAME_SERVER " and be terminated with "\n"
    buf[0] = '\0';
    strncat(buf,"NAME_SERVER",sizeof(buf));
    for (i=1; i<argc; i++) {
        strncat(buf," ",sizeof(buf));
        strncat(buf,argv[i],sizeof(buf));
    }
    strncat(buf,"\n",sizeof(buf));
    printf("Message to send to name server:\n");
    printf("%s", buf);

    // send a message to the server PORT on machine HOST
    if (send(sd, buf, strlen(buf), 0) == -1) {
        perror("send");
        exit(1);
    }

    // wait for a message to come back from the server
    for (i=0; i<BUFSIZE; i++) {
        buf[i] = '\0';
    }
    if (recv(sd, buf, BUFSIZE, 0) == -1) {
        perror("recv");
        exit(1);
    }

    // print out the results
    printf("Response from yarp server:\n");
    printf("%s", buf);

#ifdef WIN32
    closesocket(sd);
#else
    close(sd);
#endif
}
