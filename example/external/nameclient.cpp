#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

// port number for yarp server
#define PORT        10000

// IP address or host name for yarp server
#define HOST        "127.0.0.1"

// buffer size for responses from name server
#define BUFSIZE     1000

int main(int argc, char *argv[]) {
	char buf[BUFSIZE];
	int  sd, i;
	struct sockaddr_in sin;
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

	close(sd);
}

 
