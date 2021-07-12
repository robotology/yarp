/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define YARP_ADDRESS_HOST_MAXLEN 100

typedef struct yarpAddressStruct {
  char host[YARP_ADDRESS_HOST_MAXLEN];
  int port_number;
} yarpAddress;
typedef yarpAddress *yarpAddressPtr;

typedef long int yarpConnection;

/* initialize YARP, find name server */
yarpAddressPtr yarp_init();

/* initialize YARP with a predetermined name server */
yarpAddressPtr yarp_init_with(const char *name_server_host,
                              int name_server_socket_port);

/* shut down YARP */
void yarp_fini();

/* connect to a YARP port */
yarpConnection yarp_connect(yarpAddressPtr address);

/* check if connection is valid */
int yarp_is_valid(yarpConnection connection);

/* shut down connection */
void yarp_disconnect(yarpConnection connection);

/* send text across a connection */
int yarp_send(yarpConnection connection, const char *msg);

/* send bytes across a connection */
int yarp_send_binary(yarpConnection connection, const char *msg, int len);

/* receive unprocessed bytes from a connection */
int yarp_receive(yarpConnection connection, char *buf, int len);

/* receive a string from a connection */
int yarp_receive_line(yarpConnection connection, char *buf, int len);

/* receive a series of strings from a connection.  The pending_buf and
   pending_len should be initially NULL/0.  Keep calling until
   return value is <= 0
*/
int yarp_receive_lines(yarpConnection connection, char *buf, int len,
                       char **pending_buf, int *pending_len);

/* receive payload bytes (user data, no YARP meta data) from a connection */
int yarp_receive_binary(yarpConnection connection, char *buf, int len);

/* fetch a standard header from a connection */
int yarp_receive_data_header(yarpConnection connection);

/* interpret a registration lookup string */
int yarp_parse_registration(yarpAddressPtr address, char *buf);

/* look up the address of a port */
int yarp_port_lookup(yarpAddressPtr address, const char *port_name);

/* send and receive a string */
int yarp_rpc(yarpAddressPtr address, const char *msg, char *buf, int len);

/* read an integer */
int yarp_read_int(const char *buf, int len);

/* write an integer */
int yarp_write_int(char *buf, int len, int x);

/* set up connection for reading text-mode data */
yarpConnection yarp_prepare_to_read(yarpAddressPtr address);

/* set up connection for reading binary-mode data */
yarpConnection yarp_prepare_to_read_binary(yarpAddressPtr address);
