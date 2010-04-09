// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#define YARP_ADDRESS_HOST_MAXLEN 100

typedef struct yarpAddressStruct {
  char host[YARP_ADDRESS_HOST_MAXLEN];
  int port_number;
} yarpAddress;
typedef yarpAddress *yarpAddressPtr;

typedef long int yarpConnection;

yarpAddressPtr yarp_init();

void yarp_fini();

yarpConnection yarp_connect(yarpAddressPtr address);
int yarp_is_valid(yarpConnection connection);
void yarp_disconnect(yarpConnection connection);

int yarp_send(yarpConnection connection, const char *msg);
int yarp_receive(yarpConnection connection, char *buf, int len);
int yarp_receive_line(yarpConnection connection, char *buf, int len);

int yarp_parse_registration(yarpAddressPtr address, char *buf);
int yarp_port_lookup(yarpAddressPtr address, const char *port_name);

int yarp_rpc(yarpAddressPtr address, const char *msg, char *buf, int len);

yarpConnection yarp_prepare_to_read(yarpAddressPtr address);
