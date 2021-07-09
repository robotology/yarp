/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarpmin.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#ifdef WIN32
#define SOCK_CAST(x) ((SOCKET)x)
#else
#define SOCK_CAST(x) ((int)x)
#endif

#if WIN32
#define safe_printf sprintf_s
#else
#define safe_printf snprintf
#endif

static int yarp_is_initialized = 0;
static yarpAddress yarp_server;


static int file_exists(const char *fname) {
    struct stat s;
    int result = stat(fname,&s);
    return result==0;
}

int find_name_server(int verbose) {
    char path[1000];
    char buf[1000];
    char current_namespace[1000] = "/root";
    char server_filename[1000] = "yarp.conf";
    char full_server_filename[1000];
    char namespace_filename[1000];
    FILE *fin;
    int i;
    int len;
    int in_group = 0;
    int group = 0;
    const char *first;
    const char *second;
#ifdef WIN32
    const char *sep = "\\";
#else
    const char *sep = "/";
#endif
    // =================================================================
    // WARNING: This code is deprecated and will no longer work,
    //          see ResourceFinder documentation for paths searched
    //          by YARP
    if (getenv("YARP_CONF")!=NULL) {
        safe_printf(path,sizeof(path),"%s",getenv("YARP_CONF"));
    } else if (getenv("HOMEDIR")!=NULL) {
        safe_printf(path,sizeof(path),"%s%s%s%s%s",
                    getenv("HOMEDIR"),sep,"yarp",sep,"conf");
    } else if (getenv("HOME")!=NULL) {
        safe_printf(path,sizeof(path),"%s%s%s%s%s",
                    getenv("HOME"),sep,".yarp",sep,"conf");
    } else {
        if (verbose) {
            printf("Please set YARP_CONF to the location reported by this command:\n");
            printf("  yarp conf\n");
        }
        return -1;
    }
    // =================================================================

    if (verbose) {
        fprintf(stderr,"YARP config file should be present in %s\n", path);
    }
    safe_printf(namespace_filename,sizeof(namespace_filename),"%s%s%s",
                path,sep,"yarp_namespace.conf");
    if (file_exists(namespace_filename)) {
        fin = fopen(namespace_filename,"r");
        if (fin==NULL) {
            if (verbose) {
                fprintf(stderr,"Cannot read %s\n", namespace_filename);
            }
            return -1;
        }
        fgets(current_namespace,sizeof(current_namespace),fin);
        fclose(fin);
        if (current_namespace[0]=='\"') {
            int len = strlen(current_namespace)+1;
            int at = 0;
            int i = 0;
            for (i=0; i<len; i++) {
                if (current_namespace[i]!='\"') {
                    if (at!=i) {
                        current_namespace[at] = current_namespace[i];
                    }
                    at++;
                }
            }
        }
        if (verbose) {
            fprintf(stderr,"Namespace set to %s\n", current_namespace);
        }
        if (strcmp(current_namespace,"/root")!=0) {
            for (i=0; i<strlen(current_namespace)+1; i++) {
                char ch = current_namespace[i];
                if (ch<32) {
                    ch = '\0';
                }
                server_filename[i] = (ch=='/')?'_':ch;
            }
            strncat(server_filename,".conf",sizeof(server_filename));
        }
    } else {
        if (verbose) {
            fprintf(stderr,"Using default namespace %s\n", current_namespace);
        }
    }
    safe_printf(full_server_filename,sizeof(full_server_filename),
                "%s%s%s",path,sep,server_filename);
    if (verbose) {
        fprintf(stderr,"Expect name server information in %s\n",
                server_filename);
    }
    fin = fopen(full_server_filename,"r");
    if (fin==NULL) {
        if (verbose) {
            fprintf(stderr,"Cannot read %s\n", full_server_filename);
        }
        return -1;
    }
    fgets(buf,sizeof(buf),fin);
    fclose(fin);
    len = strlen(buf);
    for (i=0; i<len; i++) {
        if (buf[i]==' '||buf[i]=='\t') {
            buf[i] = '\0';
            in_group = 0;
        } else {
            if (!in_group) {
                if (group==0) {
                    first = buf+i;
                } else if (group==1) {
                    second = buf+i;
                }
                group++;
                in_group = 1;
            }
        }
    }
    if (first==NULL||second==NULL) {
        if (verbose) {
            fprintf(stderr,"Cannot parse %s\n", full_server_filename);
        }
        return -1;
    }
    strncpy(yarp_server.host,first,sizeof(yarp_server.host));
    yarp_server.port_number = atoi(second);
    if (verbose) {
        fprintf(stderr,"Name server should be at %s:%d\n",yarp_server.host,
                yarp_server.port_number);
    }
    if (yarp_server.port_number==0) {
        fprintf(stderr,"Oops, that does not look right...\n");
        return -1;
    }
    return 0;
}

yarpAddressPtr yarp_init() {
    return yarp_init_with(NULL,0);
}


yarpAddressPtr yarp_init_with(const char *name_server_host,
                              int name_server_socket_port) {
    int res;
#ifdef WIN32
    int     wsaRc;
    WSADATA wsaData;
#endif
    if (yarp_is_initialized) {
        return &yarp_server;
    }
#ifdef WIN32
    if(wsaRc=WSAStartup(0x0101, &wsaData)) {
        perror("WinSock init");
    }
    if(wsaData.wVersion != 0x0101) {
        WSACleanup();
        perror("wsaData.wVersion");
    }
#endif
    if (name_server_host==NULL) {
        strncpy(yarp_server.host,"127.0.0.1",sizeof(yarp_server.host));
    } else {
        strncpy(yarp_server.host,name_server_host,sizeof(yarp_server.host));
    }
    if (name_server_socket_port<=0) {
        yarp_server.port_number = 10000;
        res = find_name_server(0);
        if (res<0) {
            find_name_server(1);
            exit(1);
        }
    } else {
        yarp_server.port_number = name_server_socket_port;
    }
    yarp_is_initialized = 1;
    return &yarp_server;
}


void yarp_fini() {
}


yarpConnection yarp_connect(yarpAddressPtr address) {
#ifdef WIN32
    SOCKET sd;
#else
    int sd;
#endif
    //struct sockaddr_in sin;
    struct sockaddr_in pin;
    struct hostent *hp;

    // get host information
    if ((hp = gethostbyname(address->host)) == 0) {
        perror("gethostbyname");
        exit(1);
    }

    // set up socket structure
    memset(&pin, 0, sizeof(pin));
    pin.sin_family = AF_INET;
    pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
    pin.sin_port = htons(address->port_number);

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

    return (long int) sd;
}


int yarp_is_valid(yarpConnection connection) {
#ifdef WIN32
    return (connection != INVALID_SOCKET);
#else
    return (connection != -1);
#endif
}


void yarp_disconnect(yarpConnection connection) {
    if (yarp_is_valid(connection)) {
#ifdef WIN32
        closesocket(SOCK_CAST(connection));
#else
        close(SOCK_CAST(connection));
#endif
    }
}


int yarp_send(yarpConnection connection, const char *msg) {
    return yarp_send_binary(connection,msg,strlen(msg));
}


int yarp_send_binary(yarpConnection connection, const char *msg, int len) {
    // send a message to the server PORT on machine HOST
    if (send(SOCK_CAST(connection), msg, len, 0) == -1) {
        perror("send");
        exit(1);
    }
    return 0;
}


int yarp_receive(yarpConnection connection, char *buf, int len) {
    int res = recv(SOCK_CAST(connection), buf, len, 0);
    return res;
}


int yarp_receive_lines(yarpConnection connection, char *buf, int len,
                       char **pending_buf, int *pending_len) {
    int i;
    int res = 0;
    char *start = buf;
    if (*pending_len) {
        memmove(buf,*pending_buf,*pending_len);
        res = *pending_len;
    }
    while (res>=0) {
        for (i=0; i<res; i++) {
            if (buf[i]=='\r') {
                buf[i] = '\0';
            }
            if (buf[i]=='\n') {
                buf[i] = '\0';
                *pending_buf = buf+i+1;
                *pending_len = (res - i - 1);
                return *pending_buf-start;
            }
        }
        buf += res;
        len -= res;
        res = yarp_receive(connection,buf,len);
        if (res==0) break;
    }
    *pending_buf = NULL;
    *pending_len = 0;
    if (len>0) buf[0] = '\0';
    return res;
}

int yarp_receive_line(yarpConnection connection, char *buf, int len) {
    char *pending_buf = NULL;
    int pending_len = 0;
    return yarp_receive_lines(connection,buf,len,&pending_buf,&pending_len);
}

int yarp_receive_binary(yarpConnection connection, char *buf, int len) {
    int i;
    int res = 0;
    while (len>0) {
        res = yarp_receive(connection,buf,len);
        if (res<=0) {
            res = -1;
            break;
        }
        buf += res;
        len -= res;
    }
    return res;
}


int yarp_parse_registration(yarpAddressPtr address, char *buf) {
    int in_group = 0;
    int expect_ip = 0;
    int expect_port_number = 0;
    int got_ip = 0;
    int got_port_number = 0;
    char txt[1000];
    char at = 0;
    int i;
    address->host[0] = '\0';
    address->port_number = -1;
    if (strncmp(buf,"registration name ",18)!=0) {
        return -1;
    }
    for (i=0; i<strlen(buf)+1; i++) {
        char ch = buf[i];
        if (ch!=' '&&ch!='\t'&&ch!='\r'&&ch!='\n'&&ch!='\0') {
            in_group = 1;
            if (at<sizeof(txt)-1) {
                txt[at] = ch;
                at++;
                txt[at] = '\0';
            }
        } else {
            if (in_group) {
                if (expect_ip) {
                    strncpy(address->host,txt,sizeof(address->host));
                    got_ip = 1;
                }
                if (expect_port_number) {
                    address->port_number = atoi(txt);
                    got_port_number = 1;
                }
                in_group = 0;
                expect_ip = 0;
                expect_port_number = 0;
                if (strcmp(txt,"ip")==0) {
                    expect_ip = 1;
                } else if (strcmp(txt,"port")==0) {
                    expect_port_number = 1;
                }
                at = 0;
                txt[at] = '\0';
            }
        }
    }

    if (!(got_ip&&got_port_number)) {
        return -1;
    }

    return 0;
}


int yarp_port_lookup(yarpAddressPtr address, const char *port_name) {
    char query[1000];
    char buf[1000];
    int res;
    safe_printf(query,sizeof(query),"query %s", port_name);
    yarp_rpc(&yarp_server, query, buf, sizeof(buf));
    return yarp_parse_registration(address,buf);
}


int yarp_rpc(yarpAddressPtr address, const char *msg, char *buf, int len) {
    yarpConnection con = yarp_connect(address);
    char ibuf[1000];
    if (!yarp_is_valid(con)) {
        printf("Cannot open connection\n");
        exit(1);
    }
    yarp_send(con,"CONNACK cclient\n");
    yarp_receive_line(con,ibuf,sizeof(ibuf));
    yarp_send(con,"d\n");
    yarp_send(con,msg);
    yarp_send(con,"\n");
    int res = yarp_receive_line(con,buf,len);
    yarp_disconnect(con);
    return res;
}


yarpConnection yarp_prepare_to_read(yarpAddressPtr address) {
    yarpConnection con = yarp_connect(address);
    char ibuf[1000];
    if (!yarp_is_valid(con)) {
        printf("Cannot open connection\n");
        exit(1);
    }
    yarp_send(con,"CONNECT cclient\n");
    yarp_receive_line(con,ibuf,sizeof(ibuf));
    yarp_send(con,"r\n"); // reverse the connection
    return con;
}

yarpConnection yarp_prepare_to_read_binary(yarpAddressPtr address) {
    yarpConnection con = yarp_connect(address);
    char ibuf[1000];
    if (!yarp_is_valid(con)) {
        printf("Cannot open connection\n");
        exit(1);
    }

    // Following tcp protocol documented at:
    //   http://www.yarp.it/yarp_protocol.html

    // Send header to select connection type.
    // this header is for fast_tcp, so we don't have to deal with flow control
    char hdr[8] = {'Y','A',0x64, 0x1E, 0, 0,'R','P'};
    yarp_send_binary(con,hdr,8);

    // Send name of our port - there is none, so send a name that
    // does not start with "/"
    char port[8] = {4,0,0,0,'m','i','n',0};
    yarp_send_binary(con,port,8);
    hdr[7] = '\0';

    // Check for acknowledgement
    yarp_receive_binary(con,hdr,8);
    if (hdr[7]!='P') {
        printf("Cannot make connection handshake\n");
        exit(1);
    }

    // Send header for payload (a command to reverse connection)
    char load_hdr[8] = {'Y','A',10, 0, 0, 0,'R','P'};
    yarp_send_binary(con,load_hdr,8);
    char load_hdr2[10] = { 1, 1, 255, 255, 255, 255, 255, 255, 255, 255 };
    yarp_send_binary(con,load_hdr2,10);
    char load_len[4] = { 8, 0, 0, 0 };
    yarp_send_binary(con,load_len,4);
    char reply_len[4] = { 0, 0, 0, 0 };
    yarp_send_binary(con,reply_len,4);

    // send data - this is a command to reverse the connection for reading
    char command_reverse[8] = { 0, 0, 0, 0, '~', 'r', 0, 1 };
    yarp_send_binary(con,command_reverse,8);

    return con;
}

int yarp_receive_data_header(yarpConnection con) {
    int i;
    char load_hdr_ref[8] = {'Y','A',10, 0, 0, 0,'R','P'};
    char load_hdr[8] = {0,0,0,0,0,0,0,0};
    yarp_receive_binary(con,load_hdr,8);
    for (i=0; i<8; i++) {
        if (load_hdr[i]!=load_hdr_ref[i]) {
            printf("Unexpected data received");
            exit(1);
        }
    }
    char load_hdr2[10];
    load_hdr2[1] = 0;
    yarp_receive_binary(con,load_hdr2,10);
    if (load_hdr2[1]!=1) {
        printf("Corrupt data received");
        exit(1);
    }
    int blocks = load_hdr2[0];
    int len = 0;
    unsigned char load_len[4];
    int port_message_len = 0;
    for (i=0; i<blocks; i++) {
        yarp_receive_binary(con,(char *)load_len,4);
        len += load_len[0] + (load_len[1]<<8) + (load_len[2]<<16) +
            (load_len[3]<<24);
        if (i==0) {
            port_message_len = len;
        }
    }
    yarp_receive_binary(con,load_len,4);
    for (i=0; i<4; i++) {
        if (load_len[i]!=0) {
            printf("Unexpected lengths received");
            exit(1);
        }
    }

    // extract the port header part
    char command_header[8];
    command_header[4] = '\0';
    yarp_receive_binary(con,command_header,8);
    len -= 8;
    port_message_len -= 8;
    if (command_header[4]!='~') {
        printf("Unexpected port command received\n");
        exit(1);
    }
    char cmd[256] = "?";
    cmd[0] = command_header[5];
    if (cmd[0]=='\0') {
        if (port_message_len>sizeof(cmd)) {
            printf("Port command too big\n");
            exit(1);
        }
        yarp_receive_binary(con,cmd,port_message_len);
        len -= port_message_len;
    }
    if (cmd[0]!='d') {
        printf("Unexpected port command : %s\n", cmd);
        exit(1);
    }

    return len;
}


int yarp_read_int(const char *buf, int len) {
    int x;
    unsigned char *ubuf = (unsigned char *)buf;
    if (len!=4) {
        printf("Cannot read integers with %d bytes\n", len);
        return 0;
    }
    // this could be optimized away on little-endian machines!
    x = ubuf[0] + (ubuf[1]<<8) + (ubuf[2]<<16) + (ubuf[3]<<24);
    return x;
}


int yarp_write_int(char *buf, int len, int x) {
    unsigned char *ubuf = (unsigned char *)buf;
    if (len!=4) {
        printf("Cannot write integers with %d bytes\n", len);
        return -1;
    }
    // this could be optimized away on little-endian machines!
    ubuf[0] = x&0xff;
    ubuf[1] = (x>>8)&0xff;
    ubuf[2] = (x>>16)&0xff;
    ubuf[3] = (x>>24)&0xff;
    return 0;
}
